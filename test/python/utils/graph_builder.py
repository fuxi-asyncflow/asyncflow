import uuid
import asyncflow

def gen_uuid():
    return uuid.uuid4()

class ConnType():
    Failure = 0
    Success = 1
    Always = 2

class NodeType():
    FUNC = 0
    CONTROL = 1
    EVENT = 2

class GraphBuilder:
    def __init__(self, path, type):
        self.path = path
        self.type = type
        self.uid = gen_uuid()
        self.nodes = []
        self.connectors = []
        self.start_node = gen_uuid()
        self.variables = []

    def get_start_node(self):
        return self.start_node

    def add_func_node(self, f, **kwargs):
        uid = gen_uuid()
        func_name ="{path}.{uid}".format(path = self.path, uid = uid.hex)
        asyncflow.set_node_func(func_name, f)
        node = {"uid": uid, "type": NodeType.FUNC}
        node.update(kwargs)
        self.nodes.append(node)
        return uid
    
    def add_event_node(self, f):
        uid = self.add_func_node(f)
        for node in self.nodes:
            if node['uid'] == uid:
                node['type'] = NodeType.EVENT
        return uid
    
    def add_ret_var_node(self, f, var_name):
        uid = self.add_func_node(f)
        for node in self.nodes:
            if node['uid'] == uid:
                node['return_var_name'] = var_name
        return uid


    def add_control_node(self, name, args):
        uid = gen_uuid()
        self.nodes.append({"uid": uid, "type": NodeType.CONTROL, "contents":[name] + args})
        return uid

    def add_varialble(self, name, type, isParameter = False):
        self.variables.append({"name": name, "type": type, "is_param": isParameter})

    def connect(self, start, end, type = ConnType.Always):
        self.connectors.append({"start": start, "end": end, "type": type})

    def connect_from_start(self, end):
        self.connect(self.get_start_node(), end, ConnType.Always)

    def build(self):
        lines = []
        lines.append("--- ")
        lines.append("path: {}".format(self.path))
        lines.append("uid: {}".format(self.uid))
        lines.append("type: {}".format(self.type))

        if len(self.variables) > 0:
            lines.append("variables: ")
            for v in self.variables:
                lines.append("- ")
                lines.append("  name: {}".format(v['name']))
                lines.append("  type: {}".format(v['type']))
                if v["is_param"]:
                    lines.append("  is_param: true")

        lines.append("nodes: ")
        lines.append("- ")
        lines.append("  uid: {}".format(self.start_node))

        for node in self.nodes:
            lines.append("- ")
            lines.append("  uid: {}".format(node['uid']))
            if node.get('no_loop_check', False):
                lines.append("  no_loop_check: true")
            if node['type'] == NodeType.FUNC:
                lines.append("  code:")
                lines.append("    type: FUNC")
                if 'return_var_name' in node:
                    lines.append("    return_var_name: {}".format(node['return_var_name']))

            elif node['type'] == NodeType.EVENT:
                lines.append("  code:")
                lines.append("    type: EVENT")
            elif node['type'] == NodeType.CONTROL:
                lines.append("  code:")
                lines.append("    type: CONTROL")
                lines.append('    content: ')
                for line in node['contents']:
                    lines.append('    - {} '.format(line))

        lines.append("connectors: ")
        for conn in self.connectors:
            lines.append("- ")
            lines.append("  start: {}".format(conn['start']))
            lines.append("  end: {}".format(conn['end']))
            lines.append("  type: {}".format(conn['type']))

        lines.append("...")
        return '\n'.join(lines)


class EventBuilder:
    def __init__(self):
        self.events = []
        self.add_event("Start")
        self.add_event("Tick")

    def add_event(self, name, parameters = None):
        id = len(self.events) + 1        
        self.events.append({"id": id, "name": name, "parameters": parameters})        

    def build(self):
        lines = []
        lines.append("--- ")
        for ev in self.events:
            lines.append("- ")
            lines.append("  id: {}".format(ev['id']))
            lines.append("  name: {}".format(ev['name']))
        params = ev['parameters']
        if params and len(params):
            lines.append("  parameters: ")
            for p in params:
                lines.append("  - {{name: {name}, type: {type}}}".format(name=p['name'], type=p['type']))

        lines.append("...")
        return '\n'.join(lines)


if __name__ == '__main__':
    graph_name = "AI.test"
    graph = GraphBuilder(graph_name, "Character")

    def f(self):
        print("hello")

    def g(self):
        print("world")
    
    n1 = graph.add_func_node(f)
    n2 = graph.add_func_node(g)
    graph.connect_from_start(n1)
    graph.connect(n1, n2, ConnType.Always)
    graph.connect(n2, n1, ConnType.Always)

    graph_lines = graph.build()
    print(graph_lines)

    ev_lines = EventBuilder().build()
    print(ev_lines)

    asyncflow.setup()
    asyncflow.import_charts(graph_lines)
    asyncflow.import_event(ev_lines)

    asyncflow.register(graph)
    asyncflow.attach(graph, graph_name)
    asyncflow.start(graph)

    for i in range(10):
        asyncflow.step(1000)
    
    asyncflow.exit()