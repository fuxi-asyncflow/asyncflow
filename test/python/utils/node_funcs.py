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
        if obj == 'self':
            asyncflow.call_sub(name, self, *args)
        else:
            asyncflow.call_sub(name, obj, *args)
        return True
    return _subchart

def wait_event(obj, ev_name):
    def _wait_self_event(self):
        asyncflow.wait_event(self, getattr(asyncflow.EventId, ev_name))
        return True
    def _wait_event(self):
        v = asyncflow.get_var(obj)
        asyncflow.wait_event(v, getattr(asyncflow.EventId, ev_name))
        return True
    def _wait_event_obj(self):        
        asyncflow.wait_event(obj, getattr(asyncflow.EventId, ev_name))
        return True
    if obj == "self":
        return _wait_self_event
    elif obj is str:
        return _wait_event
    else:
        return _wait_event_obj

def print_ev_param(ev_name, idx):
    def _print_ev_param(self):
        ev_param = asyncflow.get_event_param(getattr(asyncflow.EventId, ev_name), idx)
        self.Say(ev_param)
    return _print_ev_param

def return_func(value):
    def _return(self):
        asyncflow.ret(value)
    return _return