import matplotlib.pyplot as plt
import csv
import numpy as np


import sys
import math


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        pass
 
    try:
        import unicodedata
        unicodedata.numeric(s)
        return True
    except (TypeError, ValueError):
        pass
 
    return False

if (len(sys.argv)<2):
	print ("Log File parameter should be provided.")
	quit()

file = sys.argv[1]

data = []
with open(file) as inputfile:
	for row in csv.reader(inputfile):
		if (is_number(row[0])):
			data.append(float(row[0]))

print("File length:" + str(len(data)))

data = np.asarray(data)

fig = plt.figure()
ax1 = fig.add_subplot(111)

ax1.plot(data,'r', label='RMS')
plt.legend(loc='upper left')
plt.show()

output = []
with open("output.dat") as inputfile:
	for row in csv.reader(inputfile):
                output.append(float(row[0]))

input = []
with open("input.dat") as inputfile:
	for row in csv.reader(inputfile):
                input.append((float(row[1]), float(row[2]), float(row[3])))

input = np.asarray(input)
output = np.asarray(output)

print (input.shape)

print (input)

real = list(map(lambda x,y,z: ((math.sin(x * math.pi + math.pi) + math.cos(y * math.pi + math.pi) + z) / 3), input[:,0],input[:,1],input[:,2]))


print ( real )
print ( output )

fig = plt.figure()
ax1 = fig.add_subplot(111)
ax1.plot(output, 'r',label='predicted')
ax1.plot(real, 'b', label='real')
plt.show()
