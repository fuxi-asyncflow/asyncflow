import sys
sys.path.append('../')
sys.path.append('.')

import asyncflow
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

def test_waitall():
    mgr = asyncflow.setup()
    graph_name = "AI.test_waitall"

    graph = GraphBuilder(graph_name, "Character")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(wait_1s)
    n3 = graph.add_func_node(say_two)
    n4 = graph.add_control_node("waitall", [str(n1), str(n3)])
    n5 = graph.add_func_node(say_three)
    graph.connect_from_start(n1)
    graph.connect_from_start(n2)
    graph.connect(n2, n3)
    graph.connect(n1, n4)
    graph.connect(n3, n4)
    graph.connect(n4, n5)

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    c._output = actual.append

    expect_c = Character("npc")
    expected = []
    expect_c._output = expected.append

    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(100)
    say_one(expect_c)
    assert expected == actual

    asyncflow.step(1000)
    say_two(expect_c)
    say_three(expect_c)
    assert expected == actual
    asyncflow.exit()

def test_waitall_again():
    mgr = asyncflow.setup()
    graph_name = "AI.test_waitall"

    graph = GraphBuilder(graph_name, "Character")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(wait_1s)
    n3 = graph.add_func_node(say_two)
    n4 = graph.add_control_node("waitall", [str(n1), str(n3)])
    n5 = graph.add_func_node(say_three)
    graph.connect_from_start(n1)
    graph.connect_from_start(n2)
    graph.connect(n2, n3)
    graph.connect(n1, n4)
    graph.connect(n3, n4)
    graph.connect(n4, n5)
    graph.connect(n5, n1)
    graph.connect(n5, n2)

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    c._output = actual.append

    expect_c = Character("npc")
    expected = []
    expect_c._output = expected.append

    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(100)
    say_one(expect_c)
    assert expected == actual

    asyncflow.step(1000)
    say_two(expect_c)
    say_three(expect_c)
    say_one(expect_c)
    print(actual)
    assert expected == actual

    asyncflow.step(1000)
    say_two(expect_c)
    say_three(expect_c)
    say_one(expect_c)
    assert expected == actual



    asyncflow.exit()

if __name__ == '__main__':
    test_waitall()
    test_waitall_again()