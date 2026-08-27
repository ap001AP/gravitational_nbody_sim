import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("energy.csv")

initial_energy = data["Energy"].iloc[0]
final_energy = data["Energy"].iloc[-1]
energy_error = (abs(final_energy - initial_energy) / abs(initial_energy))*100

plt.plot(data["Time"], data["Energy"])
plt.axhline(initial_energy, color="gray", linestyle="--", label="E_initial")
plt.axhline(final_energy, color="red", linestyle="--", label="E_final")

plt.xlabel("Time")
plt.ylabel("Energy")
plt.title("Energy Conservation Over Time")

plt.xlim(0, data["Time"].iloc[-1])
plt.ylim(data["Energy"].min(), data["Energy"].max())

plt.legend()

plt.text(0.05, 0.9, f"ΔE = {energy_error:.2e}%", transform=plt.gca().transAxes)

plt.show()