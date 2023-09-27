import sys
sys.path.append('../')
sys.path.append('.')

import asyncflow
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

def prepare_subchart(graph_name):    
    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_two)
    n2 = graph.add_func_node(say_three)
    graph.connect_from_start(n1)
    graph.connect(n1, n2)    
    return graph.build()

def prepare_subchart_sync(graph_name):    
    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_two)    
    n2 = graph.add_func_node(subchart(graph_name, 'self'))
    graph.connect_from_start(n1)
    graph.connect(n1, n2)    
    return graph.build()

def prepare_subchart_return(graph_name):    
    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_two)
    n2 = graph.add_func_node(return_func("abc"))
    graph.connect_from_start(n1)
    graph.connect(n1, n2) 
    return graph.build()

def test_subchart():    
    c = Character("npc")
    mgr = asyncflow.setup()
    subgraph_name = "AI.subgraph"
    graph_name = "AI.test_subchart"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(subchart(subgraph_name, c))
    n3 = graph.add_func_node(say_four)
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)    

    asyncflow.import_charts(graph.build())
    asyncflow.import_charts(prepare_subchart(subgraph_name))
    asyncflow.import_event(EventBuilder().build())
    
    actual = []    
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    say_two(c)
    say_three(c)
    say_four(c)
    
    assert expected == actual

def test_return():
    c = Character("npc")
    mgr = asyncflow.setup()
    graph_name = "AI.test_return"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(return_func(1))
    n3 = graph.add_func_node(say_three)
    n4 = graph.add_func_node(say_four)
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)
    graph.connect(n1, n4)    

    asyncflow.import_charts(graph.build())    
    asyncflow.import_event(EventBuilder().build())
    
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)    
    
    assert expected == actual

def test_subchart_return():
    c = Character("npc")
    mgr = asyncflow.setup()
    subgraph_name = "AI.subgraph_return"
    graph_name = "AI.test_subchart_return"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_ret_var_node(subchart(subgraph_name, c), "a")
    n3 = graph.add_func_node(get_var("a"))
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)    

    asyncflow.import_charts(graph.build())
    asyncflow.import_charts(prepare_subchart_return(subgraph_name))
    asyncflow.import_event(EventBuilder().build())
    
    actual = []    
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    say_two(c)
    c.Say("abc")
    
    assert expected == actual

def test_subchart_sync():
    c = Character("npc")
    mgr = asyncflow.setup()
    subgraph_name = "AI.subgraph_sync"
    graph_name = "AI.test_subchart_sync"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(subchart(subgraph_name, c))
    n3 = graph.add_func_node(say_four)
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)    

    asyncflow.import_charts(graph.build())
    asyncflow.import_charts(prepare_subchart_sync(subgraph_name))
    asyncflow.import_event(EventBuilder().build())
    
    actual = []    
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, subgraph_name)
    
    asyncflow.start(c)
    step_count = 2
    for i in range(step_count):
        asyncflow.step(10)    
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    for i in range(step_count*(16-1)):
        say_two(c)   
    assert expected == actual




if __name__ == '__main__':
    test_subchart()
    test_return()
    test_subchart_return()
    test_subchart_sync()