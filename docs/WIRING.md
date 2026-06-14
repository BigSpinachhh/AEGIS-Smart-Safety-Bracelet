# Wiring Guide — Aegis Safety Bracelet

This document lists every connection between the ESP32 and the
peripheral modules. Pin numbers match `firmware/Aegis_Firmware/config.h`
— if you change a pin in the firmware, update it here too.

---

## 1. Power Distribution

```
3.7V Li-ion Battery (500–1000mAh)
        │
        ▼
   TP4056 Module ── (charges battery via USB-C/Micro-USB input)
        │
        ├── B+ / B- ──► Battery
        └── OUT+ / OUT- ──► 3.7–4.2V rail
                              │
                              ▼
                 ESP32 5V/VIN  +  Boost/Regulator (if required)
                              │
              ┌───────────────┼────────────────┬───────────────┐
              ▼                ▼                ▼               ▼
          NEO-6M GPS     SIM800L/A7670C     MPU6050         MAX9814 /
          (3.3–5V)       (4V, high current   (3.3V)         MAX30102
                          peaks — see note)                  (3.3V)
```

> **SIM800L power note:** The GSM module can draw current spikes of
> 1–2A during transmission. Power it directly from the battery /
> TP4056 output rail (not through the ESP32's onboard regulator),
> and add a bulk capacitor (≥470 µF–1000 µF) close to the module's
> VCC/GND pins.

---

## 2. ESP32 Pin Map

| Module | Module Pin | ESP32 Pin | Notes |
|---|---|---|---|
| **MPU6050** | VCC | 3.3V | |
| | GND | GND | |
| | SDA | GPIO21 | Shared I2C bus |
| | SCL | GPIO22 | Shared I2C bus |
| **MAX30102** | VIN | 3.3V | |
| | GND | GND | |
| | SDA | GPIO21 | Shared I2C bus with MPU6050 |
| | SCL | GPIO22 | Shared I2C bus with MPU6050 |
| **NEO-6M GPS** | VCC | 5V (VIN) | |
| | GND | GND | |
| | TX | GPIO16 (RX2) | GPS → ESP32 |
| | RX | GPIO17 (TX2) | ESP32 → GPS (3.3V is fine for NEO-6M RX) |
| **SIM800L / A7670C** | VCC | Battery/TP4056 OUT (4V) | **Not** from ESP32 3.3V — see power note above |
| | GND | GND (common ground) | |
| | TXD | GPIO26 (RX1) | GSM → ESP32 |
| | RXD | GPIO27 (TX1) | **via voltage divider** (see §3) |
| **MAX9814** | VDD | 3.3V | |
| | GND | GND | |
| | OUT | GPIO34 (ADC1_CH6) | Analog input only |
| | GAIN | leave floating (60dB) or tie per datasheet | |
| | A/R | leave floating (attack/release default) | |
| **SOS Button** | Pin 1 | GPIO4 | Other leg → GND |
| | Pin 2 | GND | Internal pull-up enabled in firmware |
| **Status LED** | Anode (+) | GPIO2 (via resistor) | |
| | Cathode (−) | GND | |
| **Buzzer** | + | GPIO25 | |
| | − | GND | |
| **Battery monitor** (optional) | Divider midpoint | GPIO35 (ADC1_CH7) | See §4 |

---

## 3. Voltage Divider — ESP32 → GSM RXD (GPIO27)

The ESP32 GPIO27 line passes through a resistor divider before
reaching the GSM module's RXD pin. This protects the module's RX
input and matches its expected logic level on 5V-tolerant /
SIM800L-style boards.

```
ESP32 GPIO27 (TX1, 3.3V logic)
        │
       [R1]
        │
        ├──────────────► GSM module RXD
        │
       [R2]
        │
       GND
```

A common ratio is **R1 = 10 kΩ, R2 = 20 kΩ**, giving roughly a
2/3 step-down. Choose values so the divider output sits within
the GSM module's documented "logic high" input range.

> If you are using an **A7670C** module with native 3.3V logic
> I/O, this divider is optional — the ESP32's 3.3V output can
> drive the RXD pin directly. Keeping the divider in the design
> does no harm (it simply attenuates a 3.3V signal further, which
> most modules still read correctly as logic-high if R1/R2 are
> chosen conservatively).

The GSM **TXD → ESP32 GPIO26 (RX1)** line should also be checked
against your specific module's logic-high voltage. If your module's
TXD can exceed 3.3V, add a second divider or a simple diode/zener
level-shifter on that line as well.

---

## 4. Battery Voltage Monitor (optional)

A simple two-resistor divider from the battery's positive terminal
to GPIO35 lets the firmware report battery voltage over Serial
(and, in future, over SMS).

```
Battery+ ──[R1]──┬──[R2]── GND
                 │
                 └──► GPIO35 (ADC1_CH7)
```

Set `BATTERY_DIVIDER_RATIO = (R1 + R2) / R2` in `config.h` to
match your chosen resistor values.

---

## 5. SOS Button Placement

The SOS button is mounted on the top face of the enclosure
(8 mm dome, see `docs/ENCLOSURE.md`) and wired between **GPIO4**
and **GND**. The firmware uses `INPUT_PULLUP`, so no external
pull-up resistor is required — a press pulls the pin LOW.

---

## 6. Quick Continuity Checklist Before First Power-On

- [ ] All module GNDs share a common ground with the ESP32 and battery
- [ ] GSM module powered from battery rail, **not** ESP32 3.3V pin
- [ ] Voltage divider present on GPIO27 → GSM RXD line
- [ ] I2C SDA/SCL (GPIO21/22) wired to **both** MPU6050 and MAX30102
- [ ] GPS TX → GPIO16, GPS RX → GPIO17 (not swapped)
- [ ] SOS button between GPIO4 and GND
- [ ] MAX9814 OUT → GPIO34 (an ADC1, input-only pin)
