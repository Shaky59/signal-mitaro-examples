# Signal Mitaro (入力信号トランスファーボード)

Industrial PNP signal acquisition platform  
→ Structured event generation  
→ Multi-path communication (Wi-Fi / Bluetooth / CAN / Satellite)

Signal Mitaro is a robust industrial input board concept designed to convert physical PNP signals into structured digital events suitable for local and global transmission.

This repository documents the early design and reference implementations.

---

## Concept Illustration

<h3>French Version</h3>
<p align="center">
  <img src="docs/images/docsimagesmitaro_board_fr.png" width="400">
</p>

<h3>Japanese Version</h3>
<p align="center">
  <img src="docs/images/docsimagesmitaro_board_ja.png" width="400">
</p>

---

## Repository Structure

signal-mitaro-examples/
├─ README.md
├─ docs/images/
├─ schemas/
---

## Event Specification

Event format is defined in:

`schemas/event_v1.json`

This schema represents the structured output generated from physical input signals.

---

## Examples

- Minimal ESP32 GPIO event logger  
  `examples/esp32/gpio_logger/`

More communication examples (CAN / Wi-Fi / Satellite) will be added progressively.

---

## Status

Work in progress.  
Minimal ESP32 example available.  
Hardware under design.
│ └─ event_v1.json
└─ examples/
└─ esp32/
└─ gpio_logger/

