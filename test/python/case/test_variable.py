import sys
sys.path.append('../')
sys.path.append('.')

import asyncflow
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

def test_setget():    
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    n1 = graph.add_func_node(set_var("a", "one"))
    n2 = graph.add_func_node(get_var("a"))
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)    

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    actual = []
    
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    
    assert expected == actual

def test_weakref(weak = True):    
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"

    graph = GraphBuilder(graph_name, "Character")
    graph.add_varialble("a", "String")
    test_var = [Character("weak var")]

    def del_var(self):
        test_var.clear()

    def set_var_weak(name, lst, weak = False):
        def _set_var(self):
            asyncflow.set_var(name, lst[0], weak)
        return _set_var
    
    n1 = graph.add_func_node(set_var_weak("a", test_var, weak))
    n2 = graph.add_func_node(del_var)
    n3 = graph.add_func_node(get_var("a", weak))
    
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
    asyncflow.step(10)
    asyncflow.exit()

    expected = []
    Character._output = expected.append    
    if weak:
        say_msg(None)(c)
    #print(actual, expected)
    assert expected == actual

if __name__ == '__main__':
    #test_setget()
    test_weakref()