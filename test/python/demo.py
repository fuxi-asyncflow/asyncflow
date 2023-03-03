import sys
import time
import logging
sys.path.append("../flowchart")
sys.path.append("../flowchart/generate")
import asyncflow
asyncflow.set_logger(logging)
asyncflow.setup({"debug_ip":u"127.0.0.1"})

import asyncflow_events
import AI
import Subchart

print(asyncflow.EventId.Tick)

def test():
    return a + b

class Character:
    allCharacter = []
    
    def __init__(self):
        self.output = []

    def Say(self, s):
        print(s)
        #test()
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


    # start
    for i in range(1, 9):
        c = Character()
        asyncflow.register(c)
        chart_name = "AI.test_0" + str(i)
        asyncflow.attach(c, chart_name)
        asyncflow.start(c)
    
    # loop
    for i in range(10000):
        #time.sleep(0.1)
        asyncflow.step(100)

    # destroy
    asyncflow.remove(c,"AI.test_07_sub")
    asyncflow.deregister(c)
    asyncflow.step(10)
    asyncflow.exit()
