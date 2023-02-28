#include "py_agent.h"
#include "py_common.h"
#include "export_class.h"

asyncflow::py::PyAgent::PyAgent(Manager* manager, PyObject* game_object)
	: Agent(manager)
	, obj_(game_object)
{
	Py_XINCREF(game_object);
	export_object_ = AgentObject::New(this);
}

asyncflow::py::PyAgent::~PyAgent()
{
	Py_XDECREF(obj_);
	auto* obj = (AgentObject*)(export_object_);
	obj->ptr = nullptr;
	PyObjectRefHelper::DecRef(export_object_);
}

std::string asyncflow::py::PyAgent::GetName()
{
	return GetObjectName(GetRefObject());	 
}
