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