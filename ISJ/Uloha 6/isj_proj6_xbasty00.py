#!/usr/bin/env python3

class Polynomial():
    def __init__(self, *args, **kwargs):
        self.values = list()

        #values are in a list
        for i in args:
            if type(i) is list:
                self.values = i #save list

        #values are
        if not self.values:
            #plain numbers
            if args:
                self.values = list(args) #save them as a list

            #"definitions"
            else:
                defs = [(int(key[1:]), val) for key, val in kwargs.items()] #create a list of tuples from kwargs
                defs.sort(key=lambda tup: tup[0])                           #sort it by x
                old_x = 0

                #go throught the tuple list and save value or add 0
                for key, val in defs:
                    while key != old_x:
                        self.values.append(0)
                        old_x += 1
                    self.values.append(val)
                    old_x += 1
        #remove last/extra zeros
        while len(self.values) > 1 and self.values[-1] == 0:
            self.values.pop(-1)

    #print values
    def __str__(self):
        string = "" #string output
        max_factor = len(self.values) - 1 #maximum x factor
        #containes only zero
        if max_factor == 0 and self.values[0] == 0:
            return "0"
        for val in reversed(self.values):
            #skip empty values
            if val == 0:
                max_factor -= 1
                continue
            #print sign
            if val > 0:
                if len(string) != 0:
                    string += "+ "
                #x with value 1 >> 1x -> x
                if val != 1 or max_factor == 0:
                    string += str(val)
            else:
                string += "- "
                #x with value -1 >> -1x -> -x
                if val != -1 or max_factor == 0:
                    string += str(val)[1:]
            #dont print x on factor 0
            if max_factor == 0:
                break
            #dont print ^ on factor 1
            elif max_factor == 1:
                string += "x "
            #print x^y
            else:
                string += "x^"
                string += str(max_factor) + " "
            max_factor -= 1
        return string.rstrip()
    
    #compare
    def __eq__(self, other):
        #have to be same lenght
        if len(self.values) != len(other.values):
            return False
        #go through values and compare
        for i in range(len(self.values)):
            if self.values[i] != other.values[i]:
                return False
        return True

    #add
    def __add__(self, other):
        self_len = len(self.values)
        other_len = len(other.values)
        diff = abs(self_len - other_len)
        new_vals = list()
        #extend lists by 0s, when not equal
        if self_len > other_len:
            other.values.extend([0] * diff)
        if self_len < other_len:
            self.values.extend([0] * diff)
        #add values and return new list
        for i in range(len(self.values)):
            new_vals.append(self.values[i] + other.values[i])
        return Polynomial(new_vals)

    #multiply
    def __mul__(self, other):
        mul = [0] * (len(self.values) * len(other.values)) #make new eq with size of eq1*eq2
        #go through both and multiply
        for i in range(len(self.values)):
            for j in range(len(other.values)):
                mul[i + j] += self.values[i] * other.values[j]
        return Polynomial(mul)
    
    #power
    def __pow__(self, n):
        #raise error on negative power
        if n < 0:
            raise ValueError("Raising to negative power not supported")
        #0^0 not defined
        elif n == 0:
            if len(self.values) == 1 and self.values[0] == 0:
                raise ValueError("Zero to power of zero not defined")
            return 1
        #power of 1 -> no change
        elif n == 1:
            return self
        else:
            #create a copy, which we will be multiplying with the default
            tmp = self
            for _ in range(1, n):
                tmp *= self
            return tmp
    
    #derivate
    def derivative(self):
        #derivation of a constant is 0
        if len(self.values) == 1:
            return 0
        der = list()
        #derivate except firt element
        for i in range(len(self.values) - 1):
            der.append(self.values[i + 1] * (i + 1))
        return Polynomial(der)

    #calculate result when x = a for at_value(a) or x - y where x = a, y = b for at_value(a, b)
    def at_value(self, a, b=None):
        #save first value (nothing to multiply)
        val = self.values[0]
        #add them up
        for i in range(len(self.values) - 1):
            val +=  self.values[i + 1] * (a ** (i + 1))
        #nothing else to do
        if b == None:
            return val
        #save first value (nothing to multiply)
        val2 = self.values[0]
        #add them up
        for i in range(len(self.values) - 1):
            val2 += self.values[i + 1] * (b ** (i + 1))
        #subtract
        return val2 - val

def test():
    assert str(Polynomial(0,1,0,-1,4,-2,0,1,3,0)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial([-5,1,0,-1,4,-2,0,1,3,0])) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x - 5"
    assert str(Polynomial(x7=1, x4=4, x8=3, x9=0, x0=0, x5=-2, x3= -1, x1=1)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial(x2=0)) == "0"
    assert str(Polynomial(x0=0)) == "0"
    assert Polynomial(x0=2, x1=0, x3=0, x2=3) == Polynomial(2,0,3)
    assert Polynomial(x2=0) == Polynomial(x0=0)
    assert str(Polynomial(x0=1)+Polynomial(x1=1)) == "x + 1"
    assert str(Polynomial([-1,1,1,0])+Polynomial(1,-1,1)) == "2x^2"
    pol1 = Polynomial(x2=3, x0=1)
    pol2 = Polynomial(x1=1, x3=0)
    assert str(pol1+pol2) == "3x^2 + x + 1"
    assert str(pol1+pol2) == "3x^2 + x + 1"
    assert str(Polynomial(x0=-1,x1=1)**1) == "x - 1"
    assert str(Polynomial(x0=-1,x1=1)**2) == "x^2 - 2x + 1"
    pol3 = Polynomial(x0=-1,x1=1)
    assert str(pol3**4) == "x^4 - 4x^3 + 6x^2 - 4x + 1"
    assert str(pol3**4) == "x^4 - 4x^3 + 6x^2 - 4x + 1"
    assert str(Polynomial(x0=2).derivative()) == "0"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative()) == "6x^2 + 3"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative().derivative()) == "12x"
    pol4 = Polynomial(x3=2,x1=3,x0=2)
    assert str(pol4.derivative()) == "6x^2 + 3"
    assert str(pol4.derivative()) == "6x^2 + 3"
    assert Polynomial(-2,3,4,-5).at_value(0) == -2
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3) == 20
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3,5) == 44
    pol5 = Polynomial([1,0,-2])
    assert pol5.at_value(-2.4) == -10.52
    assert pol5.at_value(-2.4) == -10.52
    assert pol5.at_value(-1,3.6) == -23.92
    assert pol5.at_value(-1,3.6) == -23.92

if __name__ == '__main__':
    test()
