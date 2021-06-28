#include "py_agent.h"

#include "py_common.h"

asyncflow::py::PyAgent::PyAgent(Manager* manager, PyObject* game_object)
	: Agent(manager)
	, obj_(game_object)
{
	Py_XINCREF(game_object);
}

asyncflow::py::PyAgent::~PyAgent()
{
	Py_XDECREF(obj_);
}

std::string asyncflow::py::PyAgent::GetName()
{
	return GetObjectName(GetRefObject());	 
}
