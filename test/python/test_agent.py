import sys
sys.path.append("../flowchart/generate")
import asyncflow
import Flowchart_AI
import Flowchart_Subchart
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
        manager = asyncflow.get_current_manager()
        manager.register(char)
        return char

eventlist = {
    1: [ 3 ],
    2: [ 4, " hello" ],
    3: [ 5, " first", " second" ]
}


def RaiseEvent(mgr, obj, event_list, step):
    #运行到21frame，2.1s时开始抛出时间
    step = step - 20    
    if step in event_list:
        print("raise event ", step)
        event_id = event_list[step][0]
        mgr.event(obj, event_id, *event_list[step][1:])

def test_get_charts():
    manager = asyncflow.setup()
    manager.import_charts("../flowchart/generate/Flowchart_AI.json")   
    manager.import_event("../flowchart/generate/event_info.json")
    c = Character()
    agent = manager.register(c)
    chart_tbl = ["AI.test_01","AI.test_02","AI.test_03","AI.test_07"]
    for i in chart_tbl:
        agent.attach(i)
    agent.start()
    result = agent.get_charts()
    assert(set(result) == set(chart_tbl))
    result = agent.get_charts(True)
    assert(set(result) == set(chart_tbl))
    for i in range(150):
        manager.step(10)
    result = agent.get_charts(True)
    chart_tbl.append("AI.test_07_sub")
    assert(set(result) == set(chart_tbl))
    manager.deregister(Character)
    manager.step(10)
    asyncflow.exit()
    
def test_get_obj():
    obj1 = Character()
    obj2 = Character()
    obj3 = Character()
    manager = asyncflow.setup()
    agent1 = manager.register(obj1)
    agent2 = manager.register(obj2)
    assert(agent1.get_obj()==obj1)
    assert(agent2.get_obj()==obj2)
    assert(agent2.get_obj()!=obj3)
    asyncflow.exit()

if __name__ == "__main__":
    test_get_charts()
    test_get_obj()

