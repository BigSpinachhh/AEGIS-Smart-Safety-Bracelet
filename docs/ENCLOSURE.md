# Enclosure Specification — Aegis Safety Bracelet

This document records the CAD enclosure design parameters so the
model in `cad/` can be reproduced, modified, or re-derived if the
source files need to be regenerated.

---

## 1. Main Housing (PCB Compartment)

| Parameter | Value |
|---|---|
| External dimensions | 70 × 22 × 10 mm |
| Corner radius | R10 (all outer corners) |
| Wall thickness | 1.2 mm |
| Screw bosses | 4× corners, sized for **M1.4** self-tapping screws |

---

## 2. Strap Mounting

| Parameter | Value |
|---|---|
| Strap lug width | 20 mm |
| Strap type | Silicone band |
| Lug location | Both short ends of the housing |

---

## 3. Sensor Cutouts & Openings

| Feature | Dimensions | Location |
|---|---|---|
| MAX30102 optical window | 14 × 10 mm | Skin-contact (underside) face, aligned with sensor LEDs/photodiode |
| USB-A cutout (charging access) | 12 × 5 mm | Left short edge |
| SOS button dome | ⌀8 mm | Top face, positioned over SOS button footprint on PCB |

---

## 4. Assembly Notes

- The MAX30102 window must sit directly over the sensor's optical
  components with minimal air gap — even a thin clear window
  (e.g. a small acrylic insert) can reduce ambient light
  interference if the print isn't fully opaque.
- The 8 mm SOS button dome should be a slight press-fit or use a
  silicone boot over the tactile switch so it remains splash
  resistant while still giving tactile feedback.
- Screw bosses (M1.4) are sized for self-tapping into printed
  plastic — avoid over-torquing on first assembly to prevent
  stripping; pilot with the screw before final tightening.
- Leave clearance around the MAX9814 microphone for a small sound
  port if the enclosure fully encloses the PCB.

---

## 5. Folder Contents

| Path | Contents |
|---|---|
| `cad/source/` | Editable CAD source files (e.g. STEP / native CAD project) |
| `cad/renders/` | STL files for 3D printing + preview renders/images |

> Add your exported CAD files to the folders above. If you used
> generated reference drawings to model the enclosure, keep those
> reference images in `cad/source/` alongside the model for
> future revisions.
