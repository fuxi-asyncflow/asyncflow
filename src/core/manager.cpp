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

using namespace asyncflow::core;
using namespace asyncflow::util;

const int Manager::DEFAULT_AGENT_TICK = 1000;
std::unordered_map<std::string, asyncflow::core::ChartData*>* DataManager::chart_data_ = nullptr;
DataManager Manager::dataManager = DataManager();
EventManager Manager::eventManager = EventManager();

int BasicAgentManager::AGENT_ID_COUNT = 0;

Manager::Manager()
	: current_frame_(0)	
	, current_event_(nullptr)
	, default_time_interval_(100)
	, immediate_subchart_(false)
#ifdef FLOWCHART_DEBUG
	, websocket_manager_(this)
#endif
{	
	chart_data_dict_ = dataManager.GetChartData();
}

Manager::~Manager()
{	
	current_event_ = nullptr;
	dataManager.ClearChartDataMap(chart_data_dict_);	
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
	for (auto* agent : dying_agents_.GetDyingAgents())
	{
		auto* agt = GetAgentManager().UnRegister(agent);
		if (agt == nullptr)
		{
			ASYNCFLOW_LOG("agent{0} has already destroyed", (void*)agent);
			continue;
		}
		assert(agt == agent);
		delete agent;
	}
	dying_agents_.Step();
	
	current_frame_++;
	in_step_ = false;
	return;
}

bool Manager::Event(int event_id, Agent* agent, void* args, int arg_count, bool immediate)
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
	//import JSON string
	rapidjson::Document doc;
	rapidjson::ParseResult ok = doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(file_name.c_str());
	if (ok)
		return ImportJson(file_name);

	//import filename
	auto const json_str = File::ReadAllText(file_name);
	if (json_str.empty())
	{
		ASYNCFLOW_WARN("file is empty or cannot read {0}", file_name);
		return 0;
	}

	return ImportJson(json_str);
}


//read chart info
int Manager::ImportJson(const std::string& json_str)
{
	return ImportChatData(ParseChartsFromJson(json_str));	
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

std::vector<ChartData*> Manager::ParseChartsFromJson(const std::string& json_str)
{	
	rapidjson::Document doc;
	std::vector<ChartData*> data_list;
	if (JsonUtil::ParseJson(json_str, doc))
	{
		if (doc.IsArray())
		{
			for (auto& chart_obj : doc.GetArray())
			{
				auto* data = CreateChartData();
				if (!data->FromJson(chart_obj))
				{
					ASYNCFLOW_ERR("init chart data error");
				}
				else
				{
					data_list.push_back(data);
				}
			}
			return data_list;
		}
	}
	ASYNCFLOW_ERR("import chars failed: not a valid json");
	return data_list;
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
	rapidjson::Document doc;
	rapidjson::ParseResult ok = doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(file_name.c_str());
	if (ok)
		return eventManager.InitFromJson(file_name);
	auto const json_str = File::ReadAllText(file_name);
	if (json_str.empty())
	{
		ASYNCFLOW_WARN("file is empty or cannot read {0}", file_name);
		return 0;
	}

	return eventManager.InitFromJson(json_str);
}

void Manager::HandleEvent(AsyncEventBase& event)
{
	auto* agent = event.GetAgent();
	current_event_ = &event;
	agent->HandleEvent(event);
	current_event_ = nullptr;
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
	node->SetStatus(Node::Running);
}

bool Manager::WaitAll(span<const int> args)
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
		node->SetResult(true);
	}
	return result;
}

bool Manager::StopNode(span<const int> args)
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

bool Manager::StopFlow(span<const int> args)
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
	agent->WaitEvent(node, event_id);	
	return true;
}

bool Manager::Subchart(std::string chart_name, Agent* agent, void* args, int arg_count)
{
	//There are several cases to consider
	//Run for the first time, and loop
	//Whether the object that called the subchart is same as the object of the owner chart
	//Whether the object called again is same as the object called before
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
	agent->StartChart(chart, true);
	//set args and start
	if (arg_count != 0)
		chart->SetArgs(args, arg_count);
	node->SetStatus(Node::Running);
	ASYNCFLOW_DBG("start subchart {0} {1}", chart_name, (void*)chart);
	return true;
}

int64_t Manager::CreateAsyncContext()
{
	auto* node = GetCurrentNode();
	node->SetStatus(Node::Running);
	async_manager_.AddNode(node);
	return reinterpret_cast<int64_t>(node);
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
	dying_agents_.AddDyingAgent(agent, in_step_);	
	return true;
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
					ci->owner_chart_name = owner_node->GetChart()->Name();
				}
				else
				{
					ci->owner_node_id = -1;
					ci->owner_chart_name = "";
				}

				ci->object_name = agent_name;
				ci->chart_name = name_of_chart;
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
