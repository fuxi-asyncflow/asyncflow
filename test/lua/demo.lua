package.path = package.path ..';../flowchart/generate/?.lua';
require("asyncflow")
require("AI")
require("Subchart")


 local ffi=require('ffi')
 ffi.cdef[[
      void Sleep(int ms);
      int poll(struct pollfd *fds, unsigned long nfds, int timeout);
 ]]

 local sleep
 if ffi.os == "Windows" then
     function sleep(s)
         ffi.C.Sleep(s*1000)
     end
 else
     function sleep(s)
          ffi.C.poll(nil, 0, s*1000)
     end
 end

Character = {}
Character.__index = Character
Character.allCharacter = {}
Character.output = {}

function Character:Say(s)
    table.insert(self.output,tostring(s))
    print(s)
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
print("========================")
asyncflow.setup()
print("========================")
asyncflow.import_charts("../flowchart/graphs/AI.yaml")
asyncflow.import_charts("../flowchart/graphs/Subchart.yaml")
local event_count = asyncflow.import_event("../flowchart/types/_event.yaml")

print("========================")
asyncflow.register(Character)
asyncflow.attach(Character, "AI.test_07")
asyncflow.start(Character)

local frame = 0
local step_time = 100;
local run_time = run_time or 10
local total_frames = run_time * 1000 / step_time

for i=1,total_frames do
    for key, value in ipairs(Character.allCharacter) do
        RaiseEvent(value, event_list, frame)
    end
  
    asyncflow.step(step_time)
    --sleep(step_time / 1000.0)
    frame = frame + 1
end

