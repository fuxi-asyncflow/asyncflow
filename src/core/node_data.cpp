#include "core/node_data.h"
#include "core/node_func.h"
#include "util/log.h"
#include "util/file.h"

#include <string>
#include <sstream>
#include <unordered_map>
#include "core/manager.h"

#include "rapidyaml.hpp"

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

bool NodeData::InitFromYaml(c4::yml::ConstNodeRef& nodeRef, std::unordered_map<std::string, int>& id_map, ChartData* chart_data)
{
	auto tmp = nodeRef["uid"].val();
	node_uid_ = std::string{ tmp.data(), tmp.size() };
	id_map[node_uid_] = node_id_;
	if (node_id_ == 0)
		return true;

	auto tmpNode = nodeRef.find_child("text");
	if(tmpNode.valid())
	{
		tmp = tmpNode.val();
		text_ = std::string{ tmp.data(), tmp.size() };
	}

	auto noLoopCheckNode = nodeRef.find_child("no_loop_check");
	if (noLoopCheckNode.valid())
	{
		is_no_loop_check = std::string(noLoopCheckNode.val().str, noLoopCheckNode.val().size()) == "true";
	}

	auto codeRef = nodeRef.find_child("code");

	if(codeRef.valid())
	{
		auto&& typeRef = codeRef.find_child("type");
		auto type_str = std::string(typeRef.val().str, typeRef.val().size());
		if(strcmp("FUNC", type_str.c_str()) == 0 || strcmp("EVENT", type_str.c_str()) == 0)
		{
			auto ret_name = codeRef.find_child("return_var_name");
			if(ret_name.valid())
				var_id_ = chart_data->GetVarIndex(std::string{ ret_name.val().str, ret_name.val().size()});

			auto func_name = codeRef.find_child("func_name");
			auto content = codeRef.find_child("content");
			if(func_name.valid())
			{
				node_func_ = chart_data->CreateNodeFunc(std::string(), std::string(func_name.val().str, func_name.val().size()));
			}
			else if(content.valid())
			{
				tmp = content.val();
				std::stringstream ss;
				ss << "return function(self) \n" << std::string{ tmp.str, tmp.size() } << "\n end";
				node_func_ = chart_data->CreateNodeFunc(ss.str(), "");
			}
			else
			{
				std::stringstream ss;
				char normalUid[32];
				UuidUtil::DtoN(node_uid_.c_str(), normalUid);
				// "AI.test.00000000111122223333444444444444"
				ss << chart_data->Name() << "." << std::string(normalUid, 32);
				node_func_ = chart_data->CreateNodeFunc(std::string(), ss.str());
			}
			
			is_event_ = (strcmp("EVENT", type_str.c_str()) == 0);
		}
		else if(strcmp("CONTROL", type_str.c_str()) == 0)
		{
			auto contentsRef = codeRef.find_child("content");
			if(contentsRef.valid())
			{
				int i = 0;
				std::vector<int> params;
				std::string func_name;
                for (auto&& contentRef : contentsRef)
                {
					if (i == 0)
						func_name = std::string(contentRef.val().str, contentRef.val().size());
					else 
					{
						auto id_str = std::string(contentRef.val().str, contentRef.val().size());
						if(id_str.length() == 32)
						{
							char dst[36];
							UuidUtil::NtoD(id_str.c_str(), dst);
							id_str = std::string(dst, 36);
						}
						auto id = id_map.find(id_str);
						if(id != id_map.end())
						    params.emplace_back(id->second);
						else
						{
							ASYNCFLOW_ERR("load node data error in {0}[{1}]({2}), param contains invalid uid `{3}`"
								, chart_data->Name(), this->GetId(), this->GetUid(), std::string(contentRef.val().str, contentRef.val().size()));
							return false;
						}
					}
					i++;
                }
				if(func_name == "stopflow")
					node_func_ = ControlNodeFunc::Create(&Manager::StopFlow, params);
				else if(func_name == "stopnode")
					node_func_ = ControlNodeFunc::Create(&Manager::StopNode, params);
				else if(func_name == "waitall")
					node_func_ = ControlNodeFunc::Create(&Manager::WaitAll, params);
			}
		}
		else if (strcmp("ERROR", type_str.c_str()) == 0)
		{
			ASYNCFLOW_ERR("load node data error in {0}[{1}]({2}), node type is error"
				, chart_data->Name(), this->GetId(), this->GetUid());
			return false;
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

void NodeData::AddSubsequence(int id, int type)
{
	switch(type)
	{
	case 0:
		children_[0].push_back(id);
		break;
	case 1:
		children_[1].push_back(id);
		break;
	case 2:
		children_[0].push_back(id);
		children_[1].push_back(id);
		break;
	default:
		break;
	}
}