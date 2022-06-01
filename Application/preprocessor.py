f = open('output.txt','r')
d = open('pro_output.txt','w')

g = open('pro_output2.txt','w')

text = f.readline()
print(len(text))
for i in range(len(text)):
    d.write(text[i])
    if (i + 1) % 100 == 0:
        d.write('\n')

print(text)
for i in range(len(text)):
    g.write(text[i])
    if (i+1) % 10 ==0:
        g.write('\n')
    if (i + 1) % 100 == 0:
        g.write('\n')


f.close()
d.close()
g.close()

