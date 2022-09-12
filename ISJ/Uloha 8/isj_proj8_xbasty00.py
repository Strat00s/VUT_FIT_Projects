def first_with_given_key(iterable, key=lambda a : a):
    done = list()
    for value in iterable:
        if key(value) not in done:
            done.append(key(value))
            yield value

