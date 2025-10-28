import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys
import os

file = sys.argv[1]

data = pd.read_csv(file, skiprows=20)

values = data.values

plt.plot(values)
plt.show()
