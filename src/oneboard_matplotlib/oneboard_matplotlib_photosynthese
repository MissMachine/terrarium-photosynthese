import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("Oneboard_photosynthese.csv")
df["timestamp"] = pd.to_datetime(df["timestamp"])

fig, axs = plt.subplots(2, 1, figsize=(10, 6), sharex=True)

# CO2
axs[0].plot(df["timestamp"], df["co2_ppm"])
axs[0].set_ylabel("CO₂ (ppm)")
axs[0].set_title("Évolution du CO₂")

# Lux
axs[1].plot(df["timestamp"], df["lux"])
axs[1].set_ylabel("Lux")
axs[1].set_title("Évolution de la lumière")
axs[1].set_xlabel("Temps")

for ax in axs:
    ax.grid(True)

fig.autofmt_xdate()
plt.tight_layout()
plt.show()

