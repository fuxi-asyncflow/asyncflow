import sys
sys.path.append('../')
sys.path.append('.')

import time
import asyncio
import aiohttp
from aiohttp import web
import requests
from multiprocessing import Process
import asyncflow
from utils.graph_builder import *
from utils.game_object import Character
from utils.node_funcs import *

LISTEN_PORT = 2888
TEST_URL = f'http://localhost:{LISTEN_PORT}/'

####################################################################
## server
async def hello(request):
    await asyncio.sleep(2.0)
    return web.Response(text="Hello")

def server(port):
    app = web.Application()
    app.add_routes([web.get('/', hello)])
    web.run_app(app, port=LISTEN_PORT)
    print("server down ======================================================================")

def start_server():
    p = Process(target=server, args=(LISTEN_PORT,))
    p.start()
    return p

async def async_hello():
    async with aiohttp.ClientSession() as session:
        async with session.get(TEST_URL) as resp:
            print(resp.status)
            print(await resp.text())

async def client_test():
    tasks = []
    for i in range(10):
        tasks.append(asyncio.create_task(async_hello()))
    await asyncio.gather(*tasks)

def start_client():
    p = start_server()
    asyncio.run(client_test())
    p.kill()
    
####################################################################

# async node func
async def async_http(ctx):
    timeout = aiohttp.ClientTimeout(total=1)
    async with aiohttp.ClientSession() as session:
        try:
            async with session.get(TEST_URL, timeout = timeout) as resp:
                result = await resp.text()
                ctx.continue_node(result)
        except Exception as e:
            ctx.cancel(False)

def async_node(self):
    ctx = asyncflow.suspend_node()
    asyncio.create_task(async_http(ctx))

def sync_node(var_name):
    def _node(self):
        resp = requests.get(TEST_URL)
        asyncflow.set_var(var_name, resp.text)
    return _node

## prepare graph
def prepare_chart_async(graph_name):    
    graph = GraphBuilder(graph_name, "Character")
    for i in range(5):
        var_name = f'v_{i}'
        graph.add_varialble(var_name, "String")
        n1 = graph.add_ret_var_node(async_node, var_name)
        #n1 = graph.add_func_node(sync_node(var_name))
        n2 = graph.add_func_node(get_var(var_name))
        n3 = graph.add_func_node(say_msg(f'{i} is cancelled'))
        
        graph.connect_from_start(n1)
        graph.connect(n1, n2, ConnType.Success)
        graph.connect(n1, n3, ConnType.Failure)
    return graph.build()

async def async_loop(seconds):
    start_time = time.time()
    end_time = start_time + seconds
    delta_time = 1.0    
    while time.time() < end_time:
        print('step ...')
        asyncflow.step(int(delta_time))
        await asyncio.sleep(delta_time)

def test_async_http():
    mgr = asyncflow.setup()
    graph_name = "AI.test_async"
    mgr.import_charts(prepare_chart_async(graph_name))
    mgr.import_event(EventBuilder().build())   

    c = Character("npc")    
    agent = asyncflow.register(c)
    graph = asyncflow.attach(c, graph_name)    
    asyncflow.start(c)    
    asyncio.run(async_loop(7))    
    asyncflow.exit()


if __name__ == '__main__':
    # start_client()

    p = start_server()
    time.sleep(1.0)         # wait server start
    try:
        test_async_http()
    finally:
        p.kill()

