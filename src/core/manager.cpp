#define RYML_SINGLE_HDR_DEFINE_NOW
#include "rapidyaml.hpp"
#include "core/manager.h"

#include <unordered_map>
#include "rapidjson/document.h"

#include "util/log.h"
#include "core/node.h"
#include "util/file.h"
#include "util/json.h"
#include "core/node_attacher.h"
#include "core/node_func.h"
#include <set>
#include <queue>
#include <fstream>
#include <functional>

using namespace asyncflow::core;
using namespace asyncflow::util;

const int Manager::DEFAULT_AGENT_TICK = 1000;
const int Manager::MAX_EVENT_FRAME = 16;

DataManager Manager::dataManager = DataManager();
EventManager Manager::eventManager = EventManager();

Manager::Manager()
	: current_frame_(0)	
	, current_event_(nullptr)
	, default_time_interval_(100)
	, defer_event_(true)
    , AUTO_REGISTER(true)
	, rd("default")
	, current_event_frame_(0)
#ifdef FLOWCHART_DEBUG
	, websocket_manager_(this)
#endif
{	
	chart_data_dict_ = dataManager.GetChartData();
}

Manager::~Manager()
{	
	current_event_ = nullptr;
#ifdef ENABLE_PERF
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);	
	std::stringstream ss;
	ss << "asyncflow_perf_" << std::to_string(in_time_t) << ".txt";
	
	FILE* f;
	fopen_s(&f, ss.str().c_str(), "wt");
	if (f != nullptr)
	{
		for (auto it = chart_data_dict_->begin(); it != chart_data_dict_->end(); ++it)
		{
			auto* chartData = it->second;
			fprintf(f, "%s\n", chartData->Name().c_str());			
			for (int i = 0; i < chartData->GetNodeCount(); ++i)
			{
				auto* nodeData = chartData->GetNodeData(i);
				auto run_count = nodeData->GetRunCount();
				auto time_cost = nodeData->GetTimeCost();
				fprintf(f, "%d %d %lld\n", i, run_count, time_cost.count());
			}
		}
		fclose(f);
	}
#endif
	
}

void Manager::Step(int milliseconds)
{
	ASYNCFLOW_DBG("========== step {0} ==========", Now());
	in_step_ = true;
	//time count is the milliseconds from last step, must > 0
	auto const time_count = milliseconds >= 0 ? milliseconds : default_time_interval_;

	//timer
	timer_manager_.Step(time_count);

#ifdef FLOWCHART_DEBUG
	websocket_manager_.Step();
#endif	
	
	async_manager_.Step();

	//handle event
	event_queue_.Switch();
	auto* ev = event_queue_.NextEvent();
	while(ev != nullptr)
	{
		HandleEvent(*ev);
		delete ev;
		ev = event_queue_.NextEvent();
	}

	//deregister_obj agent, must after event handling
	dying_agents_.Step();
	
	current_frame_++;
	in_step_ = false;
	return;
}

bool Manager::Event(int event_id, Agent* agent, void* args, int arg_count, bool immediate, bool trigger)
{
	auto const event_arg_count = eventManager.GetEventArgsCount(event_id);
	if (event_arg_count < 0)
	{
		ASYNCFLOW_WARN("invalid event id {0}", event_id);
		return false;
	}

	if (agent == nullptr || agent->GetStatus() == Agent::Destroying)
	{
		ASYNCFLOW_WARN("event object is not registered to asyncflow");
		return false;
	}

	AsyncEventBase* event = CreateAsyncEvent(event_id, agent, args, arg_count);

	ASYNCFLOW_DBG("raise event {0} for agent {1} [{2}-{3}]", (void*)event, (void*)agent, event_id, eventManager.GetEventName(event_id));
	if(trigger && TriggerEvent(*event))
	{
		delete event;
	}
	else
	    event_queue_.AddEvent(event, immediate);
	
	return true;
}

//attach a chart to an object.
Chart* Manager::AttachChart(Agent* agent, const std::string& chart_name)
{
	auto* chart_data = GetChartData(chart_name);
	if (chart_data == nullptr)
	{
		ASYNCFLOW_WARN("cannot find chart {0} in AttachChart", chart_name);
		return nullptr;
	}
	return agent->AttachChart(chart_data);	
}

//returns the ChartData corresponding to the chart_name, or nullptr.
ChartData* Manager::GetChartData(const std::string& chart_name)
{
	auto const it = chart_data_dict_->find(chart_name);
	if (it == chart_data_dict_->end())
		return nullptr;
	return it->second;
}

//import the chart info with either a filename or a JSON string as an argument
int Manager::ImportFile(const std::string& file_name)
{
    auto str = file_name;
    if(File::Exists(file_name))
    {
        str = File::ReadAllText(file_name);
    }
	//import JSON string
	rapidjson::Document doc;
	rapidjson::ParseResult ok = doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(str.c_str());
    if (ok)
        return ImportJson(str);
    else
        return ImportYaml(str);
}

//read chart info
int Manager::ImportJson(const std::string& json_str)
{
	return ImportChatData(ParseChartsFromJson(json_str));
}

//read chart info
int Manager::ImportYaml(const std::string& yaml_str)
{
    return ImportChatData(ParseChartsFromYaml(yaml_str));
}

int	Manager::ImportChatData(const std::vector<ChartData*>& data_list)
{
	int count = 0;
	for (auto* data : data_list)
	{
		if (ReloadChartData(data))
			count++;
	}
	return count;
}

std::vector<ChartData*> Manager::ParseChartsFromJson(const std::string& yaml_str)
{	
	rapidjson::Document doc;
	std::vector<ChartData*> data_list;
	if (JsonUtil::ParseJson(yaml_str, doc))
	{
		if (doc.IsArray())
		{
			for (auto& chart_obj : doc.GetArray())
			{
				auto* data = CreateChartData();
				if (!data->FromJson(chart_obj))
				{
					ASYNCFLOW_ERR("init chart data error");
					delete data;
				}
				else
				{
					data_list.push_back(data);
				}
			}
			return data_list;
		}
	}
	ASYNCFLOW_ERR("import graphs failed: not a valid json");
	return data_list;
}

std::vector<ChartData*> Manager::_ParseChartsYaml(const std::string& yaml_str, std::function<ChartData* (const c4::yml::ConstNodeRef& doc)> handler)
{
	std::vector<ChartData*> data_list;
	asyncflow::util::YamlErrorHandler errh;

	//TODO error handle
	ryml::set_callbacks(errh.callbacks());

	try
	{
		ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(yaml_str));
		const ryml::NodeRef root = tree.rootref();
		if (root.is_stream())
		{
			auto doc_count = root.num_children();
			for (const auto& doc : root.children())
			{
				auto path = doc["path"].val();
				// printf("yaml chart : %.*s\n", static_cast<int>(path.size()), path.data());

				auto* data = handler(doc);
				if(data != nullptr)
					data_list.push_back(data);				
			}
			return data_list;
		}
	}
	catch (std::runtime_error& e)
	{
		ASYNCFLOW_ERR("{0}", e.what());
	}

	ASYNCFLOW_ERR("import graphs failed: not a valid yaml");
	return data_list;
}

std::vector<ChartData*> Manager::ParseChartsFromYaml(const std::string& yaml_str)
{
	return _ParseChartsYaml(yaml_str,
		[this](const c4::yml::ConstNodeRef& doc)-> ChartData*
		{
			auto* data = CreateChartData();
			if (!data->FromYaml(doc))
			{
				ASYNCFLOW_ERR("init chart data error");
				delete data;
				return nullptr;
			}
			return data;
		});
}

int	Manager::PatchFromYaml(const std::string& yaml_str, bool in_place)
{
	auto data_list = _ParseChartsYaml(yaml_str,
		[this, in_place](const c4::yml::ConstNodeRef& doc)-> ChartData*
		{
			auto path = doc["path"];
			if (!path.valid())
			{
				ASYNCFLOW_ERR("missing chart Path");
				return nullptr;
			}
			const std::string fullPath = std::string(path.val().data(), path.val().size());
			auto* data = GetChartData(fullPath);

			if(data != nullptr && in_place)
			{
				if (data->PatchFromYaml(doc))
				{
					ASYNCFLOW_LOG("patch chart {0} success!", fullPath);
					return data;
				}
				ASYNCFLOW_ERR("patch chart {0} failed!", fullPath);
				return data;
			}
			else
			{
				data = CreateChartData();
				if (data->FromYaml(doc))
				{
					ASYNCFLOW_LOG("patch new chart {0}", fullPath);
					ReloadChartData(data);
					return data;
				}
				else
				{
					ASYNCFLOW_ERR("init chart data error");
					delete data;
					return nullptr;
				}
			}
		});

	return data_list.size();
}

bool Manager::ReloadChartData(ChartData* new_data) const
{
	if (new_data == nullptr) return false;

	auto it = chart_data_dict_->find(new_data->Name());
	if (it != chart_data_dict_->end())
	{
		it->second->Update(new_data);
		it->second = new_data;
	}
	else
	{
		chart_data_dict_->insert(std::make_pair(new_data->Name(), new_data));
	}
	return true;
}

void Manager::RestartChart(const std::string& chart_name)
{
	const auto& agents = GetAgentManager().GetAgents();
	auto* chart_data = GetChartData(chart_name);
	for (const auto& agentkv : agents)
	{
		auto* agent = agentkv.second;
		auto* chart = agent->FindChart(chart_name, nullptr);
		if (chart != nullptr)
		{
			agent->RemoveChart(chart_name);
			agent->AttachChart(chart_data);
			agent->StartChart(chart_name);
		}
	}
}

int Manager::ImportEvent(const std::string& file_name)
{
    auto str = file_name;
    if(File::Exists(file_name))
    {
        str = File::ReadAllText(file_name);
        if (str.empty())
        {
            ASYNCFLOW_WARN("file is empty or cannot read {0}", file_name);
            return 0;
        }
    }

	rapidjson::Document doc;
	rapidjson::ParseResult ok = doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(file_name.c_str());
	if (ok)
		return eventManager.InitFromJson(str);
	return eventManager.InitFromYaml(str);
}

void Manager::_handleEvent(AsyncEventBase& ev, Agent::NodeList& node_list)
{
	current_event_ = &ev;
	// As the node runs, new nodes may be added to the list, so a copy is created.
	ASYNCFLOW_DBG("handle event {0} for agent {1} [{2}-{4}], total {3} nodes",
		(void*)&ev, (void*)this, ev.Id(), node_list.size(), GetEventManager().GetEventName(ev.Id()));

	while (!node_list.IsEmpty())
	{
		// The agent of the node may be different from the agent which the current event belongs to, eg $obj.OnSee.
		auto* node = node_list.Pop();
		auto* agent = node->GetAgent();
		node->SetStatus(Node::Idle);
#ifdef FLOWCHART_DEBUG
		node->SendEventStatus(&ev);
#endif
		RunFlow(node);
	}
	current_event_ = nullptr;
	// assert(waiting_nodes->empty());	
	// Assert may be failed: As subchart called, the start node of the subchart is also waiting for the start event and is placed in the same list, which cannot be cleared
}

void Manager::HandleEvent(AsyncEventBase& ev)
{
	auto* waiting_nodes = ev.GetWaitingNodes(*this);
	if (waiting_nodes == nullptr || waiting_nodes->IsEmpty())
		return;
	NodeLinkedList node_list;
	node_list.swap(*waiting_nodes);
	
	_handleEvent(ev, node_list);
	
}

bool Manager::TriggerEvent(AsyncEventBase& ev)
{
	auto* waiting_nodes = ev.GetWaitingNodes(*this);
	if (waiting_nodes == nullptr || waiting_nodes->IsEmpty())
		return true;
	
	if(current_event_frame_ >= MAX_EVENT_FRAME)	
		return false;
	current_event_frame_++;
	// use function stack as stack of node list and current event
	auto* prev_ev = current_event_;

	NodeLinkedList node_list;
	node_list.swap(*waiting_nodes);

    // handle event
	_handleEvent(ev, node_list);

	// pop
	current_event_frame_--;
	current_event_ = prev_ev;
	return true;
}

void Manager::Wait(int milliseconds)
{
	if (milliseconds <= 0)
	{
		milliseconds = 1;
	}
	auto* node = GetCurrentNode();
	if (node == nullptr)
	{
		ASYNCFLOW_WARN("wait function should be called inside asyncflow node");
		return;
	}

	auto* attacher = node->GetAttacher();
	//existing timer
	if (attacher != nullptr && attacher->GetType() == INodeAttacher::TIMER)
	{
		auto* old_timer = static_cast<NodeTimer*>(node->GetAttacher());
		old_timer->Register(node, milliseconds);
	}
	else
	{
		auto* timer = new NodeTimer(node, milliseconds);
		node->SetAttacher(timer);
	}

	//time is up, the node is not needed to run.
	node->SetSkip(true);
	node->SetResult(true);
	node->SetStatus(Node::Running);
}

bool Manager::WaitAll(const std::vector<int>& args)
{
	auto* node = GetCurrentNode();
	node->SetWaitAllFlag(true);
	auto* wait_all = (NodeWaitAll*)node->GetAttacher();
	if (wait_all == nullptr)
	{
		wait_all = new NodeWaitAll(node);
		wait_all->Init(args);
		node->SetAttacher(wait_all);
	}
	bool result = wait_all->Run(node->GetPreNodeId());
	if (!result)
	{
		node->SetStatus(Node::Running);
	}
	else
	{
		node->SetStatus(Node::EndRun);
		node->SetResult(rTRUE);
	}
	return result;
}

bool Manager::StopNode(const std::vector<int>& args)
{
	auto* chart = GetCurrentNode()->GetChart();
	auto const max_id = chart->GetNodesCount();

	for (auto id: args)
	{		
		if (id >= max_id)
		{
			ASYNCFLOW_WARN("id is out of range when stop node by id");
			continue;
		}

		Node* node = chart->GetNode(id);
		node->Stop();
		executor_.Remove(node);
	}
	return true;
}

bool Manager::StopFlow(const std::vector<int>& args)
{
	Chart* chart = GetCurrentNode()->GetChart();
	auto const max_id = chart->GetNodesCount();
	std::set<int> stop_id;
	std::queue<int> id_queue;
	for (auto id: args)
	{		
		if (id >= max_id)
		{
			ASYNCFLOW_WARN("id is out of range when stop node by id");
			continue;
		}

		Node* node = chart->GetNode(id);
		id_queue.push(node->GetId());
		while (!id_queue.empty())
		{
			int head = id_queue.front();
			id_queue.pop();
			if (stop_id.find(head) == stop_id.end())
			{
				stop_id.insert(head);
				auto* node_data = chart->GetNode(head)->GetData();				
				for (auto c : node_data->GetSubsequenceIds(true))
					id_queue.push(c);
				for (auto c : node_data->GetSubsequenceIds(false))
					id_queue.push(c);
			}
		}
	}
	for (auto c : stop_id)
	{
		auto* node = chart->GetNode(c);
		node->Stop();		
		executor_.Remove(node);
	}
	return true;
}

bool Manager::WaitEvent(Agent* agent, int event_id)
{
	auto* node = GetCurrentNode();
	if (node == nullptr || !node->GetData()->IsEventNode())
		return false;
	node->SetStatus(Node::Running);
	node->SetSkip(true);
	node->SetResult(true);
	agent->WaitEvent(node, event_id);	
	return true;
}

bool Manager::Subchart(const std::string& chart_name, Agent* agent, void* args, int arg_count)
{
	//There are several cases to consider
	//Run for the first time, and loop
	//Whether the object that called the subchart is same as the object of the owner chart
	//Whether the object called again is same as the object called before
	if (agent == nullptr) return false;
	auto* node = GetCurrentNode();
	//Since the node status has already been checked in runflow, the running status check is deleted

	auto* subchart = static_cast<core::Subchart*>(node->GetAttacher());
	ChartData* chart_data = nullptr;
	chart_data = GetChartData(chart_name);
	if (chart_data == nullptr)
	{
		ASYNCFLOW_WARN("cannot find chart {0} in Subchart", chart_name);
		return false;
	}
	Chart* chart = nullptr;
	if (subchart != nullptr)
	{
		auto* prev_chart_agent = subchart->GetChart()->GetAgent();
		if (prev_chart_agent == agent)
		{
			// reuse the previous subchart
			chart = subchart->GetChart();
			if (chart_data != chart->GetData())
			{
				node->SetAttacher(nullptr);
				chart = nullptr;
				ASYNCFLOW_DBG("use new data in chart {0}", chart_name);
			}
		}
		else
		{
			assert(subchart->GetType() == INodeAttacher::SUBCHART);
			node->SetAttacher(nullptr);
		}
	}

	if (chart == nullptr)
	{
		chart = CreateChart();
		chart->Init(chart_data, node);
		agent->AddChart(chart, node);

		//create subchart
		subchart = new core::Subchart(node);
		subchart->SetChart(chart);
		node->SetAttacher(subchart);
	}
	//A start node can only be used in the asynchronous way.
	//Thus, if the chart does not have a return node, it also can be return.
	node->SetStatus(Node::Running);
	agent->StartChart(chart, true, args, arg_count);
	
	ASYNCFLOW_DBG("start subchart {0} {1}", chart_name, (void*)chart);
	return true;
}

int64_t Manager::CreateAsyncContext()
{
	auto* node = GetCurrentNode();
	if(node == nullptr)
	{
		ASYNCFLOW_ERR("create asyncflow context failed: function is not called inside node");
	}
	else
	{
		node->SetStatus(Node::Running);
		async_manager_.AddNode(node);
	}	
	return reinterpret_cast<int64_t>(node);
}

bool Manager::ContinueAsyncNode(int64_t context)
{
	auto node = (core::Node*)context;
	if (async_manager_.IsNodeWaiting(node))
	{
		ASYNCFLOW_DBG("async callback for node {0} {1}[{2}]", (void*)node, node->GetChart()->Name(), node->GetId());
		async_manager_.RemoveNode(node);
		async_manager_.ActivateNode(node);
		return true;		
	}

	ASYNCFLOW_WARN("node {0} is not waiting async callback", (void*)node);
	return false;	
}

bool Manager::CancelAsyncNode(int64_t context, bool stop)
{
	auto node = (core::Node*)context;
	if (async_manager_.IsNodeWaiting(node))
	{
		ASYNCFLOW_DBG("async callback for node {0} {1}[{2}]", (void*)node, node->GetChart()->Name(), node->GetId());
		async_manager_.RemoveNode(node);
		if (!stop)
		{
			async_manager_.ActivateNode(node);
			node->SetResult(false);
		}
		return true;
	}
	
    ASYNCFLOW_WARN("node {0} is not waiting async callback", (void*)node);	
	return false;
}

bool Manager::UnregisterGameObject(Agent* agent)
{
	if (agent == nullptr)
	{
		ASYNCFLOW_LOG("try to unregister object which not registered to asyncflow");
		return false;
	}
	if (agent->GetStatus() == Agent::Destroying)
		return true;
	ASYNCFLOW_LOG("unregister object from asyncflow");
	agent->SetStatus(Agent::Destroying);
	agent->Stop();
	GetAgentManager().UnRegister(agent);
	dying_agents_.AddDyingAgent(agent, in_step_);	
	return true;
}

std::string Manager::uuid4_str()
{
	char uustr[] = "00000000000000000000000000000000";
	constexpr char encode[] = "0123456789abcdef";

	auto tmp = (dist(rd) & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;

	size_t bit = 15;
	for (size_t i = 0; i < 16; i++) 
	{		
		uustr[i] = encode[tmp >> 4 * bit & 0x0f];
		bit--;
	}

	tmp = (dist(rd) & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
	bit = 15;
	for (size_t i = 16; i < 32; i++) 
	{		
		uustr[i] = encode[dist(rd) >> 4 * bit & 0x0f];
		bit--;
	}	
	return std::string(uustr);
}


#ifdef FLOWCHART_DEBUG
std::vector<asyncflow::debug::ChartInfo*> Manager::GetDebugChartList(const std::string& object_name, const std::string& chart_name)
{	
	bool const filter_object = !object_name.empty();
	bool const filter_chart = !chart_name.empty();
	//std::string tmp_str = object_name;
	auto const obj_search_str = object_name;
	auto const chart_search_str = chart_name;
	ASYNCFLOW_LOG("GetDebugChartList obj_name: {0} chart_name: {1}", object_name, chart_name);
	std::vector<asyncflow::debug::ChartInfo*> chart_list;
	for (auto& kv : GetAgentManager().GetAgents())
	{
		Agent* agent = kv.second;
		std::string agent_name = agent->GetName();
		if (filter_object && agent_name.find(obj_search_str) == std::string::npos)
		{
			continue;
		}

		for (auto& chart_kv : agent->GetChartDict())
		{
			auto& name_of_chart = chart_kv.first;
			if (filter_chart && name_of_chart.find(chart_search_str) == std::string::npos)
			{
				continue;
			}

			for (auto* chart : chart_kv.second)
			{
				auto* ci = new debug::ChartInfo();
				ci->agent_id = agent->GetId();
				ci->owner_node_addr = chart->GetOwnerNode();
				if (ci->owner_node_addr != nullptr)
				{
					auto* owner_node = chart->GetOwnerNode();
					ci->owner_node_id = owner_node->GetId();
					ci->owner_node_uid = owner_node->GetData()->GetUid();
					ci->owner_chart_name = owner_node->GetChart()->Name();
				}
				else
				{
					ci->owner_node_id = -1;
					ci->owner_node_uid = "";
					ci->owner_chart_name = "";
				}

				ci->object_name = agent_name;
				ci->chart_name = name_of_chart;
				ci->chart_uid = chart->GetUid();
				chart_list.push_back(ci);
			}
		}
	}
	return chart_list;
}

std::vector<Chart*> Manager::GetChartsByName(const std::string& chart_name)
{
	std::vector<Chart*> result;
	for (auto kv : GetAgentManager().GetAgents())
	{
		auto chart_kv = kv.second->GetChartDict();
		auto it = chart_kv.find(chart_name);
		if (it != chart_kv.end())
		{
			result.insert(result.end(),it->second.begin(), it->second.end());
		}
	}
	return result;
}

bool Manager::SetBreakpoint(NodeData* data)
{
	auto* func = dynamic_cast<BreakpointFunc*>(data->GetNodeFunc());
	if (func != nullptr)
	{
		ASYNCFLOW_WARN("Set Breakpoint error, the node is already a breakpoint.");
		return false;
	}
	func = new BreakpointFunc(data->GetNodeFunc());
	ASYNCFLOW_DBG("set breakpoint function for nodedata");
	data->SetNodeFunc(func);
	return true;
}

bool Manager::DeleteBreakpoint(NodeData* data)
{
	auto* func = dynamic_cast<BreakpointFunc*>(data->GetNodeFunc());
	if (func == nullptr)
	{
		ASYNCFLOW_WARN("Delete Breakpoint error, the node is not a breakpoint.");
		return false;
	}
	auto* original_func = func->GetOriginalFunc();
    func->SetOriginalNull();
	data->SetNodeFunc(original_func);
	delete func;
	async_manager_.ContinueBreakpoint(data);
	return true;
}

#endif
