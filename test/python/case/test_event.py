import sys
sys.path.append('../')
sys.path.append('.')

import pytest

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

@pytest.mark.parametrize("defer", [(True,), (False,)])
def test_event(defer):    
    c = Character("npc")
    expect_c = Character("npc")
    mgr = asyncflow.setup()    
    graph_name = "AI.test_event"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event("self", event0))
    n3 = graph.add_func_node(say_two)
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)    

    print(graph.build())
    mgr.import_charts(graph.build())    
    mgr.import_event(prepare_events())
    print("event id", getattr(asyncflow.EventId, event0))
    
    actual = []
    expected = []
    c._output = actual.append
    expect_c._output = expected.append
    asyncflow.register(c)    
    asyncflow.attach(c, graph_name)    

    asyncflow.start(c)
    assert actual == expected

    asyncflow.step(10)
    say_one(expect_c)
    assert actual == expected       # run first node
    
    asyncflow.step(10)
    asyncflow.step(10)
    assert actual == expected       # stuck at event node

    if defer:
        asyncflow.post(c, getattr(asyncflow.EventId, event0))
    else:
        say_two(expect_c)
        asyncflow.trigger(c, getattr(asyncflow.EventId, event0))
    assert actual == expected       # event raised, but event will be handled in next step

    asyncflow.step(10)              # event is handled now
    if defer:
        say_two(expect_c)
    assert actual == expected       # successor node now is excucted

    asyncflow.exit()

def test_trigger():    
    c = Character("npc")
    expect_c = Character("npc")
    mgr = asyncflow.setup()    
    graph_name = "AI.test_trigger"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event("self", event0))
    n3 = graph.add_func_node(say_two)
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)    

    print(graph.build())
    mgr.import_charts(graph.build())    
    mgr.import_event(prepare_events())
    print("event id", getattr(asyncflow.EventId, event0))
    
    actual = []
    expected = []
    c._output = actual.append
    expect_c._output = expected.append
    asyncflow.register(c)    
    asyncflow.attach(c, graph_name)    

    asyncflow.start(c)
    assert actual == expected

    asyncflow.step(10)
    say_one(expect_c)
    assert actual == expected       # run first node
    
    asyncflow.step(10)
    asyncflow.step(10)
    assert actual == expected       # stuck at event node

    asyncflow.trigger(c, getattr(asyncflow.EventId, event0))    
    say_two(expect_c)
    assert actual == expected       # successor node now is excucted

    asyncflow.exit()

def test_event_other():    
    c = Character("npc")
    other = Character("other")
    expect_c = Character("npc")
    mgr = asyncflow.setup()    
    graph_name = "AI.test_event_other"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event(other, event0))
    n3 = graph.add_func_node(say_two)
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)    

    print(graph.build())
    mgr.import_charts(graph.build())    
    mgr.import_event(prepare_events())
    print("event id", getattr(asyncflow.EventId, event0))
    
    actual = []
    expected = []
    c._output = actual.append
    expect_c._output = expected.append
    asyncflow.register(c)    
    asyncflow.attach(c, graph_name)    

    asyncflow.start(c)
    assert actual == expected

    asyncflow.step(10)
    say_one(expect_c)
    assert actual == expected       # run first node
    
    asyncflow.step(10)
    asyncflow.step(10)
    assert actual == expected       # stuck at event node

    asyncflow.post(other, getattr(asyncflow.EventId, event0))
    assert actual == expected       # event raised, but event will be handled in next step

    asyncflow.step(10)              # event is handled now
    say_two(expect_c)
    assert actual == expected       # successor node now is excucted

    asyncflow.exit()

def test_event_param():    
    c = Character("npc")
    expect_c = Character("npc")
    mgr = asyncflow.setup()    
    graph_name = "AI.test_event_param"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event("self", event2))
    n3 = graph.add_event_node(print_ev_param(event2, 0))
    n4 = graph.add_event_node(print_ev_param(event2, 1))
    n5 = graph.add_event_node(print_ev_param(event2, 2))    
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)
    graph.connect(n3, n4)
    graph.connect(n4, n5)

    print(graph.build())
    mgr.import_charts(graph.build())    
    mgr.import_event(prepare_events())    
    
    actual = []
    expected = []
    c._output = actual.append
    expect_c._output = expected.append
    asyncflow.register(c)    
    asyncflow.attach(c, graph_name)    

    asyncflow.start(c)
    assert actual == expected

    asyncflow.step(10)
    say_one(expect_c)
    assert actual == expected       # run first node
    
    asyncflow.step(10)
    asyncflow.step(10)
    assert actual == expected       # stuck at event node

    asyncflow.post(c, getattr(asyncflow.EventId, event2), 1, "abc", c)
    assert actual == expected       # event raised, but event will be handled in next step

    asyncflow.step(10)              # event is handled now
    expect_c.Say(1)
    expect_c.Say("abc")
    expect_c.Say(c)
    assert actual == expected       # successor node now is excucted

    asyncflow.exit()

def test_event_order():
    '''
    if several nodes wait for one event, the node first waited will be handled first
    '''
    c = Character("npc")
    expect_c = Character("npc")
    mgr = asyncflow.setup()    
    graph_name = "AI.test_event_order"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event("self", event0))
    n3 = graph.add_func_node(say_two)
    n4 = graph.add_event_node(wait_event("self", event0))
    n5 = graph.add_func_node(say_three)
    n6 = graph.add_event_node(wait_event("self", event0))
    n7 = graph.add_func_node(say_four)    
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)
    graph.connect(n1, n4)
    graph.connect(n4, n5)
    graph.connect(n1, n6)
    graph.connect(n6, n7)
    
    mgr.import_charts(graph.build()) 
    mgr.import_event(prepare_events())
    print("event id", getattr(asyncflow.EventId, event0))
    
    actual = []
    expected = []
    c._output = actual.append
    expect_c._output = expected.append
    asyncflow.register(c)    
    asyncflow.attach(c, graph_name)    

    asyncflow.start(c)    
    asyncflow.step(10)
    say_one(expect_c)
    asyncflow.post(c, getattr(asyncflow.EventId, event0))   

    asyncflow.step(10)
    say_two(expect_c)
    say_three(expect_c)
    say_four(expect_c)
    assert actual == expected
    asyncflow.exit()

def test_event_handle_order():
    '''
    if several event raise before step, the event first raise will be handled first
    '''
    c = Character("npc")
    expect_c = Character("npc")
    mgr = asyncflow.setup()    
    graph_name = "AI.test_event_order"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event("self", event0))
    n3 = graph.add_func_node(say_two)
    n4 = graph.add_event_node(wait_event("self", event2))
    n5 = graph.add_func_node(say_three)
    n6 = graph.add_event_node(wait_event("self", event0))
    n7 = graph.add_func_node(say_four)    
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)
    graph.connect(n1, n4)
    graph.connect(n4, n5)
    graph.connect(n1, n6)
    graph.connect(n6, n7)
    
    mgr.import_charts(graph.build()) 
    mgr.import_event(prepare_events())
    print("event id", getattr(asyncflow.EventId, event0))
    
    actual = []
    expected = []
    c._output = actual.append
    expect_c._output = expected.append
    asyncflow.register(c)    
    asyncflow.attach(c, graph_name)    

    asyncflow.start(c)    
    asyncflow.step(10)
    say_one(expect_c)
    asyncflow.event(c, getattr(asyncflow.EventId, event2), 1, "abc", c)
    asyncflow.event(c, getattr(asyncflow.EventId, event0))

    asyncflow.step(10)
    say_three(expect_c)
    say_two(expect_c)
    say_four(expect_c)
    assert actual == expected
    asyncflow.exit()

def test_trigger_max():
    c = Character("npc")
    expect_c = Character("npc")
    mgr = asyncflow.setup()    
    graph_name = "AI.test_trigger"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_event_node(wait_event("self", event0))
    n3 = graph.add_func_node(say_two, no_loop_check = True)
    n4 = graph.add_func_node(lambda self: asyncflow.trigger(self, getattr(asyncflow.EventId, event0)), no_loop_check = True)
    n5 = graph.add_func_node(lambda self: asyncflow.trigger(self, getattr(asyncflow.EventId, event0)))
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n2)
    graph.connect(n2, n3)
    graph.connect(n3, n4)
    graph.connect(n1, n5)

    print(graph.build())
    mgr.import_charts(graph.build())    
    mgr.import_event(prepare_events())
    print("event id", getattr(asyncflow.EventId, event0))
    
    actual = []
    expected = []
    c._output = actual.append
    expect_c._output = expected.append
    asyncflow.register(c)    
    asyncflow.attach(c, graph_name)    

    asyncflow.start(c)

    asyncflow.step(10) 
   
    print(actual)
    #assert actual == expected       # successor node now is excucted

    asyncflow.exit()

if __name__ == '__main__':
    #test_event()
    #test_trigger()
    #test_event_other()
    #test_event_param()
    #test_event_order()
    #test_event_handle_order()
    test_trigger_max()