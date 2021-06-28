#include "py_chartdata.h"
#include "py_nodefunc.h"

using namespace asyncflow::py;
///////////////////////////////////////////////////////////////////////////
PyChartData::~PyChartData()
{
}

NodeFunc* PyChartData::CreateNodeFunc(const std::string& code, const std::string& name)
{
	return PyNodeFunc::GetFuncFromString(code, name);
}

