import sys
sys.path.append("../flowchart")
sys.path.append("../flowchart/generate")
import asyncflow
asyncflow.setup()
import asyncflow_events
import AI
import Subchart
import time
import asyncflow
from io import StringIO
import pytest

class EventId():
    Event0Arg = 3
    Event1Arg = 4
    Event2Arg = 5

class Character:
    allCharacter = []
    
    def __init__(self):
        self.output = []

    def Say(self, s):
        #print(s)
        self.output.append(str(s))

    def CreateCharacter(self):
        char = Character()
        Character.allCharacter.append(char)
        char.output = self.output
        asyncflow.register(char)
        asyncflow.start(char)
        return char

eventlist = {
    1: [ 3 ],
    2: [ 4, " hello" ],
    3: [ 5, " first", " second" ]
}


#SubchartTest_07,SubchartTest_08,SubchartTest_09 的输出次数受子图是否立刻执行的影响
testdata = [
      ("AI.test_01", 0, {}, ["hello"])
    , ("AI.test_02", 0, {}, ["0", "1000"])
    , ("AI.test_03", 0, {}, ["hello"])
    , ("AI.test_04", 2, {}, ["hello", "world", "0", "hello", "world", "1000", "hello", "world", "2000"])
    , ("AI.test_05", 0, eventlist, ["event 0 pass", "event 1 pass hello", "event 2 pass", "1st arg first", "2nd arg second"])
    , ("AI.test_06", 0, {}, ["subunit pass"])
    , ("AI.test_07", 0, {}, ["1", "2", "3", "3", "4", "7", "7"])
    , ("AI.test_09", 0, {}, ["node 1", "node 2", "node 3", "all node runned"])
    , ("AI.test_10", 0, eventlist, ["event 0 pass", "event 1 pass hello", "event 2 pass", "1st arg first", "2nd arg second"])
    , ("AI.test_11", 0, {}, ["hello sub", "subunit pass", "green"])
    , ("AI.test_12", 0, {}, ["1", "2"])
    #, ("AI.test_13", 0, {}, ["hello"])
    , ("AI.test_14", 3, {}, ["1", "2", "3", "4", "1", "2", "3", "4","1", "2", "3", "4","1", "2", "3", "4"])
    , ("AI.test_15", 0, {}, ["1000", "2000", "6000"])
    , ("AI.test_16", 0, eventlist, ["1000", "2000", "2200"])
    , ("AI.test_17", 0, {}, ["1000", "1000", "2000", "4000"])
    ,("Subchart.SubchartTest_01", 5, {},
     ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    ,("Subchart.SubchartTest_02", 5, {},
     ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_03", 5, {},
       ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_04", 5, {},
       ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_05", 5, {},
       ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_06", 5, {},
       ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_07", 0.5, {},
       ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_08", 0.5, {},
       ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_09", 0.5, {},
       ["hello", "hellosub", "end", "hello", "hellosub", "end", "hello", "hellosub"])
    , ("Subchart.SubchartTest_10", 5, {}, ["joinsub", "end","joinsub", "end","joinsub", "end", "joinsub", "end", "joinsub"])
    , ("Subchart.SubchartTest_11", 0, {}, ["hellosub", "hellosub", "hellosub", "end"])
    , ("Subchart.SubchartTest_12", 0, {}, ["hellosub", "hellosub", "hellosub", "end"])
    , ("Subchart.SubchartTest_13", 0, {}, ["hellosub", "hellosub", "hellosub", "end"])
]

def RaiseEvent(obj, event_list, step):
    #运行到21frame，2.1s时开始抛出时间
    step = step - 20    
    if step in event_list:
        print("raise event ", step)
        event_id = event_list[step][0]
        asyncflow.event(obj, event_id, *event_list[step][1:])

@pytest.mark.parametrize("chart_name, run_time, event_list, result", testdata)
def test_RunCase(chart_name, run_time, event_list, result):    
    frame = 0
    step_time = 100
    run_time =  run_time or 10
    total_frames = int(run_time * 1000 // step_time)
    print('total_frames', total_frames)
    asyncflow.setup()
    asyncflow.import_charts("../flowchart/graphs/AI.yaml")   
    asyncflow.import_charts("../flowchart/graphs/Subchart.yaml")	
    # asyncflow.import_event("../flowchart/generate/event_info.json")

    c = Character()
    asyncflow.register(c)
    asyncflow.attach(c, chart_name)
    asyncflow.start(c)
    asyncflow.step(0)
    for i in range(total_frames):   
        for item in Character.allCharacter:
            RaiseEvent(item, event_list, frame)
        RaiseEvent(c, event_list, frame)
        asyncflow.step(step_time)
        frame = frame + 1

    print(chart_name, 'output', c.output)
    assert result == c.output    
    asyncflow.deregister(c)
    asyncflow.step(10)
    asyncflow.exit() 
    
def test_attach_chart_params():
    asyncflow.setup()
    asyncflow.import_charts("../flowchart/generate/Flowchart_AI.json")
    asyncflow.import_event("../flowchart/generate/event_info.json")
    c = Character()
    agent = asyncflow.register(c)
    tbl = dict()
    params_tbl = dict()
    params_tbl["x"] = 2
    params_tbl["y"] = 3
    params_tbl["ss"] = 4
    def callback(value):
        c.Say(value)
    chart = asyncflow.attach(c,"AI.test_07_sub",params_tbl)
    chart.set_callback(callback)
    del chart
    agent.start()
    for i in range(10):
        asyncflow.step(10)
    assert ["2","3","5","5"] == c.output
    for i in range(10):
        c.output=[]
        print(i)
        asyncflow.start(c,["AI.test_07_sub"])
        for i in range(10):
            asyncflow.step(10)
        assert c.output == ["2","3","5"]
    asyncflow.remove(c,"AI.test_07_sub")
    asyncflow.deregister(c)
    asyncflow.step(10)
    asyncflow.exit()



if __name__ == "__main__":
    #test_attach_chart_params()
    for params in testdata:
        test_RunCase(*params)