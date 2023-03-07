import asyncflow
import sys
sys.path.append('../')
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

def test_simple_run():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)
    graph.connect(n1, n2, ConnType.Always)    

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(0)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    say_two(c)
    assert expected == actual

def test_loop():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n1)

    #           say("one") 
    #               |      \
    #           say("two") /

    # when say("one") is excucted again in one step, it'll be scheduled to wait a tick event
    # object will send tick event every 1000 milliseconds

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    count = 3    
    for i in range(count):
        asyncflow.step(1000)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    for i in range(count + 1):
        say_one(c)
        say_two(c)
    assert expected == actual

def test_dfs_run():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(say_two)
    n3 = graph.add_func_node(say_three)
    n4 = graph.add_func_node(say_four)
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)
    graph.connect(n1, n4)

    #           say("one")
    #           /         \
    #       say("two")    say("four")
    #         /
    #     say("three")
    
    # run order will be: one -> two -> three -> four

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())   

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(0)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    say_two(c)
    say_three(c)
    say_four(c)
    assert expected == actual

def test_wait():    
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(wait_1s)
    n3 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    for i in range(5):
        asyncflow.step(100)
    assert len(actual) == 1
    for i in range(10):
        asyncflow.step(100)
    assert len(actual) == 2
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    say_two(c)
    assert expected == actual

def test_loop_wait():
    mgr = asyncflow.setup()
    graph_name = "AI.test_loop_wait"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(wait_func(0.01))
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n1)

    #           say("one") 
    #               |      \
    #           wait(0.01) /

    # if a wait node exist in a loop, it'll not be scheduled to wait tick event    

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    count = 3    
    for i in range(count):
        asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    for i in range(count):
        say_one(c)
    assert expected == actual


if __name__ == '__main__':
    test_simple_run()
    test_loop()
    test_dfs_run()
    test_wait()
    test_loop_wait()
