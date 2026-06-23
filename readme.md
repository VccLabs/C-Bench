# C-Bench

**A mini bench power supply powered by USB-C Power Delivery.**

C-Bench turns any USB-C PD source (phone charger, laptop charger, or even a
phone) into an adjustable bench supply with live power monitoring and a 4"
touch UI. Open-source hardware and software, planned for launch on Crowd Supply.

- **Output:** up to **28 V @ 5 A (140 W)** — actual range depends on what the
  connected source can negotiate.
- **Host controller:** Raspberry Pi **RP2354A**.
- **PD sink:** **AP33772S** USB-C PD sink controller (supports fixed PDO, PPS, AVS).
- **Monitoring:** INA260 (V/I/P/energy), TMP102 (temperature), MAX17048 (battery
  fuel gauge) + MCP73831 Li-ion charger.
- **UI:** 4" 720×720 capacitive TDO HMI over UART (Giraffe IDE).

> Status: hardware defined; firmware bring-up in progress. Validated so far:
> PD profile read, PPS request + output arm, INA260 readback, live V/I/P
> telemetry to the HMI, HMI→RP output control, and full source-profile list
> transfer to the panel. This README is the single source of truth for the
> system architecture — keep it current.

---

## System overview

```
USB-C source ──► AP33772S (PD sink) ──► back-to-back MOSFET load switch ──► OUTPUT (28V/5A max)
                     │  I2C (5V, level-shifted)                                 │
USB-C debug  ──────► RP2354A (host) ◄── I2C ── INA260 (main output bus monitor) ┘
                     │  UART
                     └──► TDO 4" 720×720 HMI
```

The RP2354A is the master controller. The AP33772S negotiates power with the
source and switches the output; the RP supervises it over I2C and drives the
HMI. INA260 monitors the main output bus.

---

## Power architecture

### Input / rails
- **Source port (USB-C):** input from the PD source, **3.3 V – 28 V** depending
  on negotiation. Feeds **TI TPS552872RYQR** buck-boost → stable **5 V** (`Vout`).
- **Debug port (USB-C):** provides **5 V_USB**.
- Both `Vout` and `5V_USB` pass through a **510 mΩ / 1 W** impedance-matching
  resistor, then each through its own **ideal diode** to prevent back-feed if
  both ports are connected.
- Diode outputs join into the main **+5 V** rail.
- **AMS1117** LDO derives **+3.3 V** (RP2354A and 3.3 V logic) from +5 V.

Result: the user can connect either port, or both, safely. Power output
requires the **source port** connected to a real USB-C source.

### Output
Max **28 V / 5 A / 140 W** (source-dependent), exposed on multiple connectors:
- DC 2 mm barrel jack (+ / −)
- 2×2 right-angle 2.54 mm header (+ pair / − pair)
- 4 mm banana jacks (red = +, black = −)

---

## USB-C ports

| Port   | Connected to | Purpose                                                           |
| ------ | ------------ | ----------------------------------------------------------------- |
| Source | AP33772S     | PD negotiation with an external source; power input + output path |
| Debug  | RP2354A      | Programming / debugging; also supplies 5V_USB                     |

---

## RP2354A pin map

| GPIO | Dir | Net              | Notes                                                               |
| ---- | --- | ---------------- | ------------------------------------------------------------------- |
| IO2  | in  | INA260 `ALERT`   | pulled up to 3.3 V                                                  |
| IO3  | out | MAX17048 `QSTRT` | quick-start; pulled down to GND                                     |
| IO6  | in  | `CHG_STATE`      | charger STAT read-back (right-side level shift); pulled up to 3.3 V |
| IO7  | out | `CTL`            | charger STAT pull-direction control (left-side level shift)         |
| IO8  | out | HMI `RX`         | RP **TX** → HMI RX (UART)                                           |
| IO9  | in  | HMI `TX`         | RP **RX** ← HMI TX (UART)                                           |
| IO22 | in  | AP33772S `FLIP`  | via resistor divider (5 V → 3.3 V)                                  |
| IO25 | in  | AP33772S `INT`   | via resistor divider (5 V → 3.3 V)                                  |
| IO20 | —   | I2C `SDA`        | shared bus                                                          |
| IO21 | —   | I2C `SCL`        | shared bus                                                          |

> **Note:** MAX17048 `ALRT` is **not connected to the RP** — only pulled up to 5 V.

All RP GPIO (including the ones above) are broken out to the board edge on a
**2×20 right-angle 2.54 mm male header**.

---

## I2C devices

Single bus (5 V devices sit behind level shifters and appear as 3.3 V to the RP).
All addresses are distinct, so one bus is fine.

| Device        | Addr   | Role                                             | Logic | Level shift |
| ------------- | ------ | ------------------------------------------------ | ----- | ----------- |
| INA260AIPWR   | `0x40` | Voltage / current / power / energy on output bus | 3.3 V | No          |
| MAX17048G+T10 | `0x36` | Li-ion fuel gauge                                | —     | Yes         |
| TMP102AIDRLR  | `0x4B` | Temperature                                      | 3.3 V | No          |
| AP33772S      | `0x52` | USB-C PD sink controller                         | 5 V   | Yes         |

---

## AP33772S (PD sink)

- Negotiates with the source and exposes available **PDO / PPS / AVS** profiles.
- Controls **two back-to-back MOSFETs** as the output load switch.
- `FLIP` → IO22, `INT` → IO25 (both via dividers).
- `LED` pin → 0805 LED through 1 kΩ resistor.

---

## Battery / charging subsystem

- **MCP73831T-2ACI/OT** Li-ion charge controller. `PROG` pulled to GND via 2 kΩ.
- **MAX17048G+T10** fuel gauge (I2C `0x36`, level-shifted). `ALRT` pulled up to 5 V,
  not connected to the RP. `QSTRT` → IO3 (pulled down).

### Charger STAT level-shift / read circuit

The MCP73831 `STAT` pin is tri-state (driven LOW, driven HIGH ≈4.6 V, or High-Z).
Because the RP is 3.3 V-only, the 4.6 V HIGH is the hazard. A 3-MOSFET network
lets the MCU both **bias** the STAT node and **read** it safely.

**Left side — software-controlled pull on the 5 V domain (driven by `CTL`/IO7):**
- R42 (10 k) pulls STAT toward +5 V through Q5 (DMG230, P-ch) — pull-up enable.
- R46 (10 k) pulls STAT to GND through Q6 (BSS138, N-ch) — pull-down.
- `CTL` HIGH → Q6 ON (pull-down active), Q5 OFF (pull-up disconnected); and
  vice-versa.

**Right side — level-shifted read to 3.3 V (`CHG_STATE`/IO6):**
- Q7 (BSS138, N-ch), gate on the STAT node, R47 (10 k) pulls `CHG_STATE` to 3.3 V.
- STAT above Q7 Vgs(th) (~1.5–2 V) → Q7 ON → `CHG_STATE` = 0 V.
- STAT below threshold → Q7 OFF → `CHG_STATE` = 3.3 V.

**Truth table (node behavior):**

| STAT state | CTL              | Left node | Q7  | CHG_STATE |
| ---------- | ---------------- | --------- | --- | --------- |
| High-Z     | LOW (pull-up)    | ~5 V      | ON  | 0 V       |
| High-Z     | HIGH (pull-down) | ~0 V      | OFF | 3.3 V     |
| LOW (0 V)  | LOW              | 0 V       | OFF | 3.3 V     |
| LOW (0 V)  | HIGH             | 0 V       | OFF | 3.3 V     |
| HIGH (5 V) | LOW              | 5 V       | ON  | 0 V       |
| HIGH (5 V) | HIGH             | 5 V       | ON  | 0 V       |

**Charge-state decode — read `CHG_STATE` at both CTL polarities:**

| STAT   | CTL=LOW read | CTL=HIGH read | Pattern | Meaning                    |
| ------ | ------------ | ------------- | ------- | -------------------------- |
| High-Z | 0            | 1             | `01`    | High-Z (no battery / done) |
| LOW    | 1            | 1             | `11`    | Charging                   |
| HIGH   | 0            | 0             | `00`    | Charge complete            |

---

## Display / HMI

- **TDO 4" 720×720 capacitive touch**, developed in **Giraffe IDE**.
- Connected via FPC/FFC flex providing **GND + 5 V + UART** (RP RX=IO9, TX=IO8) at **115200 baud**.
- **XH-4A** connector exposes the same UART (+5 V, GND, TX, RX) so an alternative
  display (e.g. Nextion) can be used instead of the TDO panel.
- HMI role: scan and list source-advertised **PDO / PPS / AVS** profiles, let the
  user select one, then set target output voltage and limit max output current;
  live monitoring of V / I / P / energy / temperature / battery.

---

## HMI serial protocol

The RP↔HMI link uses the Giraffe register protocol (handled in
`GiraffeIDE/PD_V0.5/apps/hardware/uart/grf_hw_uart.c`). Frame format:

```
5A A5  LEN  CMD  ADDR_H ADDR_L  [DATA...]

│    │

│    └ 0x82 = write register(s), 0x83 = read register(s)

└ byte count of CMD + ADDR + DATA
```
Single 16-bit write example (voltage register 0x0010 = 9000 mV):
`5A A5 05 82 00 10 23 28`

On the panel, `grf_reg_set_user()` decodes incoming registers, formats the
value, and updates the matching label via `grf_label_set_txt()`.
`grf_reg_com_send()` provides the reverse path (panel → RP).

Note: the panel's `grf_reg_s_set` calls `grf_reg_set_user` **once per register**
(`datalen=1`, base addr each time). Multi-register payloads are therefore read
back from `ctrlreg[]` via `grf_reg_get()` on a "ready" trigger, not from the
callback's `data` pointer.

### Register map

**RP → HMI — telemetry** (pushed at 2 Hz):

| Reg      | Value          | Units |
| -------- | -------------- | ----- |
| `0x0010` | Output voltage | mV    |
| `0x0011` | Output current | mA    |
| `0x0012` | Output power   | 0.1 W |

**RP → HMI — profile list** (pushed periodically; re-reads source PDOs):

| Reg              | Value                                            | Units |
| ---------------- | ------------------------------------------------ | ----- |
| `0x0100`         | Profile count N (0 ⇒ no source; list cleared)    | —     |
| `0x0110 + i*4`   | Row i: `+0` type, `+1` vmin, `+2` vmax, `+3` imax | mV/mA |
| `0x0101`         | "List ready" trigger — panel renders on receipt  | —     |

Type codes: `0` FIX, `1` PPS, `2` AVS, `3` EPR. Voltage = `voltage_max ×
(EPR ? 200 : 100)` mV; vmin = vmax for fixed. Panel maps a selected **list
position** back to the real PDO (charger-agnostic — no reliance on PDO index).

**HMI → RP — control:**

| Reg      | Value                       | Units | Status  |
| -------- | --------------------------- | ----- | ------- |
| `0x0020` | Requested voltage (PPS/AVS) | mV    | planned |
| `0x0021` | Current limit (PPS/AVS)     | mA    | planned |
| `0x0022` | Output enable               | 0/1   | done    |
| `0x0023` | Selected profile position   | index | planned |

---

---

## HMI UI (Giraffe)

Apple-style dark UI, 720×720. Pages are Giraffe **views**, navigated with
`grf_view_set_dis_view_anim()`.

- **view1 — Monitor (boot):** hero voltage ring (`arc`, reg `0x0010`), V/I/P
  labels, output toggle (`imgbtn` → reg `0x0022`, alpha-crossfade animation),
  "Change" label → view2.
- **view2 — Profiles:** scrolling `container` with a fixed pool of **13 rows**
  (badge/voltage/meta/current/check/background labels each), filled from the
  profile-list registers and shown/hidden by count. Empty-state labels show when
  N = 0. Per-view control limit raised above the default 64.
- view3 — Battery, view4 — Settings: planned.


## Other

- **APS6404L-3SQR-SN** QSPI PSRAM footprint (unpopulated by default) to extend
  RP2354A RAM if needed.
- 4× **M3** mounting holes, one at each corner.

---

## Repository layout

```
C-Bench/
├── readme.md         ← this file
├── platformio.ini    ← PlatformIO project config (RP2354A firmware)
├── src/              ← firmware source (main.cpp)
├── include/  lib/  test/
├── GiraffeIDE/       ← TDO HMI project (Giraffe IDE)
└── LICENSE
```

## Toolchain

- Firmware: **PlatformIO** + **VS Code**, RP2354A target.
  - Board `rpipico2`, Arduino framework, **earlephilhower** core, monitor @115200.
  - Libraries (`lib_deps`): `CentyLab/AP33772S-CentyLab` (PD sink),
    `adafruit/Adafruit INA260 Library` (output-bus monitor).
- HMI: **Giraffe IDE** (TDO panel), UART @115200.
- **Optional SWD debug:** an RP2040 (e.g. Pico) flashed with `debugprobe` acts as a
  CMSIS-DAP probe (SWCLK→GP2, SWDIO→GP3, common GND; target powered separately).
  Set `upload_protocol = cmsis-dap` to flash over SWD and bypass USB-CDC upload
  hangs. Default flashing is over the USB debug port.
- Repo: `github.com/VccLabs/C-Bench`.

## License

Open-source hardware **and** software under the **MIT License**.

---

## TODO / open questions

- [x] Read & list source PDO/PPS/AVS profiles (AP33772S over I2C).
- [x] Request PPS voltage, arm output, read back on INA260 (closed loop validated).
- [x] Push live V/I/P telemetry to HMI labels (regs 0x0010–0x0012).
- [x] Reverse control path: HMI output enable → RP (reg 0x0022).
- [x] Source-profile list transfer RP → HMI (regs 0x0100/0x0110+/0x0101); auto-clear on unplug.
- [x] Profiles UI: 13-row scrolling pool, dynamic badges, real PDO data.
- [ ] Row selection → reg 0x0023, PPS/AVS fine adjust → regs 0x0020/0x0021.
- [ ] Apply selected profile on RP; return to Monitor armed.
- [ ] Battery (view3) and Settings (view4) pages.