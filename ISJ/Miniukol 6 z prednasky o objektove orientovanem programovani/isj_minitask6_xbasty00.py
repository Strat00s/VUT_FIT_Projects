# minitask 6
class Point:
    def __init__(self, x = 0, y = 0):
        self.x = x
        self.y = y
    def __sub__(self, other):
        new_x = self.x - other.x
        new_y = self.y - other.y
        return Point(new_x, new_y)
    def __str__(self):
        return str(self.x) + " " + str(self.y)

p0 = Point()
p1 = Point(3, 4)
print(p0-p1)
p2 = Point(1, 2)
result = p1-p2
print(result)