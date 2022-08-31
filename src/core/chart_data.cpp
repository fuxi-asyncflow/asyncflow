#include "core/chart_data.h"
#include "util/log.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/document.h>
#include "core/node_func.h"
#include <unordered_map>

using namespace asyncflow::core;
using namespace asyncflow::util;

const std::string _error_var_name = "__error";

ChartData::ChartData()
	: variable_count_(0)
	, params_count_(0)
	, version_(0)
	, prev_(nullptr)
	, next_(nullptr)
#ifdef FLOWCHART_DEBUG
	, quick_debug_(false)
#endif
{

}

bool ChartData::InitParamsFromJson(rapidjson::Value& paramsObj)
{
	auto const varArray = paramsObj.GetArray();
	variables_.clear();
	int paramsCount = 0;
	for (auto* it = varArray.begin(); it != varArray.end(); ++it)
	{
		auto name = it->FindMember("name")->value.GetString();
		auto type = it->FindMember("type")->value.GetString();
		auto isParams = it->FindMember("is_param")->value.GetBool();
		if (isParams)
			++paramsCount;
		variables_.emplace_back(name, type, isParams);
	}
	params_count_ = paramsCount;
	return true;
}

bool ChartData::FromJson(rapidjson::Value& jobj)
{
	ChartData* chartData = this;
	//read full path of chart
	auto const chartFullPath = jobj.FindMember("path");
	if (chartFullPath == jobj.MemberEnd())
	{
		ASYNCFLOW_ERR("missing chart Path");		
		return false;
	}
	const std::string fullPath = chartFullPath->value.GetString();

	//read variable info
	auto varCountObj = jobj.FindMember("varCount");
	int varCount = (varCountObj == jobj.MemberEnd()) ? 0 : varCountObj->value.GetInt();

	auto varObj = jobj.FindMember("variables");
	if(varObj != jobj.MemberEnd())
	{
		bool r = chartData->InitParamsFromJson(varObj->value);
		if( r == false )
		{
			ASYNCFLOW_ERR("read parameters error in chart {0}", fullPath);			
			return false;
		}
	}

	//read nodes data	
	auto nodesArrayIter = jobj.FindMember("nodes");
	if (nodesArrayIter == jobj.MemberEnd())
	{
		ASYNCFLOW_ERR("missing nodes in chart {0}", fullPath);		
		return false;
	}

	//read start nodes
	auto startList = jobj.FindMember("start");
	if (startList == jobj.MemberEnd())
	{
		ASYNCFLOW_ERR("missing start in chart {0}", fullPath);		
		return false;
	}

	chartData->chart_name_ = fullPath;
	chartData->variable_count_ = varCount;
	const auto nodesArray = nodesArrayIter->value.GetArray();
	int nodeId = 1;

	//get the relationship between id and uid
	std::unordered_map<std::string, int> id_map;
	for (auto it = nodesArray.begin(); it != nodesArray.end(); ++it)
	{
		std::string uid = it->FindMember("uid")->value.GetString();
		id_map[uid] = nodeId++;
	}

	auto* startNode = new NodeData(0);
	const auto startArray = startList->value.GetArray();

	for (auto it = startArray.begin(); it != startArray.end(); ++it)
	{
		startNode->GetSubsequenceIds(false).push_back(id_map[it->GetString()]);
		startNode->GetSubsequenceIds(true).push_back(id_map[it->GetString()]);
	}
	// read init function name, the function will be called at the beginning of start chart
	auto initFuncName = jobj.FindMember("start_func");
	if (initFuncName != jobj.MemberEnd())
	{
		start_func_name_ = initFuncName->value.GetString();
		auto* func = CreateNodeFunc("", start_func_name_);
		startNode->SetNodeFunc(func);
	}
	//add start node
	chartData->node_list_.push_back(startNode);

	nodeId = 1;
	for (auto* it = nodesArray.begin(); it != nodesArray.end(); ++it)
	{
		auto* nodeData = new NodeData(nodeId++);
		if (nodeData->InitFromJson(*it, id_map, this) == false)
		{			
			return false;
		}
		chartData->node_list_.push_back(nodeData);
	}
	return true;
}

ChartData::~ChartData()
{
	if (prev_ != nullptr && prev_ != this)
	{
		delete prev_;
		prev_ = nullptr;
	}

	for (auto* node_data : node_list_)
	{
		delete node_data;
	}
	
	node_list_.clear();
}

void ChartData::Update(ChartData* new_data)
{
	new_data->prev_ = this;
	this->next_ = new_data;
	new_data->SetVersion(version_ + 1);
}

void ChartData::SetNodes(std::vector<NodeData*>& nodes)
{
	node_list_.clear();
	for (auto* node : nodes)
	{
		node_list_.push_back(node);
	}
}

NodeData* ChartData::GetNodeData(const std::string& uid)
{
	const auto iter = std::find_if(node_list_.begin(), node_list_.end()
		, [&uid](NodeData* n) {return n->GetUid() == uid; });
	if (iter == node_list_.end())
		return nullptr;
	return *iter;
}

const std::string& ChartData::GetVariableName(int idx) const
{
	if (idx < 0 || idx >= static_cast<int>(variables_.size()))
	{
		ASYNCFLOW_WARN("get chart variable name error: variable index {0} out of range {1}", idx, variables_.size());
		return _error_var_name;
	}
		
	return variables_[idx].name;
}

const Parameter* ChartData::GetVariable(int idx) const
{
	if (idx < 0 || idx >= static_cast<int>(variables_.size()))
	{
		ASYNCFLOW_WARN("get chart variable name error: variable index {0} out of range {1}", idx, variables_.size());
		return nullptr;
	}

	return &variables_[idx];
}

