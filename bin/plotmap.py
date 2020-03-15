# This Python file uses the following encoding: utf-8

# if__name__ == "__main__":
#     pass

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
if (False):
    data = []
    with open(file) as inputfile:
            for row in csv.reader(inputfile):
                    print (row)
                    if (len(row)>0 and is_number(row[0])):
                            data.append([float(row[0]), float(row[1])])


myvars = {}
with open(file) as myfile:
    for line in myfile:
        name, var = line.partition("=")[::2]
        myvars[name.strip()] = var

print(myvars["pattern.size"])

for i in range(int(myvars["pattern.size"])):
    data.append( [ float( myvars[ 'pattern.in.'+'{:d}'.format(i)+'.0']), float( myvars[ 'pattern.in.'+'{:d}'.format(i)+'.1']) ])

print("File length:" + str(len(data)))

data = np.asarray(data)

fig = plt.figure()
ax1 = fig.add_subplot(111)

ax1.scatter(data[:,0], data[:,1], s=10, c='b', marker="x", label='Weights')
#ax1.scatter(data[:,0], data[:,1], s=10, c='r', marker="o", label='Closed')
plt.xlabel('Lat')
plt.ylabel('Long')
plt.legend(loc='upper left')
plt.savefig('map.png')
plt.show()
