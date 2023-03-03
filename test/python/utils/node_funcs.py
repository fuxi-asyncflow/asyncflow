import sys
sys.path.append('../')
import asyncflow

def say_one(self):
    self.Say("one")

def say_two(self):
    self.Say("two")

def say_three(self):
    self.Say("three")

def say_four(self):
    self.Say("four")

def wait_1s(self):
    asyncflow.wait(1.0)

def wait_func(second):
    def _wait(self):
        asyncflow.wait(second)
    return _wait

def set_var(name, value):
    def _set_var(self):
        asyncflow.set_var(name, value)
    return _set_var

def get_var(name):
    def _get_var(self):
        v = asyncflow.get_var(name)
        self.Say(v)
    return _get_var

def subchart(name, obj, *args):    
    def _subchart(self):
        asyncflow.call_sub(name, obj, *args)
        return True
    return _subchart