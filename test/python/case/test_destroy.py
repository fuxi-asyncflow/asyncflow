import sys
sys.path.append('../')
sys.path.append('.')

import asyncflow
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

event0 = "Event0"
event2 = "Event2"

def prepare_events():
    builder = EventBuilder()
    builder.add_event(event0)
    builder.add_event(event2, [{"name": "arg0", "type": "Number"}, {"name": "arg1", "type": "String"}, {"name": "arg2", "type": "Character"}])
    return builder.build()

def test_destroy():
    mgr = asyncflow.setup()
    graph_name = "AI.test_destroy"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(destroy_self)
    n3 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)
    graph.connect(n1, n2, ConnType.Always)
    graph.connect(n2, n3, ConnType.Always)

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(0)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    assert expected == actual


def test_agent_destroy_when_event_in_queue():
    mgr = asyncflow.setup()
    graph_name = "AI.test_agent_destroy_when_event_in_queue"
    other = Character("other")

    graph = GraphBuilder(graph_name, "Character")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event(other, event0))
    n3 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)
    graph.connect(n1, n2, ConnType.Always)
    graph.connect(n2, n3, ConnType.Always)

    mgr.import_charts(graph.build())
    mgr.import_event(prepare_events())

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.register(other)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(0)
    asyncflow.post(other, getattr(asyncflow.EventId, event0))
    asyncflow.deregister(other)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    assert expected == actual


if __name__ == '__main__':
    #test_destroy()
    test_agent_destroy_when_event_in_queue()