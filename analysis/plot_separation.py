import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("separation.csv")

plt.plot(data["Time"], data["Separation"])

plt.xlabel("Time")
plt.ylabel("Separation")
plt.title("Sensitivity to Initial Conditions")

plt.xlim(0, data["Time"].iloc[-1])
plt.ylim(0.95*data["Separation"].min(), 1.05*data["Separation"].max())


plt.yscale("log")
plt.show()