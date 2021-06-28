#include "js_chartdata.h"
#include "js_nodefunc.h"

using namespace asyncflow::js;

NodeFunc* JsChartData::CreateNodeFunc(const std::string& code, const std::string& name)
{
	return JsNodeFunc::GetFuncFromString(code, name);
}