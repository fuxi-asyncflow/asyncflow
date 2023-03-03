
class Character:
    def _output(self, msg):
        print(msg)

    def __init__(self, name):
        self.name = name

    def Say(self, msg):
        self._output("{name}: {msg}".format(name = self.name, msg = msg))
