# minitask 4
mcase = {'a':10, 'b': 34, 'A': 7, 'Z':3}    #default dict
wanted = {} #empt dict
for i in list(mcase):   #go through dict
    wanted[i.lower()] = wanted.get(i.lower(), 0) + mcase[i] #search wanted dict. If nothing found, add it. If found, increase the value
print(wanted)