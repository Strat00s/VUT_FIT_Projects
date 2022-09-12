# minitask 2
# change the last du to DU
import re
pattern = re.compile(r'(du)(?!.*du)')   #negative lookahead. 'du', after which there is no other 'du'
text = ['du du du', 'du po ledu', 'dopředu du', 'i dozadu du', 'dudu dupl']
for row in text:
    print(re.sub(pattern, 'DU', row))