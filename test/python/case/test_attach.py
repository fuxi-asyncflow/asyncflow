import asyncflow
import sys
sys.path.append('../')
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

## prepare graph
def prepare_chart_01(graph_name):    
    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)
    graph.connect(n1, n2, ConnType.Always)    
    return graph.build()

def prepare_chart_02(graph_name):    
    graph = GraphBuilder(graph_name, "Character")    
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(say_two)
    graph.connect_from_start(n1)
    graph.connect(n1, n2, ConnType.Always)
    graph.connect(n2, n1, ConnType.Always)
    return graph.build()

def test_attach():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"
    mgr.import_charts(prepare_chart_01(graph_name))
    mgr.import_event(EventBuilder().build())   

    c = Character("npc")    
    agent = asyncflow.register(c)
    graph = asyncflow.attach(c, graph_name)
    assert asyncflow.attach(c, graph_name) == None  # cannot attach same graph again
    assert agent.attach(graph_name) == None     # cannot attach same graph again
    asyncflow.attach(c, graph_name)
    assert graph.is_running() == False          # graph will not run before start
    asyncflow.start(c)
    assert graph.is_running() == True           # start all chart on object
    asyncflow.step(0)
    assert graph.is_running() == False          # no node is running, graph is idle again
    asyncflow.exit()

def test_remove():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"
    mgr.import_charts(prepare_chart_01(graph_name))
    graph_name2 = "AI.test_02"
    mgr.import_charts(prepare_chart_01(graph_name2))
    mgr.import_event(EventBuilder().build())

    c = Character("npc")    
    agent = asyncflow.register(c)
    graph = asyncflow.attach(c, graph_name)    
    graph2 = agent.attach(graph_name2) 
    asyncflow.start(c)
    assert set(agent.get_graphs()) == set([graph, graph2])      # get_graphs will return all graphs on object, no order 
    asyncflow.step(0)
    assert asyncflow.remove(c, graph_name2) == True             # remove graph by name
    assert agent.get_graphs() == [graph]
    assert agent.remove(graph_name) == True
    assert agent.get_graphs() == []
    assert asyncflow.remove(c, graph_name2) == False            # if graph not attached, remove will return false
    assert agent.remove(graph_name) == False
    asyncflow.exit()

def test_start():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"
    mgr.import_charts(prepare_chart_01(graph_name))
    mgr.import_event(EventBuilder().build())

    graph_name2 = "AI.test_02"
    mgr.import_charts(prepare_chart_02(graph_name2))

    c = Character("npc")
    asyncflow.register(c)
    graph = asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    graph2 = asyncflow.attach(c, graph_name2)
    assert graph2.is_running() == False         # graph2 attach after start, it will keep idle
    asyncflow.start(c, [graph_name2])           # start graph2 by name
    assert graph2.is_running() == True          # now graph2 is running
    asyncflow.step(1000)
    asyncflow.step(1000)
    assert graph.is_running() == False
    assert graph2.is_running() == True          # graph2 has a loop, so it will keep running    
    asyncflow.exit()

def test_stop():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"
    mgr.import_charts(prepare_chart_02(graph_name))
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    asyncflow.register(c)
    graph = asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    for i in range(10):
        asyncflow.step(1000)
    assert graph.is_running() == True
    asyncflow.stop(c)                           # stop will stop all graph attached on object
    assert graph.is_running() == False
    asyncflow.exit()
    pass

def test_stop2():
    mgr = asyncflow.setup()
    graph_name = "AI.test_01"
    mgr.import_charts(prepare_chart_02(graph_name))
    graph_name2 = "AI.test_02"
    mgr.import_charts(prepare_chart_02(graph_name2))
    mgr.import_event(EventBuilder().build())

    c = Character("npc")
    asyncflow.register(c)
    graph = asyncflow.attach(c, graph_name)
    asyncflow.start(c)
    graph2 = asyncflow.attach(c, graph_name2)

    assert graph.start() == False                       # start a running graph will return False
    assert graph2.start() == True                       # start success
    
    for i in range(10):
        asyncflow.step(1000)

    assert graph.is_running() == True
    assert graph2.is_running() == True

    assert graph2.stop() == True                           # stop chart
    assert graph.is_running() == True
    assert graph2.is_running() == False
    assert graph2.stop() == False                          # stop again will return false
    asyncflow.exit()
    pass


if __name__ == '__main__':
    test_attach()
    test_remove()
    test_start()