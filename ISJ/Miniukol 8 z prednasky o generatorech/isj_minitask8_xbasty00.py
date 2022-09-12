# minitask 8
# f is a general iterator, for example a file
f = open("students.txt")
header = f.readline().rstrip() #save first line to header
print(header)
#go through rest of the file
for line in f:
    line = line.rstrip()
    print(line)