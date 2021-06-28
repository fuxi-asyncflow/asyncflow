#include "js_agent.h"
#include "export_js.h"
using namespace asyncflow::js;

JsAgent::JsAgent(Manager* manager, int id)
	:Agent(manager),obj_(id){}

JsAgent::~JsAgent()
{
	js_clear_gameobj(obj_);
}