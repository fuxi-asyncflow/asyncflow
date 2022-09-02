#pragma once
#include <string>
#include <map>
#include "core/node_data.h"



namespace asyncflow
{
	namespace core
	{
		struct Parameter
		{
			Parameter(const std::string& n, const std::string& t, bool is_params)
				: name(n), type(t), is_params(is_params) {}
			std::string name;
			std::string type;
			bool is_params;
		};

		class ChartData
		{
		public:
			ChartData();
			virtual ~ChartData();
			virtual NodeFunc* CreateNodeFunc(const std::string& code, const std::string& name) = 0;

			const std::string& Name() { return chart_name_; }
			const std::string& StartFuncName() { return start_func_name_; }
			bool FromJson(rapidjson::Value& jobj);
			bool FromYaml(const c4::yml::NodeRef& doc);
			
			int		GetVarCount() { return variable_count_; }
			int		GetParamCount() { return params_count_; }
			const std::vector<Parameter>& GetVariableInfo() { return variables_; }
			const std::string& GetVariableName(int idx) const;
			const Parameter* GetVariable(int idx) const;

			int			GetNodeCount() { return (int)node_list_.size(); }
			NodeData*	GetNodeData(int i) { return node_list_[i]; }
			NodeData*	GetNodeData(const std::string& uid);

			void	Update(ChartData* new_data);
			
#ifdef FLOWCHART_DEBUG
			void SetQuickDebug(bool flag) { quick_debug_ = flag; }
			bool IsQuickDebug() { return quick_debug_; }			
#endif

		private:
			bool	InitParamsFromJson(rapidjson::Value& paramsObj);
			void	SetVersion(int v) { version_ = v; }

		private:
			int variable_count_;
			int params_count_;
			ChartData* prev_;
			ChartData* next_;
			int version_;
			std::vector<Parameter> variables_;
			std::vector<NodeData*> node_list_;
			std::string chart_name_;	
			std::string start_func_name_;
#ifdef FLOWCHART_DEBUG
			bool quick_debug_;		// true means that it directly enters the debugging state in the next time
#endif

		// used by test case
		public:
			void SetNodes(std::vector<NodeData*>& nodes);
			void SetName(const std::string& name) { chart_name_ = name; }
			void SetVarCount(int count) { variable_count_ = count; }
			void SetVariables(const std::vector<Parameter>& variables) { variables_ = variables; }
			void SetParamsCount(int count) { params_count_ = count; }
		};		
	}
}