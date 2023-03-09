import asyncflow
import sys
sys.path.append('../')
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

def test_stop_node():
    mgr = asyncflow.setup()
    graph_name = "AI.test_stop_node"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(wait_1s)
    n3 = graph.add_control_node("stopnode", [str(n2)])
    n4 = graph.add_func_node(say_two)

    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n4)
    graph.connect(n1, n3)

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    for i in range(5):
        asyncflow.step(500)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    assert expected == actual

def test_stop_flow():
    mgr = asyncflow.setup()
    graph_name = "AI.test_stop_flow"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(wait_1s)
    n2a = graph.add_func_node(say_two)
    n3 = graph.add_control_node("stopflow", [str(n1)])
    n3a = graph.add_func_node(say_three)
    n4 = graph.add_func_node(say_four)

    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n2a)
    graph.connect(n1, n3)
    graph.connect(n3, n3a)
    graph.connect(n1, n4)

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    for i in range(5):
        asyncflow.step(500)
    asyncflow.exit()

    expected = []    
    Character._output = expected.append
    say_one(c)
    assert expected == actual

if __name__ == '__main__':
    test_stop_node()
    test_stop_flow()