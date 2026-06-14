# 🛡️ Aegis — Smart Safety Bracelet

**A Wearable Guardian for Women & Child Safety**

> *"Help should be one tap — or one fall — away."*

Aegis is a compact, wrist-worn safety device that detects
emergencies and alerts the people who matter most — automatically,
and without needing a smartphone, Wi-Fi, or mobile data. It combines
GPS tracking, cellular SOS messaging, fall/impact detection, vital-sign
monitoring, and ambient distress-sound detection into a single
low-power wearable built around the ESP32.

Built for **[zupp Faraway]** by **[Krishna Khokhar / Hala Madrid]**.

---

## 🚨 The Problem

- Personal-safety apps depend on a charged smartphone, mobile
  data, and the presence of mind to unlock and open an app while
  under threat — none of which can be guaranteed in an emergency.
- Most "SOS wearable" products are single-purpose: they send an
  alert, but give responders **no context** — no location, no
  vitals, no idea of what actually happened.
- Children, elderly family members, or someone being physically
  restrained may not be able to operate a phone at all.

This leaves a dangerous gap between *"something is wrong"* and
*"someone who can help knows about it and knows where to go."*

---

## 💡 Our Solution

Aegis closes that gap with a standalone bracelet that:

- Sends an **SOS with a live GPS location link** over the
  **cellular network** — works with no smartphone, no Wi-Fi, and
  no app to open
- **Automatically detects falls, impacts, and violent shaking**
  using a 6-axis accelerometer + gyroscope
- **Listens for distress sounds** (screaming, shouting) using an
  always-on microphone
- **Monitors heart rate** and flags abnormal spikes or drops that
  may indicate panic, struggle, or a medical emergency
- Runs for hours on a rechargeable Li-ion battery, topped up via a
  TP4056 charging module

Any one of these triggers is enough to fire an alert — the wearer
doesn't have to do anything for three of the four trigger paths.

---

## ✨ Features at a Glance

| Capability | How it works |
|---|---|
| 🔴 One-touch SOS | Dedicated button — press & hold sends SMS + GPS link to emergency contacts |
| 🤸 Fall / impact detection | MPU6050 continuously monitors motion for free-fall + impact signatures |
| 🔊 Distress sound detection | MAX9814 mic flags sudden loud sounds (screaming/shouting) |
| ❤️ Health monitoring | MAX30102 tracks heart rate; abnormal readings trigger an alert |
| 📍 Live location | NEO-6M GPS provides real-time coordinates embedded in every alert |
| 📶 Works anywhere | SIM800L / A7670C cellular module — no smartphone or Wi-Fi required |
| 🔋 All-day battery | 3.7V Li-ion battery (500–1000mAh) + TP4056 charge controller |
| ⌚ Wearable form factor | Custom-designed enclosure with silicone strap |

---

## 🧠 How It Works

```
   ┌──────────────┐   ┌──────────────┐   ┌──────────────┐   ┌──────────────┐
   │  SOS Button   │   │   MPU6050    │   │   MAX9814    │   │  MAX30102    │
   │ (manual press)│   │ (fall/shock) │   │(distress audio)│  │(heart rate) │
   └──────┬───────┘   └──────┬───────┘   └──────┬───────┘   └──────┬───────┘
          │                  │                  │                  │
          └──────────────────┴─────────┬────────┴──────────────────┘
                                        ▼
                                  ┌───────────┐
                                  │   ESP32   │
                                  │  (brain)  │
                                  └─────┬─────┘
                          ┌─────────────┴─────────────┐
                          ▼                           ▼
                  ┌──────────────┐           ┌──────────────────┐
                  │  NEO-6M GPS  │           │  SIM800L / A7670C │
                  │  (location)  │──────────►│   (SMS / Call)    │
                  └──────────────┘           └─────────┬─────────┘
                                                        ▼
                                          Emergency Contacts / Family
                                     "EMERGENCY ALERT! <reason>
                                      Location: maps.google.com/?q=..."
```

> **Result:** Any trigger — button press, detected fall, distress
> sound, or abnormal heart rate — causes the ESP32 to fetch the
> current GPS coordinates and send an SOS SMS with a Google Maps
> link to pre-configured emergency contacts, while a local
> buzzer/LED sounds an immediate alert.

---

## 🔧 Hardware & Design

### Schematic

[![Aegis Schematic](hardware/schematic/schematic.png)](hardware/schematic/schematic.png)

*(Add your EasyEDA schematic export to `hardware/schematic/schematic.png`)*

### Enclosure

CAD renders of the wearable enclosure live in
[`cad/renders/`](cad/renders/) — see
[`docs/ENCLOSURE.md`](docs/ENCLOSURE.md) for the full dimensional
specification (70 × 22 × 10 mm housing, M1.4 screw bosses, 20 mm
strap lugs, MAX30102 optical window, USB-A charging cutout, and
SOS button dome).

---

## 💻 Technology Stack

**Hardware**

- ESP32 Dev Module — main controller
- NEO-6M GPS Module — location tracking
- SIM800L / A7670C — cellular SMS & calling
- MPU6050 — accelerometer + gyroscope (fall/impact detection)
- MAX9814 — electret microphone amplifier (distress sound detection)
- MAX30102 — pulse oximeter & heart-rate sensor
- TP4056 — Li-ion battery charge controller
- 3.7V Li-ion battery (500–1000mAh)

**Firmware**

- Arduino framework (C/C++) on ESP32
- [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus) — NMEA parsing
- [Adafruit MPU6050](https://github.com/adafruit/Adafruit_MPU6050) + Adafruit Unified Sensor
- [SparkFun MAX3010x](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library) — heart-rate driver
- AT-command driven GSM communication (SMS/call)

**Design**

- EasyEDA — schematic capture & PCB layout
- CAD enclosure modeling (see [`docs/ENCLOSURE.md`](docs/ENCLOSURE.md))

---

## 📦 Bill of Materials

| Component | Qty | Approx. Cost (INR) |
|---|---|---|
| ESP32 Dev Board | 1 | ₹400 |
| NEO-6M GPS Module | 1 | ₹450 |
| SIM800L / A7670C Module | 1 | ₹700 |
| MPU6050 | 1 | ₹100 |
| MAX9814 Mic Module | 1 | ₹150 |
| MAX30102 Heart Rate Sensor | 1 | ₹200 |
| TP4056 Charging Module | 1 | ₹40 |
| 3.7V Li-ion Battery (500–1000mAh) | 1 | ₹250 |
| SOS Push Button, LED, Buzzer | 1 set | ₹30 |
| Custom PCB + passives | 1 | ₹280 |
| 3D-printed enclosure + strap | 1 | ₹225 |
| **Total** | | **~₹2,300 – ₹3,000** |

Full breakdown and sensor-by-sensor justification:
[`docs/BOM.md`](docs/BOM.md)

---

## 📂 Repository Structure

```
.
├── firmware/
│   └── Aegis_Firmware/
│       ├── Aegis_Firmware.ino   # Main ESP32 sketch
│       └── config.h              # Pins, thresholds, emergency contacts
├── hardware/
│   ├── schematic/                # EasyEDA schematic exports
│   └── pcb/
│       └── gerber/                # Gerber files for fabrication
├── cad/
│   ├── source/                    # Editable enclosure CAD files
│   └── renders/                   # STL + preview renders
├── docs/
│   ├── WIRING.md                  # Full pin connection table
│   ├── BOM.md                     # Bill of materials
│   ├── ENCLOSURE.md               # Enclosure dimensions/spec
│   └── SETUP.md                   # Assembly & flashing guide
├── assets/
│   └── images/                    # README images / demo photos
├── .gitignore
├── LICENSE
└── README.md
```

---

## ⚡ Pin Connections

Full table with notes: [`docs/WIRING.md`](docs/WIRING.md)

| Module | ESP32 Pin |
|---|---|
| MPU6050 SDA / SCL | GPIO21 / GPIO22 |
| MAX30102 SDA / SCL | GPIO21 / GPIO22 (shared I2C bus) |
| NEO-6M TX / RX | GPIO16 / GPIO17 |
| SIM800L / A7670C TX / RX | GPIO26 / GPIO27 (RX via voltage divider) |
| MAX9814 AOUT | GPIO34 |
| SOS Button | GPIO4 |
| Status LED | GPIO2 |
| Buzzer | GPIO25 |

---

## 🚀 Getting Started

### 1. Hardware Assembly

1. Build the PCB from [`hardware/pcb/gerber/`](hardware/pcb/gerber/)
2. Wire all modules per [`docs/WIRING.md`](docs/WIRING.md),
   including the voltage divider on the GSM RXD line
3. Connect the Li-ion battery through the TP4056 module
4. Fit everything into the enclosure from [`cad/`](cad/)

### 2. Firmware

1. Install the Arduino IDE + ESP32 board package
2. Install libraries: `TinyGPS++`, `Adafruit MPU6050`,
   `Adafruit Unified Sensor`, `SparkFun MAX3010x`
3. Open `firmware/Aegis_Firmware/Aegis_Firmware.ino`
4. In `config.h`, set your emergency contact numbers
5. Select **ESP32 Dev Module**, choose the COM port, and upload

Full step-by-step guide: [`docs/SETUP.md`](docs/SETUP.md)

### 3. Test

- Press & hold the SOS button → confirm SMS with location arrives
- Drop/tap the bracelet → confirm fall/impact alert fires
- Clap near the mic → confirm sound-trigger alert fires
- Place a finger on the MAX30102 → confirm heart rate logs appear

---

## 🎬 Project Status

- [x] Component selection finalized
- [x] Schematic design (EasyEDA)
- [x] Voltage divider for GSM RX level shifting
- [x] CAD enclosure modeling
- [x] Firmware: SOS button, fall detection, sound detection, heart
      rate monitoring, GPS + GSM SOS messaging
- [ ] PCB fabrication & assembly
- [ ] End-to-end integration testing on assembled hardware
- [ ] Final wearable assembly with strap

---

## 🔭 Roadmap

- Companion mobile app for live tracking & contact management
- Geofencing with safe-zone entry/exit alerts
- On-device audio classification for distress sounds (vs. general noise)
- Cloud dashboard for guardians/family members
- Battery-level reporting via SMS
- Production-ready, miniaturized PCB

---

## 👥 Team

Built for **[Zupp Faraway]** by **[Krishna Khokhar / Hala Madrid]**.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
