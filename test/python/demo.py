import sys
import time
from io import StringIO
import pytest

sys.path.append("../flowchart")
sys.path.append("../flowchart/generate")
import asyncflow

asyncflow.setup()
import asyncflow_events
import AI
import Subchart

class EventId():
    Event0Arg = 3
    Event1Arg = 4
    Event2Arg = 5

class Character:
    allCharacter = []
    
    def __init__(self):
        self.output = []

    def Say(self, s):
        print(s)
        #self.output.append(str(s))

    def CreateCharacter(self):
        char = Character()
        Character.allCharacter.append(char)
        char.output = self.output
        asyncflow.register(char)
        asyncflow.start(char)
        return char

if __name__ == "__main__":
    chart_name = "AI.test_04"
    
    # setup
    asyncflow.import_charts("../flowchart/graphs/AI.yaml")
    asyncflow.import_charts("../flowchart/graphs/Subchart.yaml")
    asyncflow.import_event("../flowchart/types/_event.yaml")

    # start
    c = Character()
    asyncflow.register(c)
    asyncflow.attach(c, chart_name)
    asyncflow.start(c)
    
    # loop
    for i in range(100):
        time.sleep(0.1)
        asyncflow.step(100)

    # destroy
    asyncflow.remove(c,"AI.test_07_sub")
    asyncflow.deregister(c)
    asyncflow.step(10)
    asyncflow.exit()
	