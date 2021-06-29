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
    print(s)
end

function callback(value)
    Character:Say(value)
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

--get_chart 默认不返回子图，加参数返回
function test_get_charts()
    Character.output={}
    Character.allCharacter = {}
    local manager = asyncflow.setup()
    manager:import_charts("Flowchart_AI.json")
    manager:import_event("event_info.json")
    local agent = manager:register(Character)
    local chart_tbl = {"AI.test_01","AI.test_02","AI.test_03","AI.test_07"}
    for k,v in ipairs(chart_tbl) do
        agent:attach(v);
    end
    result = agent:get_charts()
    lu.assertEquals(result,chart_tbl)
    result = agent:get_charts(true)
    lu.assertEquals(result,chart_tbl)
    for i=1,150 do
        manager:step(10)
    end
    result = agent:get_charts(true)
    table.insert(chart_tbl,"AI.test_07_sub")
    manager:deregister(Character)
    manager:step(10)
    manager:exit()
end

function test_get_obj()
    obj1 = {}
    obj2 = {}
    obj3 = {}
    local manager = asyncflow.setup()
    local agent1 = manager:register(obj1)
    local agent2 = manager:register(obj2)
    lu.assertIsTrue(agent1:get_obj()==obj1)
    lu.assertIsTrue(agent2:get_obj()==obj2)
    lu.assertIsTrue(agent2:get_obj()~=obj3)
end

os.exit(lu.LuaUnit.run() )

