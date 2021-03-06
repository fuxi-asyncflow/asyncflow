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

bool NodeData::InitFromJson(rapidjson::Value& jobj, const std::unordered_map<std::string, int>& id_map, ChartData* chart_data)
{
	//get uid
	auto const uid = jobj.FindMember("uid");
	if (uid != jobj.MemberEnd())
	{
		node_uid_ = uid->value.GetString();
	}

	//get text
	auto const text = jobj.FindMember("text");
	if (text != jobj.MemberEnd())
	{
		text_ = text->value.GetString();
	}

	//get funcname
	auto const funcName = jobj.FindMember("funcName");
	std::string func_name;
	if (funcName != jobj.MemberEnd())
	{
		func_name = funcName->value.GetString();
	}

	//get type
	auto const type = jobj.FindMember("type");
	if (type != jobj.MemberEnd())
	{
		auto* funcType = type->value.GetString();
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
				auto const paramsArray = params->value.GetArray();
				std::vector<int> paramsVec;
				for (auto* it = paramsArray.begin(); it != paramsArray.end(); ++it)
				{
					auto iter = id_map.find(it->GetString());
					if(iter != id_map.end())
					{
						paramsVec.push_back(iter->second);
					}
					else
					{
						ASYNCFLOW_ERR("load node data error in {0}[{1}]({2}), param contains invalid uid"
							, chart_data->Name(), this->GetId(), this->GetUid());
					}
				}
				if (func_name == "wait_all")
				{
					node_func_ = ControlNodeFunc::Create(&Manager::WaitAll, paramsVec);
				}
				else if (func_name == "stop_node")
				{
					node_func_ = ControlNodeFunc::Create(&Manager::StopNode, paramsVec);
				}
				else if (func_name == "stop_flow")
				{
					node_func_ = ControlNodeFunc::Create(&Manager::StopFlow, paramsVec);
				}
				else
				{
					ASYNCFLOW_ERR("control node can not create funcname {0}!", func_name);
				}
			}
		}
		else
		{
			ASYNCFLOW_ERR("load node data error invalid functype{0}!", funcType);
		}
	}

	// get subchartvar
	auto subChartVarObj = jobj.FindMember("subChartVar");
	if (subChartVarObj != jobj.MemberEnd())
	{
		var_id_ = subChartVarObj->value.GetInt();
	}

	// get subsequence
	std::vector<std::pair<int, std::string>> kvs { {1, "success"}, {0, "fail"} };

	for(auto& kv : kvs)
	{
		auto const& subsequence_name = kv.second;
		auto const child_id = kv.first;
		auto successObj = jobj.FindMember(subsequence_name.c_str());
		if (successObj != jobj.MemberEnd())
		{
			auto successArray = successObj->value.GetArray();
			for (auto* it = successArray.begin(); it != successArray.end(); ++it)
			{
				auto iter = id_map.find(it->GetString());
				if (iter != id_map.end())
				{
					children_[child_id].push_back(iter->second);
				}
				else
				{
					ASYNCFLOW_ERR("load node data error in {0}[{1}]({2}), {3} contains invalid uid"
						, chart_data->Name(), this->GetId(), this->GetUid(), subsequence_name.c_str());
				}
			}
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