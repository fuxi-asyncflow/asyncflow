package.path = package.path ..';../flowchart/generate/?.lua';
require("asyncflow")
require("Flowchart_AI")
require("Flowchart_Subchart")
lu = require('luaunit')
Character = {}
Character.__index = Character
Character.allCharacter = {}
Character.output = {}

function Character:Say(s)
    table.insert(self.output,tostring(s))
    --print(s)
end

function Character:CreateCharacter()
    local chara = {}
    setmetatable(chara, Character)
    table.insert(self.allCharacter, chara)
    chara.output = self.output
    local manager = asyncflow.get_current_manager()
    manager:register(chara)
    return chara
end

function callback(value)
    Character:Say(value)
end

eventlist = {{3},{4," hello"},{5," first"," second"}}
testdata = {
      {"AI.test_01", nil, nil, {"hello"}}
    , {"AI.test_02", nil, nil, {"0", "1000"}}
    , {"AI.test_03", nil, nil, {"hello"}}
    ,{"AI.test_04", 2, nil, {"hello", "world", "0", "hello", "world", "1000", "hello", "world", "2000"}}
    ,{"AI.test_05", nil, eventlist, {"event 0 pass", "event 1 pass hello", "event 2 pass", "1st arg first", "2nd arg second"}}
    ,{"AI.test_06", nil, nil, {"subunit pass"}}
    ,{"AI.test_07", nil, nil, {"1", "2", "3", "3", "4", "7", "7"}}
    ,{"AI.test_09", nil,nil, {"node 1", "node 2", "node 3", "all node runned"}}
    ,{"AI.test_10", nil, eventlist, {"event 0 pass", "event 1 pass hello", "event 2 pass", "1st arg first", "2nd arg second"}}
    ,{"AI.test_11", nil, nil, {"hello sub", "subunit pass", "green"}}
    , {"AI.test_12", nil, nil, {"1", "2"}}
    , {"AI.test_13", nil, nil, {"hello"}}
    , {"AI.test_14", 3, nil, {"1", "2", "3", "4", "1", "2", "3", "4","1", "2", "3", "4","1", "2", "3", "4"}}
    , {"AI.test_15", nil, nil, {"1000", "2000", "6000"}}
    , {"AI.test_16", nil, eventlist, {"1000", "2000", "2200"}}
    , {"AI.test_17", nil, nil, {"1000", "1000", "2000", "4000"}}
    }

function RaiseEvent(mgr,obj, event_list, step)
    --运行到21frame，2.1s时开始抛出时间
    local step = step - 20    
    if event_list == nil 
    then
    return 
    end
    if event_list[step] ~= nil
    then
        --print("raise event ", step)
        local params = event_list[step]
        local event_id = params[1]
        if #params == 1
        then
            mgr:event(obj, event_id)
        elseif #params == 2
        then
            mgr:event(obj,event_id,params[2])
        elseif #params == 3
        then
            mgr:event(obj,event_id,params[2],params[3])
        else
        end
    end
end

function run_case(chart_name, run_time, event_list,result)
    Character.output={}
    Character.allCharacter = {}
    local frame = 0
    local step_time = 100;
    local run_time = run_time or 10
    local total_frames = run_time * 1000 / step_time
    local manager = asyncflow.setup()
    manager:import_charts("../flowchart/generate/Flowchart_AI.json")
    manager:import_event("../flowchart/generate/event_info.json")
    local agent = manager:register(Character)
    agent:attach(chart_name)
    agent:start()
    manager:step(0)
    for i=1,total_frames do
        for key, value in ipairs(Character.allCharacter) do
            RaiseEvent(manager,value, event_list, frame)
        end
        RaiseEvent(manager,Character, event_list, frame)
        manager:step(step_time)
        frame = frame + 1
    end
    lu.assertEquals(Character.output,result)
    agent:remove(chart_name)
    manager:deregister(Character)
    manager:step(10)
    manager:exit()
    print(chart_name .. " pass")
    --print(Character.output)
end
    
function test_main()
    for key,value in ipairs(testdata)
    do
        run_case(value[1],value[2],value[3],value[4])
    end
end

--加图时同时加入变量表及回调
function test_attach_chart_params()
    Character.output={}
    Character.allCharacter = {}
    local manager = asyncflow.setup()
    manager:import_charts("../flowchart/generate/Flowchart_AI.json")
    manager:import_event("../flowchart/generate/event_info.json")
    local agent = manager:register(Character)
    local params_tbl = {}
    params_tbl["x"] = 2
    params_tbl["y"] = 3
    params_tbl["ss"] = 4
    local chart = agent:attach("AI.test_07_sub",params_tbl)
    chart:set_callback(callback)
    agent:start()
    for i=1,100 do
        manager:step(10)
    end
    lu.assertEquals(Character.output,{"2","3","5","5"})
    agent:remove("AI.test_07_sub")
    manager:deregister(Character)
    manager:step(10)
    manager:exit()


end
os.exit(lu.LuaUnit.run() )