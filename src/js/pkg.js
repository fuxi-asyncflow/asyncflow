mergeInto(LibraryManager.library, {
    
    js_call_func: function (id, function_name) {
        var str = UTF8ToString(function_name);
        return func_call(id, str);
    },
    
    js_to_string: function (id) {
        var variable = asyncflow.get(id)
        return allocateUTF8(String(variable))
    },
    
    js_is_true: function (id) {
        return is_true(id)
    },
    
    js_callback: function (id, chart_name) {
        var str = UTF8ToString(chart_name);
        on_stop(id, str);
    },
    
    js_clear_gameobj: function(id) {
        asyncflow.remove_game_obj(id)
    },
    
    js_clear_variable: function(id) {
        asyncflow.remove_var(id)
    },
    
    js_debug_data: function(data) {
        var str = UTF8ToString(data);
        handle_debug(str);
    }
})