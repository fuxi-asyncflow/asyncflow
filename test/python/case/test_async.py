import sys
sys.path.append('../')
sys.path.append('.')

import pytest

import time
import asyncio
import asyncflow
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

async def async_loop(seconds):
    start_time = time.time()
    end_time = start_time + seconds
    delta_time = 0.05    
    while time.time() < end_time:
        print('step ...')
        asyncflow.step(int(delta_time*1000))
        await asyncio.sleep(delta_time)

@pytest.mark.parametrize("remove_graph", [(True,), (False,)])
def test_async_remove_graph_after_activate(remove_graph):
    c = Character("npc")
    mgr = asyncflow.setup()
    graph_name = "AI.test_async_remove_graph_after_activate"

    def async_node(self):
        async def async_func(ctx):
            await asyncio.sleep(0.1)
            ctx.continue_node(True)
            if remove_graph:
                asyncflow.remove(self, graph_name)
        ctx = asyncflow.suspend_node()
        asyncio.create_task(async_func(ctx))
        return True

    graph = GraphBuilder(graph_name, "Character")
    n1 = graph.add_func_node(say_one)
    n2 = graph.add_func_node(async_node)
    n3 = graph.add_func_node(say_two)
    
    graph.connect_from_start(n1)
    graph.connect(n1, n2)
    graph.connect(n2, n3)    

    mgr.import_charts(graph.build())
    mgr.import_event(EventBuilder().build())
    
    actual = []    
    Character._output = actual.append
    asyncflow.register(c)
    asyncflow.attach(c, graph_name)
    
    asyncflow.start(c)
    asyncio.run(async_loop(1.0))
    asyncflow.exit()

    expected = []
    Character._output = expected.append
    say_one(c)
    if not remove_graph:
        say_two(c)
    assert expected == actual

if __name__ == '__main__':
    test_async_remove_graph_after_activate()

    

