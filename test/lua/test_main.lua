package.path = package.path ..';../flowchart/generate/?.lua';
require("asyncflow")
require("AI")
require("Subchart")
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
    asyncflow.register(chara)
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
    --, {"AI.test_12", nil, nil, {"1", "2"}}
    , {"AI.test_13", nil, nil, {"hello", "1", "2"}}
    , {"AI.test_14", 3, nil, {"1", "2", "3", "4", "1", "2", "3", "4","1", "2", "3", "4","1", "2", "3", "4"}}
    , {"AI.test_15", nil, nil, {"1000", "2000", "6000"}}
    , {"AI.test_16", nil, eventlist, {"1000", "2000", "2200"}}
    , {"AI.test_17", nil, nil, {"1000", "1000", "2000", "4000"}}
    --[[
    ,{"Subchart.SubchartTest_01", 5, {},
     {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    ,{"Subchart.SubchartTest_02", 5, {},
     {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_03", 5, {},
       {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_04", 5, {},
       {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_05", 5, {},
       {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_06", 5, {},
       {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_07", 0.5, {},
       {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_08", 0.5, {},
       {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_09", 0.5, {},
       {"hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"}}
    , {"Subchart.SubchartTest_10", 5, {}, {"joinsub", "end","joinsub", "end","joinsub", "end", "joinsub", "end", "joinsub"}}
    , {"Subchart.SubchartTest_11", nil, {}, {"hellosub", "hellosub", "hellosub", "end"}}
    , {"Subchart.SubchartTest_12", nil, {}, {"hellosub", "hellosub", "hellosub", "end"}}
    , {"Subchart.SubchartTest_13", nil, {}, {"hellosub", "hellosub", "hellosub", "end"}}
    ]]
    }

function RaiseEvent(obj, event_list, step)
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
            asyncflow.event(obj, event_id)
        elseif #params == 2
        then
            asyncflow.event(obj,event_id,params[2])
        elseif #params == 3
        then
            asyncflow.event(obj,event_id,params[2],params[3])
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
    
    asyncflow.setup()
    --asyncflow.config_log("./logconfig.toml", "root")
    
    asyncflow.import_charts("../flowchart/graphs/AI.yaml")
    asyncflow.import_charts("../flowchart/graphs/Subchart.yaml")
    asyncflow.import_event("../flowchart/types/_event.yaml")
    asyncflow.register(Character)
    asyncflow.attach(Character,chart_name)
    asyncflow.start(Character)
    asyncflow.step(0)
    for i=1,total_frames do
        for key, value in ipairs(Character.allCharacter) do
            RaiseEvent(value, event_list, frame)
        end
        RaiseEvent(Character, event_list, frame)
        asyncflow.step(step_time)
        frame = frame + 1
    end
    lu.assertEquals(Character.output,result)
    asyncflow.remove(Character,chart_name)
    asyncflow.deregister(Character)
    asyncflow.step(10)
    asyncflow.exit()
    print(chart_name .. " pass")
    --print(Character.output)
end
    
function test_main()
    for key,value in ipairs(testdata)
    do
        run_case(value[1],value[2],value[3],value[4])
    end
end

function attach_chart_params(params_tbl)
    Character.output={}
    Character.allCharacter = {}
    
    asyncflow.setup()
    --asyncflow.config_log("./logconfig.toml", "root")
    
    asyncflow.import_charts("../flowchart/graphs/AI.yaml")
    asyncflow.import_charts("../flowchart/graphs/Subchart.yaml")
    asyncflow.import_event("../flowchart/types/_event.yaml")
    asyncflow.register(Character)
    local chart = asyncflow.attach(Character,"AI.test_07_sub",params_tbl)
    chart:set_callback(callback)
    asyncflow.start(Character)
    for i=1,100 do
        asyncflow.step(10)
    end
    lu.assertEquals(Character.output,{"2","3","5","5"})
    for i = 1,10
    do
        Character.output={}
        asyncflow.start(Character,{"AI.test_07_sub"})
        for i=1,10 do
            asyncflow.step(10)
        end
    lu.assertEquals(Character.output,{"2","3","5"})
    end
    asyncflow.remove(Character,"AI.test_07_sub")
    asyncflow.deregister(Character)
    asyncflow.step(10)
    asyncflow.exit()
end

function test_attach_chart()
    local tbl ={x = 2, y = 3, ss = 4}
    attach_chart_params(tbl)
    tbl = {2, 3, 4, 55}
    attach_chart_params(tbl)
end

os.exit(lu.LuaUnit.run() )