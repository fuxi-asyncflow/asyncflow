#pragma once
#include <vector>
#include <string>
#include <cstdint>
using namespace asyncflow::core;

namespace asyncflow
{
	namespace debug
	{
		struct ChartInfo
		{
			ChartInfo()
				: agent_id(UINT64_MAX)
				, owner_node_addr((void*)-1)
				, owner_node_id(-1)
			{
			}
			uint64_t	agent_id;
			void*		owner_node_addr;
			int			owner_node_id;
			std::string owner_node_uid;
			std::string object_name;
			std::string chart_name;
			std::string owner_chart_name;
			std::string chart_uid;
		};

		struct DebugData
		{
			enum DataType
			{
				NodeStatus = 1,
				VariableStatus = 2,
				EventStatus = 3,
				NodeMessage = 4
			};
			int id;
			DataType type;
			virtual ~DebugData(){}
		};

		struct NodeStatusData : public DebugData
		{
			NodeStatusData(int id, const std::string& uid, int ostatus, int nstatus, bool res)
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
		};

		struct NodeMessageData : public DebugData
		{
			NodeMessageData(int id, const std::string& uid, const char* msg, int msg_length)
				: node_id(id), node_uid(uid), message(msg, msg_length)
			{
				type = NodeMessage;
			}
			int node_id;
			std::string node_uid;
			std::string message;
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
		};

	}
}