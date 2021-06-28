#pragma once
#include "rapidjson/prettywriter.h"
#include <vector>
using namespace asyncflow::core;

namespace asyncflow
{
	namespace debug
	{
		struct ChartInfo
		{
			int			agent_id{ -1 };
			void*		owner_node_addr{ (void*)-1 };
			int			owner_node_id{ -1 };
			std::string object_name{ "" };
			std::string chart_name{ "" };
			std::string owner_chart_name{ "" };

			template <typename Writer>
			void Serialize(Writer& writer) const
			{
				writer.StartObject();

				writer.String("agent_id");
				writer.Int(agent_id);

				writer.String("owner_node_addr");
				writer.Uint64((uint64_t)owner_node_addr);

				writer.String("owner_node_id");
				writer.Int(owner_node_id);

				writer.String("object_name");
				writer.String(object_name.c_str());

				writer.String("chart_name");
				writer.String(chart_name.c_str());

				writer.String("owner_chart_name");
				writer.String(owner_chart_name.c_str());
				writer.EndObject();
			}

			template <typename Value>
			void Deserialize(const Value& obj)
			{
				if (obj.HasMember("agent_id") && obj["agent_id"].IsInt())
					this->agent_id = obj["agent_id"].GetInt();
				if (obj.HasMember("owner_node_addr") && obj["owner_node_addr"].IsInt64())
					this->owner_node_addr = (void*)obj["owner_node_addr"].GetUint64();
				if (obj.HasMember("owner_node_id") && obj["owner_node_id"].IsInt())
					this->owner_node_id = obj["owner_node_id"].GetInt();
				if (obj.HasMember("object_name") && obj["object_name"].IsString())
					this->object_name = obj["object_name"].GetString();
				if (obj.HasMember("chart_name") && obj["chart_name"].IsString())
					this->chart_name = obj["chart_name"].GetString();
				if (obj.HasMember("owner_chart_name") && obj["owner_chart_name"].IsString())
					this->owner_chart_name = obj["owner_chart_name"].GetString();
			}
		};

		struct DebugData
		{
			enum DataType
			{
				NodeStatus = 1,
				VariableStatus = 2,
				EventStatus = 3
			};
			int id;
			DataType type;
			virtual ~DebugData(){}
		};

		struct NodeStatusData : public DebugData
		{
			NodeStatusData(int id, std::string uid, int ostatus, int nstatus, bool res)
				:node_id(id), node_uid(uid), old_status(ostatus), new_status(nstatus), result(res)
			{
				type = NodeStatus;
			}
			
			NodeStatusData() :
				node_id(-1), node_uid(""), old_status(-1), new_status(-1), result(false)
			{ type = NodeStatus; }

			int node_id;
			std::string node_uid;
			int old_status;
			int new_status;
			bool result;

			template <typename Writer>
			void Serialize(Writer& writer) const
			{
				writer.StartObject();

				writer.String("type");
				writer.String("node_status");

				writer.String("id");
				writer.Int(id);

				writer.String("node_id");
				writer.Int(node_id);

				writer.String("node_uid");
				writer.String(node_uid.c_str());

				writer.String("old_status");
				writer.Int(old_status);

				writer.String("new_status");
				writer.Int(new_status);

				if (new_status == 2)   //节点现在的状态为完成状态发送运行结果
				{
					writer.String("result");
					writer.Bool(result);
				}
				writer.EndObject();
			}

			template <typename Value>
			void Deserialize(const Value& obj)
			{
				if (obj.HasMember("id") && obj["id"].IsInt())
					this->id = obj["id"].GetInt();
				if (obj.HasMember("node_id") && obj["node_id"].IsInt())
					this->node_id = obj["node_id"].GetInt();
				if (obj.HasMember("node_uid") && obj["node_uid"].IsString())
					this->node_uid = obj["node_uid"].GetString();
				if (obj.HasMember("old_status") && obj["old_status"].IsInt())
					this->old_status = obj["old_status"].GetInt();
				if (obj.HasMember("new_status") && obj["new_status"].IsInt())
					this->new_status = obj["new_status"].GetInt();
				if (obj.HasMember("result") && obj["result"].IsBool())
					this->result = obj["result"].GetBool();
			}
		};

		struct VariableStatusData : public DebugData
		{
			VariableStatusData(std::string name, std::string node_uid, std::string ovalue, std::string nvalue)
				: variable_name(name), node_uid(node_uid), old_value(ovalue), new_value(nvalue)
			{
				type = VariableStatus;
			}

			VariableStatusData()
				: variable_name(""), node_uid(""), old_value(""), new_value("")
			{
				type = VariableStatus;
			}

			std::string variable_name;
			std::string node_uid;
			std::string old_value;
			std::string new_value;

			template <typename Writer>
			void Serialize(Writer& writer) const
			{
				writer.StartObject();

				writer.String("type");
				writer.String("variable_status");

				writer.String("id");
				writer.Int(id);

				writer.String("variable_name");
				writer.String(variable_name.c_str());

				writer.String("node_uid");
				writer.String(node_uid.c_str());

				writer.String("old_value");
				writer.String(old_value.c_str());

				writer.String("new_value");
				writer.String(new_value.c_str());

				writer.EndObject();
			}

			template <typename Value>
			void Deserialize(const Value& obj)
			{
				if (obj.HasMember("id") && obj["id"].IsInt())
					this->id = obj["id"].GetInt();
				if (obj.HasMember("variable_name") && obj["variable_name"].IsString())
					this->variable_name = obj["variable_name"].GetString();
				if (obj.HasMember("node_uid") && obj["node_uid"].IsString())
					this->node_uid = obj["node_uid"].GetString();
				if (obj.HasMember("old_value") && obj["old_value"].IsString())
					this->old_value = obj["old_value"].GetString();
				if (obj.HasMember("new_value") && obj["new_value"].IsString())
					this->new_value = obj["new_value"].GetString();
			}


		};

		struct EventStatusData : public DebugData
		{
			EventStatusData(std::string node_uid, std::string event_name, std::vector<std::string> nargs, int argc)
				: event_name(event_name), node_uid(node_uid), n_args(nargs), argcount(argc)
			{
				type = EventStatus;
			}

			EventStatusData()
				: event_name(""), node_uid(""), n_args(std::vector<std::string>()), argcount(-2)
			{
				type = EventStatus;
			}

			std::string event_name;
			std::string node_uid;
			std::vector<std::string> n_args;
			int argcount;

			template <typename Writer>
			void Serialize(Writer& writer) const
			{
				writer.StartObject();

				writer.String("type");
				writer.String("event_status");

				writer.String("id");
				writer.Int(id);

				writer.String("event_name");
				writer.String(event_name.c_str());

				writer.String("node_uid");
				writer.String(node_uid.c_str());

				writer.String("arg_count");
				writer.Int(argcount);

				writer.String("event_params");
				writer.StartArray();
				for (int i = 0; i < argcount; i++)
				{
					writer.String(n_args[i].c_str());
				}
				writer.EndArray();

				writer.EndObject();
			}

			template <typename Value>
			void Deserialize(const Value& obj)
			{
				if (obj.HasMember("id") && obj["id"].IsInt())
					this->id = obj["id"].GetInt();
				if (obj.HasMember("event_name") && obj["event_name"].IsString())
					this->event_name = obj["event_name"].GetString();
				if (obj.HasMember("node_uid") && obj["node_uid"].IsString())
					this->node_uid = obj["node_uid"].GetString();
				if (obj.HasMember("arg_count") && obj["arg_count"].IsInt())
					this->argcount = obj["arg_count"].GetInt();
				if (obj.HasMember("event_params") && obj["event_params"].IsArray())
				{
					auto array = obj["event_params"].GetArray();
					n_args.clear();
					for (const auto& arg: array)
					{
						if (arg.IsString())
							n_args.push_back(arg.GetString());
					}
				}
			}
		};

	}
}