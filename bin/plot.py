import matplotlib.pyplot as plt
import csv
import numpy as np


import sys


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

plt.savefig('output.png')

plt.show()



