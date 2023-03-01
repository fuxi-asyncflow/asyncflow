import asyncflow
import sys
sys.path.append('../')
from utils.graph_builder import *

class Character():
    pass

def test_setup():
    mgr = asyncflow.setup()				# setup success will return a manager object
    assert asyncflow.setup() == None	# setup again will return None
    assert mgr.is_valid()
    assert mgr.exit()
    assert not mgr.is_valid()			# manager will be invalid after exit
    assert not mgr.exit()				# exit after exit will return False

def test_register():
    asyncflow.setup()
    # register object to asyncflow
    a = Character()
    agent = asyncflow.register(a)                   # register object success will return an agent bind to it
    assert id(agent) == id(asyncflow.get_agent(a))  # get_agent can get same object
    assert agent and agent.is_valid() == True       # agent will be valid util object be deregistered
    assert asyncflow.register(a) == None            # register same object again will return None

    # deregister
    assert asyncflow.deregister(a) == True
    assert agent.is_valid() == True
    asyncflow.step(10)
    asyncflow.step(10)
    assert agent.is_valid() == False
    assert asyncflow.get_agent(a) == None       # cannot get agent of object when it's invalid
    assert asyncflow.deregister(a) == False

    asyncflow.exit()


def test_register2():
    '''
    use register method in asyncflow module has same effect
    '''
    mgr = asyncflow.setup()
    # register object to asyncflow
    a = Character()
    agent = mgr.register(a)
    assert mgr.register(a) == None
    assert agent and agent.is_valid() == True

    assert mgr.deregister(a) == True
    assert agent.is_valid() == True
    mgr.step(10)
    mgr.step(10)
    assert agent.is_valid() == False
    assert mgr.deregister(a) == False

    mgr.exit()

if __name__ == "__main__":
    test_setup()
    test_register()
    test_register2()