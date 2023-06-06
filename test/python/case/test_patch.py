import sys
sys.path.append('../')
sys.path.append('.')

import asyncflow
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

def test_patch_inplace():
    mgr = asyncflow.setup()
    graph_name = "AI.test_patch_inplace"    

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)    
    graph.connect(n1, n2, ConnType.Always)
    graph.connect(n2, n1, ConnType.Always)

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())


    # prepare patch for replace node2's function to say_three
    n2_func_name ="{path}.{uid}".format(path = graph_name, uid = n2.hex)
    asyncflow.set_node_func(n2_func_name, say_three)

    patch_str = '''--- 
path: {0}
uid: {1}
type: Character
nodes: 
- 
  uid: {2}
  text: hello
  code: 
    type: FUNC
connectors: 
- 
  start: {2}
  end: {3}
  type: 2
...'''.format(graph_name, graph.uid, n2, n1)
    # patch end

    c = Character("npc")
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(0)
    asyncflow.patch(patch_str, True)    # patch in-place
    asyncflow.step(1000)
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    say_two(c)
    # after patch
    say_one(c)
    say_three(c)  
    assert expected == actual
    print("test_patch_inplace end")

def test_patch():
    mgr = asyncflow.setup()
    graph_name = "AI.test_patch_inplace"

    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)    
    graph.connect(n1, n2, ConnType.Always)
    graph.connect(n2, n1, ConnType.Always)

    graph_yaml = graph.build()
    mgr.import_charts(graph_yaml)
    mgr.import_event(EventBuilder().build())


    # prepare patch for replace node2's function to say_three
    n2_func_name ="{path}.{uid}".format(path = graph_name, uid = n2.hex)
    asyncflow.set_node_func(n2_func_name, say_three)

    c = Character("npc")    
    actual = []
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    asyncflow.step(0)
    asyncflow.patch(graph_yaml)    # default patch, chart create before patch won't be affected
    asyncflow.step(1000)

    c2 = Character("npc2")
    asyncflow.register(c2)
    asyncflow.attach(c2, graph_name)
    asyncflow.start(c2)
    asyncflow.step(0)

    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    say_two(c)
    # after patch, existed graph keep run with old data
    say_one(c)
    say_two(c)

    # new graph will use patched data
    say_one(c2)
    say_three(c2)
    assert expected == actual

    expected = []
    Character._output = expected.append
    print("test_patch end")

if __name__ == '__main__':
    test_patch_inplace()
    test_patch()