import csv
import socket
from datetime import datetime
from pathlib import Path


# ============================================================
# À ADAPTER
# ============================================================

UDP_PORT = 4210

BASE_DIR = Path(__file__).resolve().parents[2]

CSV_PATH = (
    BASE_DIR
    / "data"
    / "photosynthese_waha"
    / "raw"
    / "photosynthese_log.csv"
)

# ============================================================
# NE RIEN MODIFIER SOUS CETTE LIGNE
# ============================================================

CSV_PATH.parent.mkdir(parents=True, exist_ok=True)

print("Writing to:", CSV_PATH)


def parse_kv(msg: str) -> dict:
    """
    msg: "CO2=845;LUX=120;T=21.6;H=52.4"
    return dict with numeric values when possible
    """

    out = {
        "CO2": None,
        "LUX": None,
        "T": None,
        "H": None,
    }

    parts = [
        p.strip()
        for p in msg.strip().split(";")
        if p.strip()
    ]

    for p in parts:
        if "=" not in p:
            continue

        k, v = p.split("=", 1)

        k = k.strip()
        v = v.strip()

        if k in ("CO2", "LUX"):
            try:
                out[k] = int(float(v))
            except:
                pass

        elif k in ("T", "H"):
            try:
                out[k] = float(v)
            except:
                pass

    return out


def main():
    sock = socket.socket(
        socket.AF_INET,
        socket.SOCK_DGRAM,
    )

    sock.bind(("", UDP_PORT))
    sock.settimeout(None)

    print(f"Listening UDP on port {UDP_PORT} ...")
    print(f"Writing CSV to: {CSV_PATH}")

    file_exists = False

    try:
        with open(CSV_PATH, "r", encoding="utf-8") as _:
            file_exists = True

    except FileNotFoundError:
        file_exists = False

    with open(
        CSV_PATH,
        "a",
        newline="",
        encoding="utf-8",
    ) as f:

        writer = csv.DictWriter(
            f,
            fieldnames=[
                "timestamp",
                "co2_ppm",
                "lux",
                "temp_c",
                "hum_pct",
                "raw",
            ],
        )

        if not file_exists:
            writer.writeheader()

        while True:
            data, addr = sock.recvfrom(2048)

            msg = data.decode(
                "utf-8",
                errors="replace",
            ).strip()

            ts = datetime.now().isoformat(
                timespec="seconds"
            )

            kv = parse_kv(msg)

            row = {
                "timestamp": ts,
                "co2_ppm": kv["CO2"],
                "lux": kv["LUX"],
                "temp_c": kv["T"],
                "hum_pct": kv["H"],
                "raw": msg,
            }

            writer.writerow(row)
            f.flush()

            print(
                f"{ts} from {addr[0]} {row}"
            )


if __name__ == "__main__":
    main()
