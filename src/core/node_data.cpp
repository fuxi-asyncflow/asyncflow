#include "core/node_data.h"
#include "core/node_func.h"
#include "util/log.h"

#include <string>
#include <unordered_map>
#include "core/manager.h"

using namespace asyncflow::core;
using namespace asyncflow::util;
using std::string;

NodeData::~NodeData()
{
	delete node_func_;
}

bool NodeData::InitFromJson(rapidjson::Value& jobj, const std::unordered_map<std::string, int>& map, ChartData* chart_data)
{
	//get uid
	auto uid = jobj.FindMember("uid");
	if (uid != jobj.MemberEnd())
	{
		node_uid_ = uid->value.GetString();
	}

	//get text
	auto text = jobj.FindMember("text");
	if (text != jobj.MemberEnd())
	{
		text_ = text->value.GetString();
	}

	//get funcname
	auto funcName = jobj.FindMember("funcName");
	std::string func_name;
	if (funcName != jobj.MemberEnd())
	{
		func_name = funcName->value.GetString();
	}

	//get type
	auto type = jobj.FindMember("type");
	if (type != jobj.MemberEnd())
	{
		auto funcType = type->value.GetString();
		if (strcmp(funcType, "function") == 0)
		{
			node_func_ = chart_data->CreateNodeFunc("", func_name);
		}
		else if (strcmp(funcType, "event") == 0)
		{
			node_func_ = chart_data->CreateNodeFunc("", func_name);
			is_event_ = true;
		}
		else if (strcmp(funcType, "control") == 0)
		{
			auto params = jobj.FindMember("param");
			if (params != jobj.MemberEnd())
			{
				auto paramsArray = params->value.GetArray();
				std::vector<int> paramsVec;
				for (auto it = paramsArray.begin(); it != paramsArray.end(); ++it)
				{
					paramsVec.push_back(map.find(it->GetString())->second);
				}
				if (func_name == "wait_all")
				{
					node_func_ = NodeInnerFunc::CreateInnerFunc(&Manager::WaitAll, paramsVec);
				}
				else if (func_name == "stop_node")
				{
					node_func_ = NodeInnerFunc::CreateInnerFunc(&Manager::StopNode, paramsVec);
				}
				else if (func_name == "stop_flow")
				{
					node_func_ = NodeInnerFunc::CreateInnerFunc(&Manager::StopFlow, paramsVec);
				}
				else
				{
					ASYNCFLOW_ERR("control node can not create funcname{0}!", func_name);
				}
			}
		}
		else
		{
			ASYNCFLOW_ERR("invalid functype{0}!", funcType);
		}
	}

	// get subchartvar
	auto subChartVarObj = jobj.FindMember("subChartVar");
	if (subChartVarObj != jobj.MemberEnd())
	{
		var_id_ = subChartVarObj->value.GetInt();
	}

	// get subsequence
	auto successObj = jobj.FindMember("success");
	if (successObj != jobj.MemberEnd())
	{
		auto successArray = successObj->value.GetArray();
		for (auto it = successArray.begin(); it != successArray.end(); ++it)
		{
			children_[1].push_back(map.find(it->GetString())->second);
		}
	}

	auto failObj = jobj.FindMember("fail");
	if (failObj != jobj.MemberEnd())
	{
		auto failArray = failObj->value.GetArray();
		for (auto it = failArray.begin(); it != failArray.end(); ++it)
		{
			children_[0].push_back(map.find(it->GetString())->second);
		}
	}
	return true;
}

void NodeData::SetChildren(const std::vector<int>& f, const std::vector<int>& s)
{
	children_[0].clear();
	children_[0].insert(children_[0].end(), f.begin(), f.end());
	children_[1].clear();
	children_[1].insert(children_[1].end(), s.begin(), s.end());
}