#pragma once
#include <inttypes.h>
#ifndef EM_PORT_API
#	if defined(__EMSCRIPTEN__)
#		include <emscripten.h>
#		if defined(__cplusplus)
#			define EM_PORT_API(rettype) extern "C" rettype EMSCRIPTEN_KEEPALIVE
#		else
#			define EM_PORT_API(rettype) rettype EMSCRIPTEN_KEEPALIVE
#		endif
#	else
#		if defined(__cplusplus)
#			define EM_PORT_API(rettype) extern "C" rettype
#		else
#			define EM_PORT_API(rettype) rettype
#		endif
#	endif
#endif

EM_PORT_API(int) js_call_func(int a, const char* name);
EM_PORT_API(int) js_is_true(int id);
EM_PORT_API(const char*) js_to_string(int id);
EM_PORT_API(int) js_callback(int id, const char* chart_name);
EM_PORT_API(int) js_clear_gameobj(int id);
EM_PORT_API(int) js_clear_variable(int id);

EM_PORT_API(int) js_debug_data(const char* data);

EM_PORT_API(int) setup();
EM_PORT_API(int) import_charts(char* str);
EM_PORT_API(int) import_event(char* str);
EM_PORT_API(int) register_obj(int id, int tick);
EM_PORT_API(int) deregister_obj(int id);
EM_PORT_API(int) asyncflow_exit();
EM_PORT_API(void *) attach(int id, char* str);
EM_PORT_API(int) remove_chart(int id, char* str);
EM_PORT_API(int) start(int id);
EM_PORT_API(int) stop(int id);
EM_PORT_API(int) start_chart(int id, char* str);
EM_PORT_API(int) stop_chart(int id, char* str);
EM_PORT_API(int) step(int time);
EM_PORT_API(int) event(int obj_id, int event_id, uint8_t * ptr, int count);
EM_PORT_API(int) callback(void* chart);

EM_PORT_API(int) debug_command(char* str);

EM_PORT_API(int) get_var(int var_id);
EM_PORT_API(int) set_var(int var_id, int id);
EM_PORT_API(int) wait(float interval);
EM_PORT_API(int) wait_event(int id, int event_id);
EM_PORT_API(int) get_event_param(int event_id, int param_id);
EM_PORT_API(int) wait_all(uint8_t* ptr, int count);
EM_PORT_API(int) get_time();