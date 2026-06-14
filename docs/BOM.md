# Bill of Materials — Aegis Safety Bracelet

Approximate costs in INR, based on typical Indian e-commerce /
electronics-market pricing. Update with your actual purchase
prices once finalized.

| # | Component | Function | Qty | Approx. Unit Cost | Approx. Subtotal |
|---|---|---|---|---|---|
| 1 | ESP32 Dev Module | Main controller | 1 | ₹350 – ₹450 | ₹400 |
| 2 | NEO-6M GPS Module | Location tracking | 1 | ₹350 – ₹500 | ₹450 |
| 3 | SIM800L **or** A7670C module | Cellular SOS (SMS/call) | 1 | ₹600–₹800 (SIM800L) / ₹1100–₹1400 (A7670C, 4G) | ₹700 |
| 4 | MPU6050 | Fall / impact detection | 1 | ₹80 – ₹150 | ₹100 |
| 5 | MAX9814 mic module | Distress sound detection | 1 | ₹100 – ₹200 | ₹150 |
| 6 | MAX30102 | Heart-rate monitoring | 1 | ₹150 – ₹300 | ₹200 |
| 7 | TP4056 charging module | Li-ion charge controller | 1 | ₹25 – ₹50 | ₹40 |
| 8 | 3.7V Li-ion battery (500–1000mAh) | Power source | 1 | ₹150 – ₹350 | ₹250 |
| 9 | SOS push button (8mm dome) | Manual trigger | 1 | ₹10 – ₹20 | ₹15 |
| 10 | Resistors (voltage divider, pull-ups) | Level shifting | assorted | ₹1 – ₹2 each | ₹10 |
| 11 | Decoupling capacitors | Power stability | assorted | ₹2 – ₹5 each | ₹20 |
| 12 | Status LED + buzzer | Local alert | 1 each | ₹5 – ₹15 | ₹15 |
| 13 | Custom PCB (per `hardware/pcb/gerber`) | Compact integration | 1 | ₹150 – ₹300 (small-batch) | ₹250 |
| 14 | 3D-printed enclosure (per `cad/`) | Wearable housing | 1 | ₹100 – ₹250 | ₹150 |
| 15 | 20mm silicone strap | Wearability | 1 | ₹50 – ₹100 | ₹75 |
| 16 | Jumper wires / connecting wire | Prototyping | — | — | ₹50 |
| | | | | **Total (approx.)** | **₹2,300 – ₹3,000** |

> Costs vary with supplier, GST, and shipping. SIM800L (2G) is
> cheaper but is being phased out on some Indian carriers — A7670C
> (4G LTE) costs more but has better long-term network coverage.

---

## Per-Sensor Justification (for judges / report)

| Sensor | Role in the safety pipeline |
|---|---|
| MPU6050 | Detects falls, sudden impacts, or violent shaking of the wrist — triggers SOS automatically even if the wearer can't reach the button |
| MAX9814 | Listens for sudden loud sounds (screaming, shouting) as a secondary distress signal |
| MAX30102 | Tracks heart rate; a sudden spike or drop outside a safe range can indicate panic, struggle, or a medical emergency |
| NEO-6M | Provides the GPS coordinates sent in every SOS message, so responders know exactly where to go |
| SIM800L / A7670C | Sends the SOS over the **cellular network** — works even when the wearer has no smartphone, Wi-Fi, or mobile data |
