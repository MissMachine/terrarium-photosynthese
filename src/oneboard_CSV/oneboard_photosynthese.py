"""
Logger série OneBoard → CSV

Rôle
----
Ce script lit un flux de mesures texte disponible sur un port série,
en extrait les valeurs CO2, luminosité, température et humidité,
puis les enregistre périodiquement dans un fichier CSV.

Il accepte plusieurs formats de lignes afin de faciliter l'exploitation
des données reçues.

Ce script concerne uniquement :
    port série → décodage des mesures → CSV

Il ne gère pas :
    - la lecture directe des capteurs ;
    - une communication Wi-Fi ou UDP ;
    - un ESP32 ;
    - un affichage OLED.

Paramètres à adapter
--------------------
PORT         : port série utilisé par la carte
BAUD         : vitesse de communication série
INTERVAL_MIN : intervalle entre deux écritures CSV
OUT_CSV      : nom ou chemin du fichier CSV

Historique
----------
Ce script correspond à une étape antérieure du dispositif de
photosynthèse, utilisant une récupération des mesures par liaison série.
"""


###########################################################################


import csv
import os
import re
import time
from datetime import datetime
import serial

# --------- À ADAPTER ----------
PORT = "/dev/ttyUSB0"          # Windows: "COM3" ; Linux: "/dev/ttyUSB0" ou "/dev/ttyACM0"
BAUD = 115200
INTERVAL_MIN = 15      # 10 ou 15 minutes
OUT_CSV = "Oneboard_photosynthese.csv"
# -----------------------------

# Essaie d'extraire co2/lux/temp/hum depuis une ligne texte.
# Accepte par ex:
# "CO2: 612 ppm | Lux: 123.4 | T: 23.56 C | RH: 45.2 %"
# "co2=612,lux=123,temp=23.56,hum=45.2"
# "612;123;23.56;45.2" (dans l'ordre co2,lux,temp,hum)
def parse_measurements(line: str):
    s = line.strip()
    if not s:
        return None

    # 1) Formats "clé=valeur" / "clé: valeur"
    patterns = {
        "co2": r"(?:\bco2\b|\beco2\b)\s*[:=]\s*([0-9]+(?:\.[0-9]+)?)",
        "lux": r"\blux\b\s*[:=]\s*([0-9]+(?:\.[0-9]+)?)",
        "temp": r"(?:\btemp\b|\bt\b)\s*[:=]\s*([0-9]+(?:\.[0-9]+)?)",
        "hum": r"(?:\bhum\b|\brh\b|\bhumidity\b|\bh\b)\s*[=:]\s*([0-9]+(?:\.[0-9]+)?)",
    }

    data = {}
    for key, pat in patterns.items():
        m = re.search(pat, s, flags=re.IGNORECASE)
        if m:
            data[key] = float(m.group(1))

    if len(data) >= 2:
        # si on a au moins 2 champs, on accepte (tu peux rendre ça strict à 4 si tu veux)
        return {
            "co2_ppm": int(round(data["co2"])) if "co2" in data else "",
            "lux": data.get("lux", ""),
            "temp_c": data.get("temp", ""),
            "hum_pct": data.get("hum", ""),
            "raw": s
        }

    # 2) Format "612;123;23.56;45.2" ou "612,123,23.56,45.2"
    #    (ordre: co2,lux,temp,hum)
    m2 = re.match(r"^\s*([0-9]+(?:\.[0-9]+)?)\s*[,;]\s*([0-9]+(?:\.[0-9]+)?)\s*[,;]\s*([0-9]+(?:\.[0-9]+)?)\s*[,;]\s*([0-9]+(?:\.[0-9]+)?)\s*$", s)
    if m2:
        co2, lux, temp, hum = map(float, m2.groups())
        return {
            "co2_ppm": int(round(co2)),
            "lux": lux,
            "temp_c": temp,
            "hum_pct": hum,
            "raw": s
        }

    return None


def ensure_csv_header(path: str):
    need_header = not os.path.exists(path) or os.path.getsize(path) == 0
    if need_header:
        with open(path, "w", newline="", encoding="utf-8") as f:
            w = csv.writer(f)
            w.writerow(["timestamp", "co2_ppm", "lux", "temp_c", "hum_pct", "raw"])


def main():
    ensure_csv_header(OUT_CSV)

    interval_s = INTERVAL_MIN * 60
    next_write = 0.0

    with serial.Serial(PORT, BAUD, timeout=2) as ser:
        # petit délai pour laisser la carte reset sur ouverture du port
        time.sleep(2)
        ser.reset_input_buffer()

        print(f"Lecture sur {PORT} @ {BAUD} — log toutes les {INTERVAL_MIN} min → {OUT_CSV}")
        last_good = None

        while True:
            try:
                line = ser.readline().decode("utf-8", errors="replace")
            except Exception as e:
                print("Erreur lecture série:", e)
                time.sleep(1)
                continue

            parsed = parse_measurements(line)
            if parsed:
                last_good = parsed
                # debug console (optionnel)
                print("OK:", parsed["co2_ppm"], "ppm |", parsed["lux"], "lux |", parsed["temp_c"], "°C |", parsed["hum_pct"], "%")

            now = time.time()
            if next_write == 0.0:
                next_write = now + interval_s

            if now >= next_write:
                ts = datetime.now().isoformat(timespec="seconds")
                if last_good is None:
                    print("Aucune mesure valide à écrire à", ts, "(j'attends une ligne parsable)")
                else:
                    with open(OUT_CSV, "a", newline="", encoding="utf-8") as f:
                        w = csv.writer(f)
                        w.writerow([ts, last_good["co2_ppm"], last_good["lux"], last_good["temp_c"], last_good["hum_pct"], last_good["raw"]])
                    print("✍️ écrit:", ts)
                next_write += interval_s


if __name__ == "__main__":
    main()

