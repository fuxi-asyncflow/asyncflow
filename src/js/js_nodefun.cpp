#include "core/node_func.h"
#include "js_nodefunc.h"
#include "export_js.h"
#include "js_agent.h"
#include "js_chart.h"
using namespace asyncflow::js;
using namespace asyncflow::core;

bool JsNodeFunc::call(Agent* agent)
{
	int id = ((JsAgent*)agent)->GetRefObject();
	int result;
	result = js_call_func(id, name.c_str());
	if (result == -1)
	{
		ASYNCFLOW_WARN("Error occur in function call");
		auto current_chart = (JsChart*)agent->GetManager()->GetCurrentNode()->GetChart();
		ASYNCFLOW_ERR("run node error in chart {0}", current_chart->Name());
		Chart* temp_chart = current_chart;
		while (temp_chart->GetOwnerNode() != nullptr)
		{
			ASYNCFLOW_ERR("The owner chart of {0} is {1}.", temp_chart->Name(), temp_chart->GetOwnerNode()->GetChart()->Name());
			temp_chart = temp_chart->GetOwnerNode()->GetChart();
		}
		for (int var_id = 0; var_id < current_chart->GetData()->GetVarCount(); var_id++)
		{
			auto js_id = current_chart->GetVar(var_id);
			std::string var = js_to_string(js_id);
			ASYNCFLOW_ERR("var_id is {0}, name is {1}, value is {2}.", var_id, current_chart->GetData()->GetVariableName(var_id), var);
		}
		return false;
	}
	else
		return result;
}

NodeFunc* JsNodeFunc::GetFuncFromString(const std::string& code, const std::string& name)
{
	auto* f = new JsNodeFunc;
	f->name = name;
	return f;
}