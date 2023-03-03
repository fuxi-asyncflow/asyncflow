
class Character:
    def _output(msg):
        print(msg)

    def __init__(self, name):
        self.name = name

    def Say(self, msg):
        Character._output("{name}: {msg}".format(name = self.name, msg = msg))
