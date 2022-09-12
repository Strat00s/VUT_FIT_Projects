#!/usr/bin/env python3

#main function body
def log_and_count(*args, **kwargs):
    #actual decorator
    def decorator(func):
        #wrapper for accessing function args and printing everything
        def wrapper(*func_args, **func_kwargs):
            #save counter
            counter = kwargs["counts"]
            
            #get counter key ("key" or function name)
            if "key" in kwargs:
                key = kwargs.get("key")
            else:
                key = func.__name__
            counter[key] += 1
            
            #print what was called
            print("called", func.__name__, "with", func_args, "and", func_kwargs)
            return func(*func_args, **func_kwargs)
        return wrapper
    return decorator
