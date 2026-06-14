# Setup Guide — Aegis Safety Bracelet

End-to-end instructions for assembling the hardware and flashing
the firmware.

---

## 1. Hardware Assembly

1. Review the schematic in `hardware/schematic/` and the pin map
   in [`docs/WIRING.md`](WIRING.md).
2. Solder/assemble the PCB using the Gerber files in
   `hardware/pcb/gerber/` (upload the zip directly to your
   fab house — e.g. JLCPCB, PCBWay).
3. Populate the board with:
   - ESP32 (module or dev board footprint)
   - NEO-6M GPS module
   - SIM800L or A7670C module
   - MPU6050
   - MAX9814
   - MAX30102
   - TP4056 charging module
   - Voltage divider resistors on the GSM RXD line (GPIO27)
   - SOS push button, status LED, buzzer
4. Connect the 3.7V Li-ion battery to the TP4056 module's `B+`/`B-`
   pads, and the TP4056 `OUT+`/`OUT-` to the main power rail.
5. Fit the assembled PCB into the printed enclosure from `cad/`
   (see [`docs/ENCLOSURE.md`](ENCLOSURE.md) for cutout dimensions),
   aligning the MAX30102 with its optical window and the SOS
   button with the top-face dome.

---

## 2. Firmware Setup (Arduino IDE)

### 2.1 Install board support

1. Open Arduino IDE → **File → Preferences** → add to
   "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
2. **Tools → Board → Boards Manager** → search "esp32" → install
   the **esp32 by Espressif Systems** package.
3. **Tools → Board** → select **ESP32 Dev Module**.

### 2.2 Install required libraries

Via **Sketch → Include Library → Manage Libraries**, install:

| Library | Author | Used for |
|---|---|---|
| TinyGPS++ | Mikal Hart | Parsing NEO-6M NMEA data |
| Adafruit MPU6050 | Adafruit | Accelerometer/gyro driver |
| Adafruit Unified Sensor | Adafruit | Dependency of the above |
| SparkFun MAX3010x Pulse and Proximity Sensor Library | SparkFun | MAX30102 heart-rate driver |

### 2.3 Configure

1. Open `firmware/Aegis_Firmware/Aegis_Firmware.ino`.
2. Open the `config.h` tab and set:
   - `EMERGENCY_NUMBER_1` / `EMERGENCY_NUMBER_2` — real numbers
     with country code (e.g. `"+91XXXXXXXXXX"`)
   - Adjust detection thresholds (`IMPACT_THRESHOLD_G`,
     `SOUND_THRESHOLD`, `HEART_RATE_LOW/HIGH`) if needed after
     bench testing
3. Connect the ESP32 via USB, select the correct **Port**, and
   click **Upload**.

### 2.4 First Boot Checklist

Open the Serial Monitor at **115200 baud** and confirm:

- [ ] `[MPU6050] Initialized.`
- [ ] `[MAX30102] Initialized.`
- [ ] `[GSM] Ready.`
- [ ] GPS fix becomes `YES` after a minute or two outdoors
- [ ] Pressing and holding the SOS button for ~1 second prints
      `[ALERT] TRIGGERED: Manual SOS button pressed`
      and an SMS arrives at the configured number

---

## 3. Bench Testing the Detection Logic

| Test | How | Expected Result |
|---|---|---|
| SOS button | Press & hold ~1s | SMS with location sent, buzzer sounds |
| Fall detection | Gently drop the bracelet onto a soft surface from ~30cm | `Fall detected` alert |
| Impact detection | Tap the enclosure firmly | `Sudden impact / shock detected` alert |
| Sound detection | Clap loudly near the mic | `Loud distress sound detected` alert |
| Heart rate | Place finger on MAX30102, wait for `[HR]` log lines | BPM printed every few seconds; alert only if outside 40–150 bpm |
| GPS | Power on outdoors, wait 1–2 min | `GPS Fix: YES` with coordinates in status log |

If the SOS SMS isn't arriving, check the SIM card has an active
plan with SMS credit, and confirm the GSM module's power supply
can handle its transmit current spikes (see `docs/WIRING.md` §1).

---

## 4. Tuning Thresholds

All thresholds live in `config.h`. Recommended approach for the
hackathon demo:

1. Wear the bracelet and walk/move normally for a minute —
   confirm no false "fall" alerts trigger.
2. If false positives occur, raise `IMPACT_THRESHOLD_G` slightly
   (e.g. from `2.5` to `3.0`).
3. For the mic, watch the raw ADC values printed in Serial during
   normal background noise vs. a loud clap/shout, and set
   `SOUND_THRESHOLD` roughly midway between the two.
