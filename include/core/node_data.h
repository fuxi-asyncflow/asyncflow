#pragma once
#include <vector>
#include "rapidjson/document.h"
#include <unordered_map>
#include <string>

namespace asyncflow
{
	namespace core
	{
		class ChartData;
		class NodeFunc;
		class NodeData
		{
		public:
			NodeData(int id)
				: node_id_(id)
				, var_id_(-1)
				, is_event_(false)
				, node_func_(nullptr)
				, text_("")
#ifdef FLOWCHART_DEBUG
				, break_point(false)
#endif

			{
				
			}
			~NodeData();
			bool InitFromJson(rapidjson::Value& jobj, const std::unordered_map<std::string, int>& map, ChartData* chart_data);
			NodeFunc* GetNodeFunc() { return node_func_; }
			int	GetId() { return node_id_; }
			std::string GetUid() { return node_uid_; }
			std::string GetText() { return text_; }
			std::vector<int>& GetSubsequenceIds(bool result) { return result ? children_[1] : children_[0]; }			
			bool IsEventNode() { return is_event_; }
			void SetEventNode(bool b) { is_event_ = b; }
			int GetVarId() { return var_id_; }

		private:
			int node_id_;
			std::string node_uid_;		//save uid for debug
			int var_id_;				//var for subchart
			bool is_event_;
			std::vector<int> children_[3];
			NodeFunc* node_func_;
			std::string text_;

#ifdef FLOWCHART_DEBUG
		public:
			void SetBreakPoint(bool flag) { break_point = flag; }
			bool IsBreakPoint() { return break_point; }
		private:
			bool break_point;		// break off program in debug state
#endif

		// used by test
		public:
			void SetUid(const std::string& uid) { node_uid_ = uid; }
			void SetNodeFunc(NodeFunc* func) { node_func_ = func; }
			void SetIsEvent(bool flag) { is_event_ = flag; }
			void SetChildren(const std::vector<int>& f, const std::vector<int>& s);
		};
	}
}