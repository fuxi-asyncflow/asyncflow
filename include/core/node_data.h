#pragma once
#include <vector>
#include "rapidjson/document.h"
#include <unordered_map>
#include <string>
#include <chrono>

namespace c4
{
	namespace yml
	{
		class NodeRef;
		class ConstNodeRef;
	}

}

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
				, is_no_loop_check(false)
				, node_func_(nullptr)
#ifdef FLOWCHART_DEBUG
				, break_point(false)
#endif
#ifdef ENABLE_PERF
				, run_count_(0)
				, time_cost_(0)
#endif

			{
				
			}
			~NodeData();
			bool InitFromJson(rapidjson::Value& jobj, const std::unordered_map<std::string, int>& id_map, ChartData* chart_data);
			bool InitFromYaml(c4::yml::ConstNodeRef& nodeRef, std::unordered_map<std::string, int>& id_map, ChartData* chart_data);
			NodeFunc* GetNodeFunc() const { return node_func_; }
			int	GetId() const { return node_id_; }
			const std::string& GetUid() { return node_uid_; }
			const std::string& GetText() { return text_; }
			std::vector<int>& GetSubsequenceIds(bool result) { return result ? children_[1] : children_[0]; }			
			bool IsEventNode() const { return is_event_; }			
			void SetEventNode(bool b) { is_event_ = b; }
			bool IsNoLoopCheck() const { return is_no_loop_check || is_event_; }
			int GetVarId() const { return var_id_; }
			void AddSubsequence(int id, int type);

		private:
			int node_id_;			
			int var_id_;				//var for subchart
			bool is_event_;
			bool is_no_loop_check;
			NodeFunc* node_func_;
			std::vector<int> children_[3];			
			std::string text_;
			std::string node_uid_;		//save uid for debug
#ifdef ENABLE_PERF
		private:
			int run_count_;
			std::chrono::nanoseconds time_cost_;
		public:
			void AddRunCount() { run_count_++; };
			int GetRunCount() { return run_count_; };
			void AddTimeCost(std::chrono::nanoseconds cur_cost) { time_cost_ += cur_cost; };
			std::chrono::nanoseconds GetTimeCost() { return time_cost_; };
#endif

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
