[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
![Platform](https://img.shields.io/badge/MCU-RP2354A-informational)
![PD](https://img.shields.io/badge/USB--C-PD%203.1%20PPS%2FAVS-blue)
![Build with](https://img.shields.io/badge/PlatformIO-firmware-orange)
![Status](https://img.shields.io/badge/status-active-brightgreen)
![Stars](https://img.shields.io/github/stars/VccLabs/C-Bench)
![Last commit](https://img.shields.io/github/last-commit/VccLabs/C-Bench)
![Issues](https://img.shields.io/github/issues/VccLabs/C-Bench)
![Top language](https://img.shields.io/github/languages/top/VccLabs/C-Bench)

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
> telemetry, HMI→RP output control, full source-profile list transfer with
> live 13-row Profiles UI (auto-clears on unplug), row selection + apply, 4-page
> tab navigation, a Settings page (boot-output state + auto-arm) persisted to
> flash, and "Last used" restore of rail + voltage + output at boot. This README
> is the single source of truth — keep it current.

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
**2×20 right-angle 2.54 mm male header** on the **left edge** of the device (pins
face you when looking at that edge). Full breakout — Row 1 (top) / Row 2 (bottom),
pin-pairs 1→20:

| #   | Row 1 | note                    | Row 2 | note           |
| --- | ----- | ----------------------- | ----- | -------------- |
| 1   | GND   |                         | GND   |                |
| 2   | IO0   |                         | IO1   |                |
| 3   | IO2   | INA260 ALERT            | IO3   | MAX17048 QSTRT |
| 4   | IO4   |                         | IO5   |                |
| 5   | IO6   | STAT_3.3V               | IO7   | CTL            |
| 6   | IO8   | → HMI Rx                | IO9   | → HMI Tx       |
| 7   | IO10  |                         | IO11  |                |
| 8   | IO12  |                         | IO13  |                |
| 9   | IO14  |                         | IO15  |                |
| 10  | IO16  |                         | IO17  |                |
| 11  | IO18  |                         | IO19  |                |
| 12  | IO20  | I2C SDA                 | IO21  | I2C SCL        |
| 13  | IO22  | AP33772S FLIP           | IO23  |                |
| 14  | IO24  |                         | IO25  | AP33772S INT   |
| 15  | ADC1  |                         | ADC2  |                |
| 16  | ADC3  |                         | ADC2  |                |
| 17  | SWCLK |                         | SWDIO |                |
| 18  | GND   |                         | +3.3V |                |
| 19  | +5V   |                         | GND   |                |
| 20  | Vbus  | source USB port voltage | GND   |                |

An on-screen **Pin Map page** presents this so no labels need printing on the
enclosure (mockup: `HTML Mockups/cbench_pinmap_apple.html`).

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

### Battery presence detection (resolved — charging-only model)

The full-cell vs. no-cell case is physically unsolvable off +BATT: with no cell the
MCP73831 cycles the VBAT caps (C33/C34 ≈ 4.8 µF) — top-off → terminate → sag →
recharge — and the **termination plateau** sits at `00`, steady, ~4.2 V, identical
in level and steadiness to a real full cell. High-Z (`01`) never appears while VDD
is powered. No dynamics heuristic on +BATT can separate the two (3 attempts tried:
short dynamics window, asymmetric confirm debounce, disturbance-reset quiet-latch —
all false-positived into a phantom "96–99% Charged" on the 20–40 s plateau).

**Design decision:** don't distinguish them. Presence = **sustained charging only**:

- A real charging cell holds STAT `11` for minutes → latches present (`0x001E`=1).
- The no-cell recharge blip is ~1.4 µs (0.72 µC into 4.8 µF); at 2 Hz it is never
  caught and never sustained → never latches.
- `00` (complete) and `01` (High-Z) both map to **no battery**. Consequence: a real
  cell reverts to "no battery" the instant it tops off. This is intended UX.

`0x001E` now only ever emits `0` (none/full) or `1` (charging); `2` (complete) is
unused. No +BATT dynamics, no log-threshold tuning — reads the charger's own state.

The deterministic alternative (kept for a future hardware rev) needs a FET load +
+BATT→ADC divider for a transient decay probe (caps collapse ~1 V, real cell <5 mV),
which cleanly detects a floating full cell too. Deferred — no board change for now.

**SoC (unchanged, works):** MAX17048 ModelGauge unusable with CELL on +BATT
(`quickStart()` removed); SoC from `cellVoltage()` via piecewise Li-ion curve
(`socFromVoltage()`). Only presence changed.
    
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
(`datalen=1`, base addr each time). Multi-register payloads are read back from
`ctrlreg[]` via `grf_reg_get()` on the `0x0101` "ready" trigger — that handler
also caches each row into the panel's `g_prof[]` array, which the adjust panel
and apply logic depend on. (Do **not** rely on the `datalen>=4` branch in the
`0x0110` handler; with `datalen=1` it never fires.)

### Register map

**RP → HMI — telemetry / state** (pushed at 2 Hz; output state also on change):

| Reg      | Value                                                     | Units         |
| -------- | --------------------------------------------------------- | ------------- |
| `0x0010` | Output voltage                                            | mV            |
| `0x0011` | Output current                                            | mA            |
| `0x0012` | Output power                                              | 0.1 W         |
| `0x0013` | Session energy — **low 16 bits** of a 32-bit value        | mWh           |
| `0x0014` | Session energy — **high 16 bits**                         | mWh           |
| `0x0015` | Session charge (pushed; no panel label yet)               | mAh           |
| `0x0016` | Real output state (drives the view1 toggle)               | 0/1           |
| `0x0017` | Active list position for highlight; `0xFFFF` = none       | index         |
| `0x0018` | Session elapsed time                                      | s             |
| `0x0019` | Active profile type: 0 none, 1 Fixed, 2 PPS, 3 AVS, 4 EPR | enum          |
| `0x001A` | Active profile setpoint voltage                           | mV            |
| `0x001B` | Eased arc value (analog ring ramp) — see note             | 0–280 (0.1 V) |
| `0x001C` | Battery cell voltage (MAX17048); `0` = no battery         | mV            |
| `0x001D` | Battery SoC (MAX17048), clamped 0–100; `0xFFFF` = no batt | %             |
| `0x001E` | Charge state: 0 no battery/full, 1 charging (2 unused)    | enum          |
| `0x001F` | OCP fault flag → raises the trip popup on all views       | 0/1           |
| `0x0026` | Temperature (TMP102 @ 0x4B); `0xFFFF` = no sensor         | 0.1 °C        |
| `0x003A` | Lifetime energy odometer — **high 16 bits** of Wh         | Wh            |
| `0x003B` | Lifetime energy odometer — **low 16 bits** of Wh          | Wh            |

**32-bit values over a 16-bit bus:** session energy (`0x0013/0x0014`) and the
lifetime odometer (`0x003A/0x003B`) are 32-bit, split high/low across two
registers. Registers arrive **one at a time**, so the panel keeps a 32-bit shadow
(`g_sess_mWh`) and recombines on each half (`(hi<<16)|lo`), repainting on either.
Units were chosen to fit `u16` halves where single-register: session/charge use
mWh/mAh, the odometer uses whole Wh. Session energy is shown as `X.XXX` Wh (mWh
resolution) on `label3` (id 4); elapsed as `M:SS` / `H:MM:SS` on `label22` (id 26).

**Analog arc (`0x001B`):** the RP eases a displayed ring value toward the measured
voltage and pushes it at ~25 Hz **on change** (not 2 Hz), so the ring settles like a
needle while the numeric voltage (`0x0010`) stays instant. Panel keeps a `g_arc`
shadow and re-applies it on view1 entry (`view1_sync_armed`).

**RP → HMI — profile list** (resent only on change; empty pushed while no source):

| Reg            | Value                                              | Units |
| -------------- | -------------------------------------------------- | ----- |
| `0x0100`       | Profile count N (0 ⇒ no source; panel clears list) | —     |
| `0x0110 + i*4` | Row i: `+0` type, `+1` vmin, `+2` vmax, `+3` imax  | mV/mA |
| `0x0101`       | "List ready" trigger — panel renders on receipt    | —     |

Type codes: `0` FIX, `1` PPS, `2` AVS, `3` EPR. Voltage = `voltage_max ×
(EPR ? 200 : 100)` mV; vmin = vmax for fixed. The RP reads source PDOs directly
over I2C (`CMD_SRCPDO 0x20`) and normalizes them; the panel renders by **list
position** and never relies on PDO index — so any charger works.

PPS/AVS minimum voltage is decoded from the PDO `voltage_min` field (not
hardcoded): field `2` ⇒ floor 5000 mV (PPS) / 20000 mV (AVS); field `1` ⇒
3300/15000. Most consumer chargers report field `2`, i.e. a 5 V PPS floor.
Requested current is clamped to **≤ 4999 mA** before any request, because the
sink's current code maxes at 15 and `currentMap(5000)=16` is rejected — this is
what made 5 A rails (e.g. 20 V @ 5 A) silently stay on the previous contract.

**HMI → RP — control:**

| Reg      | Value                                            | Units   | Status |
| -------- | ------------------------------------------------ | ------- | ------ |
| `0x0020` | Requested voltage (PPS/AVS)                      | mV      | done   |
| `0x0021` | Current limit (PPS/AVS)                          | mA      | done   |
| `0x0022` | Output enable                                    | 0/1     | done   |
| `0x0023` | Selected profile position                        | index   | done   |
| `0x0024` | Refresh-list request                             | 1       | done   |
| `0x0025` | Session trip reset (zeros energy/charge/elapsed) | 1       | done   |
| `0x0030` | Display brightness                               | 10–100% | done   |
| `0x0031` | Boot output state (0 Off / 1 Last used)          | 0/1     | done   |
| `0x0032` | Auto-arm output after apply                      | 0/1     | done   |
| `0x0033` | Settings-sync request (view4 entry / HMI ready)  | 1       | done   |
| `0x0039` | Theme (0 dark / 1 light)                         | 0/1     | done   |

`0x0024` is sent by the panel on **every view2 entry** (`view2_reset_panel`); the
RP forces a fresh PDO re-read and re-pushes the list, so the list is always
current regardless of HMI boot timing or a late source attach. On **apply** the
panel sends `0x0020`/`0x0021` (range rails only), then `0x0023`; the RP latches
the adjust values, maps the list position to the real PDO, requests it, and —
**if auto-arm is enabled** (`0x0032`) — arms the output, then the panel animates
back to Monitor. The toggle colour follows the **real** output state pushed on
`0x0016`, not the apply event.

`0x0031`/`0x0032`/`0x0030`/`0x0039` are written when the user changes them in the
UI and are persisted to flash. `0x0033` is sent both on **view4 entry** and on
**HMI ready** (`view1_entry`, after the panel's ~7 s boot); the RP replies by
pushing the stored boot-state, auto-arm, **theme**, and lifetime odometer so the
panel reflects saved state. The RP also re-pushes boot-state, auto-arm,
**brightness**, and **theme** for the first ~12 s after boot to cover the slow HMI
bring-up — so brightness and theme are restored even before any view is opened.

**Session trip meter:** the RP integrates measured power/current over **real `dt`**
(`micros()`) into `µWh`/`µAh` accumulators while output is on, and pushes session
energy/charge/elapsed at 2 Hz (`0x0013/0x0014/0x0015/0x0018`). `0x0025` zeros the
session. The **lifetime odometer** accumulates always in `g_lifeE_uWh` (µWh) and
is persisted to **LittleFS** (`/life.bin`, full-precision µWh, wear-levelled):
committed every **20 s** when the value changed, plus on **output-off**. The
`0x0033` sync reply also re-pushes `0x003A/0x003B` so view4's entry-reset digit
labels repaint. Boot reads the file (migrates once
from old EEPROM `lifeCWh` if the file is absent). Sent to the HMI odometer as **Wh**
on `0x003A/0x003B` (shown as `XXXX.XXX` kWh). Requires **both** `board_upload.maximum_size
= 2097152` **and** `board_build.filesystem_size = 1m` in `platformio.ini`: the
`rpipico2` board declares 4 MB but the RP2354A has only **2 MB** internal flash, so
without the explicit max size the FS is placed past physical flash (3–4 MB) and all
writes fail silently — the odometer read back 0 after every power cycle until fixed.

**Active profile:** pushed at 2 Hz (`0x0019` type + `0x001A` setpoint mV) so the
view1 `label18` (id 22) reads e.g. `PPS 9.00 V` / `Fixed 20.00 V`, or `—` when
nothing is applied; survives output-off (driven by `g_activeSel`).

---

---

## HMI UI (Giraffe)

Apple-style dark UI, 720×720. Pages are Giraffe **views**, navigated with
`grf_view_set_dis_view_anim()`. Per-view control limit raised from 64 to 80.

- **view1 — Monitor (boot):** voltage ring (`arc` id 7, reg `0x0010`), V/I/P
  labels (`label0`/`label2`/`label4` = ids 1/3/5).
  - **Output toggle** is now a **label** (`label7`, id 11), not an imgbtn — green
    `#30D158` "Turn output on" / red `#ff453a` "Turn output off", styled via
    `grf_ctrl_style_set_bg_color` + `grf_label_set_txt_color`. It's driven by the
    **real** output state (`0x0016`): `view1_entry` and the `0x0016` handler both
    call `view1_set_output_btn()`. Tapping it sends `0x0022 = !g_out_on` (a label
    has no checked state, so the RP flips and pushes `0x0016` back to repaint).
  - **Session energy** `label3` (id 4, Wh `X.XXX`) + **elapsed** `label22`
    (id 26). **Reset button** `label13` (id 16) with a nested icon `image1`
    (id 21); a semi-transparent **press-tint overlay** `label21` (id 25) is shown
    on `GRF_EVENT_PRESSED` and hidden on `RELEASED`/`PRESS_LOST` (image colors
    can't be changed, so the overlay supplies the touch feedback); `CLICKED`
    sends `0x0025`.
  - **Active profile** `label18` (id 22), fed by `0x0019`/`0x001A`.
  - **Theme toggle** `image2` (id 31) → `view1_toggle_theme()`; see Theme.
  - **Boot gift-message popup:** `image4` (id 32) card + `label24` (id 33) text.
    `view1_entry` loads `D:/gift.txt` and shows the popup **once per boot**
    (`g_bootMsgShown` guard; hidden if the file is absent/empty). Tapping `image4`
    or `label24` dismisses it (`view1_hide_boot_msg`). Message is authored in view5.
    Gated on `g_giften` (view4 sw1) — popup skipped when disabled.
  - `view1_entry` also sends `0x0033` (HMI ready) and calls `view1_apply_theme()`.
- **view2 — Profiles:** scrolling `container0` holding a fixed pool of **13 rows**;
  each row = 6 controls (badge / voltage / meta / current / check / background),
  all **label** widgets. Rows are filled from the profile-list registers and
  shown/hidden by count. Badge is a colored chip (bg + text per type); empty-state
  labels show when N = 0.
- **Select:** tap a row background → ✓ + chip tint + a single `#FF9F0A`
    outline box (`selbox`, container id 84) repositioned over the row via
    `grf_ctrl_get_x/y/width/height` + `grf_ctrl_set_pos/size`.
- **Adjust panel** (`container1`, id 82): on selecting a PPS/AVS rail it shows
    two sliders (set-voltage → `0x0020`, current-limit → `0x0021`, ranges from
    the PDO), value labels, and a **Use** button (label id 90) that applies and
    returns to Monitor. Fixed rails hide the panel.
- **Active rail** is remembered (`g_applied`) and re-highlighted on return to
    view2; tapping it again behaves as a fresh selection.
- **Tap outside** the open panel just dismisses it (no select) so scrolling
    stays free.
- **Back label** (`label85`, id 91) returns to Monitor (`MOVE_RIGHT`).
 - The active-rail **highlight survives a boot restore**: the RP pushes the
    active position on `0x0017` just before the list on every view2 entry, so the
    restored rail highlights even though the panel didn't apply it itself.
- **view3 — Battery:** implemented as a **monitor** (MCP73831 is a fixed linear
  charger — nothing programmable). Elements:
  - SoC % `label6` (id 9) and cell voltage `label8` (id 11), fed by `0x001D` /
    `0x001C`; both show `-.-` on the no-battery sentinels.
  - Charge-state text `label0` (id 2) — "Charging" / "Charged" / "No battery" —
    and status dot `label1` (id 3): `TC_GREEN` when charging/charged, `TC_SURF2`
    when none. Both driven by `0x001E`. (Dot breathing animation must be an
    IDE-side animation — `grf_animation_set` is a no-op on this build.)
  - SoC arc `arc0` (id 15): SoC scaled 0–100 → 0–628 (full at 628) via
    `grf_arc_set_value`.
  - Nav labels: `label10` (id 13)→Monitor, `label11` (id 14)→Profiles,
    `label9` (id 12)→Settings, via `grf_view_set_dis_view_anim(..., ANIM_NONE)`.
  - **Theme toggle** `image2`/id 5 (macro `VIEW3_IMAGE1_ID`) → `view1_toggle_theme()`.
  - **Themed** via `theme_apply_view3()`, called on entry (`view3_entry` →
    `view3_apply_theme`) so control colors persist across navigation. Arc track
    (`TC_TRACK`) themed via `grf_arc_set_dis` part 0; green fg is constant.
    Charge-state pill `label0` (id 2) bg = `TC_SURF2`. Nav img `nav-battery.png` /
    `-light`. Presence = sustained charging only (see "Battery presence detection"
    above): a cell shows "Charging" until it tops off, then reverts to "No battery"
    by design. "Charged" state is intentionally never emitted.
- **view4 — Settings:** boot output state (segmented Off / Last used) and auto-arm
  output (switch), wired to `0x0031`/`0x0032`. Controls are painted from a
  panel-side shadow on entry (`view4_apply_settings`), kept in sync by RP pushes.
  - **Brightness** (done): `slider0` (id 19) 10–100% + `label16` (id 23) percent.
    `view4_set_bright()` applies the backlight **live** via `grf_disp_set_bright()`
    (panel maps `pct → pct*99/100`, range 0–99) and sends `0x0030`; the RP
    persists it (debounced) and echoes it back on boot/sync. A guard
    (`g_bright_guard`) suppresses the slider's `VALUE_CHANGED` echo when the value
    is set programmatically.
  - **Appearance selector (done):** in-Settings Dark/Light chooser mirroring the
    boot-state pattern. "Dark" `label26` (id 33) / "Light" `label27` (id 34) texts
    with chips `label24` (id 31) / `label25` (id 32) shown behind the active option;
    segment bg `label23` (id 30). `theme_state_paint()` colors the texts
    (`TC_TXT` selected / grey unselected) and shows/hides chips from `g_dark`.
    Handlers call `view4_set_theme(0|1)` → sets `g_dark`, `theme_apply()`, persists
    `0x0039`. Every view also has a **theme-toggle image** (`view1` id 31, `view2`
    id 99, `view3` id 5, `view4` id 35) → `view1_toggle_theme()`; all stay in sync
    via `theme_apply()`, and each swaps `theme-dark.png`/`theme-light.png`.
  - **view4 is now fully themed** (`theme_apply_view4`): screen bg, brand/`· Settings`,
    OUTPUT/DISPLAY section headers, boot-state card + segmented bg + chips + texts,
    separator, auto-arm switch **bg fill** (`grf_ctrl_style_set_bg_color(sw, TC_SURF2, 0)`
    — part MAIN=0; the `GRF_SW_PART_*` enum is **not** in this panel's headers, use the
    literal), display card, brightness % , slider track (`part 0`), brightness icons
    `image1`/`image2` (dark/light asset swap), nav `image0` swap, and the Appearance
    block above. Slider **borders** can't be set at runtime (no border setter shipped);
    they were zeroed in the IDE.
  - **Lifetime-energy odometer (done):** 7-digit `XXXX.XXX` kWh readout on Control
    IDs 42–49 (MSD→LSD: 49,48,47,45,44,43,42; id 46 = static decimal point), painted
    digit-by-digit in the `0x003B` handler via `grf_label_set_txt`. Re-pushed on the
    `0x0033` sync so it repaints after view4's entry reset. The lifetime card
    (label28 id37 / label41 id58) + gift card, plus their text (ids 38/39/41/51/59/
    62–66) and lock image `image6` (id 50, `lock-dark/-light.png`), are themed in
    `theme_apply_view4`; label40 (id 51) uses a bespoke light-mode grey `#C4C4C6`.
  - **Gift popup enable:** switch `sw1` (id 60, themed like sw0) toggles `g_giften`
    via `view4_set_giften()`, persisted **panel-local** to `D:/giften.bin` (mirrors
    `theme.bin`; no register/RP involvement). `giften_load_boot()` runs in `grf_main`
    before `grf_prj_create()`. **Defaults ON** (no file ⇒ `g_giften=1`). view1's boot
    popup is gated on it.
  - **Gift-message row** (`label49`, **Control ID 67**) navigates to **view5** (the
    message editor) via `grf_view_set_dis_view_anim(GRF_VIEW5_ID, …, ANIM_NONE)`.

- **view5 — Gift message editor:** a `txtbox0` (id 1) + `keyboard0` (id 2) pair
  (`grf_keyboard_set_txtbox`), a **Save** label (`label2`, id 5), a "Saved!" popup
  `image1`, and a **back** label (`label3`, **id 6**) → view4. On entry the saved
  message is loaded from `D:/gift.txt` into the txtbox (`grf_txtbox_get_text`/
  `grf_txtbox_set_text`); Save reads the txtbox and writes it to `D:/gift.txt`
  (`grf_fs_open(…, GRF_FS_MODE_WR)` → `grf_fs_write` → `grf_fs_close`), then shows the
  popup. HMI-side only — no RP round-trip.
  - **Fully themed** (`theme_apply_view5`, called on `view5_entry` via
    `view5_apply_theme`): screen bg, txtbox0 (id 1) surface+text, Save button
    (`label2` id 5, blue bg + white text), headers/labels (ids 3/4/6/7), and image
    swaps `saved-dark/-light.png` (id 9) + `theme-dark/-light.png`. Theme toggle
    `image0` (id 8) → `view1_toggle_theme()`.
- **view6 — Pin Map:** static reference page for the 2×20 left-edge GPIO breakout
  (the header table in "RP2354A pin map"). Fully themed (`theme_apply_view6`): logo
  (id 1) + theme-toggle (id 4) image swaps, brand/header/hint labels, six legend
  labels (GND/Power/I2C/UART/Special-fn/SWD → `TC_TXT2`), the **45 pin chips**
  (grouped: `TC_SURF2`-fill muted-text set; and a `TC_CHIP`-fill set — `#2C2C2E`
  dark / **white** in light — forced opaque via `grf_ctrl_style_set_bg_opa(c,255,0)`
  since IDE private-property transparency is overridden), and the 19 grid-line
  bars (ids 166–184, inline `#121212` dark / `#DCDCE1` light). **Back button** is a
  copied label (Control ID **162**, handler `label158_event`) → returns to the
  originating view via `g_prev_view` (see Pin-Map shortcut). ⚠ view6's IDE label
  names/IDs do **not** match the codegen enum (`view6.h`) — the enum value is the
  runtime Control ID; map through `view6.h`, and note runtime-set text only renders
  glyphs the control's font was compiled with (see gotchas).
- **view7 — About:** final page — static product / credits reference (like view6).
  Fully themed (`theme_apply_view7`): VccLabs logo (`image1`/id5) + theme-toggle
  (`image0`/id1 → `view1_toggle_theme()`) image swaps; text groups `V7_TXT`→`TC_TXT`
  and `V7_TXT2`→`TC_TXT2`; card surfaces `V7_SURF`→`TC_SURF` (pill + hardware/project
  panels, ids 10/16/38); hairline separators `V7_LINE`→`TC_SURF2` (ids 33–36, 49, 50);
  version label (id13) + **Back button** (`label0`/id2, `TC_SURF` bg + `TC_ORANGE` text)
  accented orange. Back goes **straight to Settings (view4)** — single origin, so no
  `g_prev_view`. The green pip (`label7`/id11) is left a fixed accent.
  **QR popup:** `image11`/id53 is a full-screen overlay, hidden by default. Three link
  labels — `label40`/id54 (Crowd Supply), `label41`/id55 (GitHub), `label42`/id56
  (Website) — call `view7_qr_show(1..3)`, swapping `qr-{crowd-supply,github,website}-{dark,light}.png`
  by `g_dark` and unhiding it; any tap dismisses via `image11_event`→`view7_qr_hide()`.
  A `g_v7_qr` shadow tracks which QR so `theme_apply_view7` re-swaps it if the theme is
  toggled while open. The overlay needs **touch mode = "can touch"** + an enlarged
  **extended-touch range** (both IDE properties) so a tap *anywhere* lands on it.

**About entry point:** Settings row `label61`/id80 → view7 (`view4_ui.c label61_event`).

**Pin-Map shortcut (per-page):** view1–view4 each carry an orange redirect button
(→ view6): `label27`/id34, `label90`/id100, `label3`/id17, `label60`/id79, plus the
Settings "Go to Pin Map" row `label53`/id71. Each is themed `TC_SURF` bg + `TC_ORANGE`
text and, before navigating, sets a global `g_prev_view` so view6's Back returns to
the right page. Visibility is gated by **view4 `sw2`** (id 76) → `view4_set_pinbtn()`,
persisted **panel-local** to `D:/pinbtn.bin` (mirrors `giften.bin`; `pinbtn_load_boot()`
runs in `grf_main`, **defaults ON**). Each view's `_entry` applies `g_pinbtn` via
`grf_ctrl_set_hidden`.

Row data lives in a `ROW_ID[13][6]` table in `grf_hw_uart.c` mapping each row's
six Control IDs; `fill_row()` / `show_row()` / `highlight_row()` / selection all
index through it.

**Navigation:** every page carries the same bottom tab bar — three touchable
labels per view jumping to the other three pages via
`grf_view_set_dis_view_anim(GRF_VIEWx_ID, GRF_SCR_LOAD_ANIM_NONE, …)`. Transitions
are **instant** (no slide). Handlers bind by numeric Control ID in `viewX_cc.h`,
which does **not** always match the IDE's label name — map by ID, not symbol.

### Over-current protection (OCP)

The AP33772S hardware OCP is armed on every profile apply — `setOCPTHR(limMA)` (50 mA/LSB)
for PPS/AVS, the PDO `imax` for fixed — with `OCP_EN` + `OCP_MSK`. The chip only *flags*
OCP (it doesn't open VOUT itself and doesn't reliably raise INT), so STATUS (`0x01`, bit 5)
is **polled** in `serviceStatus()`. On a trip the firmware opens the output
(`setOutput(0)`), latches off (`g_ocpLatched`), and pushes `0x0016=0` (arm button back to
green) + `0x001F=1`. The latch clears only on a manual re-arm or a fresh apply.

`serviceStatus()` handles **only** OCP — negotiation events are left to `sendProfileList()`
edge-tracking and the 500 ms watch, because acting on every re-negotiation caused a
spurious re-attach that reverted the rail on each profile select.

HMI: one popup image per view (all bound to `ocp-light.png` / `ocp-dark.png`), raised
together by `0x001F` and dismissed together on any tap; hidden by default and re-applied on
entry via the `g_ocp` shadow, so a trip persists across navigation and theme changes.

### Temperature (TMP102)

The TMP102 (`0x4B`, no config — powers up in 12-bit continuous mode) is read each telemetry
pass and pushed as tenths of a degree on reg `0x0026` (`0xFFFF` = no sensor). A top-bar
widget on all 7 views shows value + thermometer + unit, colored by status (green <45 °C,
orange 45–60 °C, red >60 °C — thresholds always in Celsius); the thermometer asset is
theme-aware (`temp-light` / `temp-dark`).

Tapping the value flips **°C ↔ °F** on every view at once (`temp_set_unit`), persisted to
`D:/tempunit.bin`. Settings (view4) adds a Temperature section: `sw3` shows/hides the widget
on all pages (persisted to `D:/tempshow.bin`, re-applied on entry) and a °C/°F segmented
toggle mirroring the theme toggle's selected-chip behavior.

### Theme color table

Dark/light theming lives panel-side in `grf_hw_uart.c` as a `THEME[role][col]`
table, indexed by role and by `g_dark` (which holds the `0x0039` reg value:
`0`=dark, `1`=light — so it doubles as the column index `0=dark | 1=light`).
Apply via `TCOL(role)` (raw color) or the `THEME_BG(ctrl, role)` / `THEME_TXT(ctrl,
role)` helpers (bg = part 0 fill, txt = label glyphs). The table lives near the top
of `grf_hw_uart.c` (above `use_btn_set`) so runtime callers can use `TCOL`.
Two non-fill cases: the **arc track** is a line, set with
`grf_arc_set_dis(arc, 0, {color,width,opa,rounded})` (part 0 = bg line, part 1 = fg,
width 27 / radius 12); **image assets** are theme-swapped with `grf_img_set_src`
(e.g. `nav-monitor.png` ↔ `nav-monitor-light.png`, `arrow-dark.png` ↔
`arrow-light.png`). Surfaces + text only — there is **no runtime border setter**
(see gotchas). Accents are iOS system colors that shift slightly per mode.

| Role        | Dark      | Light     | Used for                                 |
| ----------- | --------- | --------- | ---------------------------------------- |
| `TC_BG`     | `#000000` | `#F2F2F7` | screen / view background                 |
| `TC_SURF`   | `#1C1C1E` | `#FFFFFF` | cards / panels                           |
| `TC_SURF2`  | `#2C2C2E` | `#E5E5EA` | nested chips, slider track, segmented bg |
| `TC_TRACK`  | `#444446` | `#C7C7CC` | arc track (line stroke, not a fill)      |
| `TC_TXT`    | `#FFFFFF` | `#000000` | primary values / text                    |
| `TC_TXT2`   | `#8E8E93` | `#6C6C70` | units, captions, muted labels            |
| `TC_TXT3`   | `#474747` | `#C7C7CC` | empty-state / tertiary text              |
| `TC_GREEN`  | `#30D158` | `#34C759` | output-on / positive                     |
| `TC_RED`    | `#FF453A` | `#FF3B30` | output-off / alert                       |
| `TC_ORANGE` | `#FF9F0A` | `#FF9500` | selection / Use accent                   |
| `TC_BLUE`   | `#0A84FF` | `#007AFF` | info accent (spare)                      |
| `TC_CHIP`   | `#2C2C2E` | `#FFFFFF` | FIX badge (dark chip, white in light)    |

`theme_apply()` repaints from the shadow and dispatches to per-view
`theme_apply_viewN()` (wired into each `viewN_apply_theme` entry). **All six views
are fully themed** (view1 Monitor, view2 Profiles incl. the AVS/PPS adjust popup
`container1` id82 = `TC_SURF` + labels id85/86 = `TC_TXT2`, view3 Battery, view4
Settings incl. the pin-map section + `acc-orange`/`acc-black` image swaps, view5
editor, view6 Pin Map, view7 About). view1's boot gift popup is themed too (`image4` id32 asset
swap + `label24` id33 = `TC_TXT`). Selected-row tint uses `SEL_TINT` (orangy `#3A2A10`
dark / `#FFECD1` light) — note the ternary keys on `g_dark` where `1`=light. On
view2, `view2_paint_cards()` is the
single row-color authority — called on render (`0x0101`), entry, and toggle — and
covers: card bg (selection-aware, orange tint stays), per-row column text
(current = `#64D2FF`, check = `#FF9F0A`, both fixed; voltage = `TC_TXT`;
"adjustable rail" meta = `TC_TXT2`), and badges (PPS/AVS/EPR fixed accents, FIX =
`TC_CHIP`). The Use button (`BTN_USE`) hides when no source, shows a themed idle
"Select a rail", and turns orange when selected. Status line `label91` (`LBL_STAT`)
shows `"<W> W USB-C · <n> profiles"` or an empty-state prompt (`view2_apply_status`).

---

## Firmware behavior (RP `src/main.cpp`)

- **PDO list:** `sendProfileList()` reads `CMD_SRCPDO` (26 B) over I2C, normalizes
  each slot, and pushes only on change (signature compare). It also builds
  `g_slots[]` mapping **list position → real 1-based PDO index + type + ranges**.
  No-source pushes an empty list so the panel auto-clears.
- **Apply (`0x0023`):** sets `pendingSel`; the main loop maps it via `g_slots[]`
  and calls `setPPSPDO` / `setAVSPDO` (with latched `reqMV`/`limMA`) or
  `setFixPDO`. Output is armed **only if auto-arm is on** (`g_set.autoArm`).
  `activePdoIdx`/`activeType` track the armed rail and `g_activeSel` the list
  position (for the `0x0017` highlight); the PPS/AVS keep-alive refreshes **that**
  rail only (fixed rails need no keep-alive).
- **Output toggle (`0x0022`):** acts whenever the HMI changes it; every change is
  also pushed back on `0x0016` so the panel toggle stays truthful.
- **Source attach:** the AP33772S **`INT`** (IO25, active-HIGH, push-pull ~4.85 V
  via 0.656 divider → ~3.18 V) drives an ISR; on the flag the RP reads **STATUS
  `0x01`** (auto-clears) and on `STARTED|READY|NEWPDO` re-asserts output + refreshes
  the PDO list. A slow **500 ms** I2C poll remains as a detach/missed-edge backstop.
  Re-assert (`g_prevSource` → `g_outAttach`) sets output state (default OFF) and
  calls **`usbpd.begin()` to refresh the library's PDO array** — required so a
  source plugged in *after* boot can actually be requested (the library only
  reads PDOs in `begin()`; a stale array makes `setFixPDO/PPS` silently no-op).
  A brief 5 V blip on attach is unavoidable (the sink brings up its default
  contract before firmware can react).
- **Settings persistence:** a small `Settings` struct (magic **`0xCB05`**) lives in
  flash via the earlephilhower **EEPROM** emulation — `loadSettings()` at boot,
  `saveSettings()` on each change (guarded so only real changes write). Fields:
  boot-output state, auto-arm, last output on/off, last list position, last mV/mA,
  **brightness** (`0x0030`), **theme** (`0x0039`). (The **lifetime odometer** moved
  out of EEPROM to LittleFS `/life.bin`; `lifeCWh` remains only as a one-time
  migration source.) Changing the layout bumps the magic and resets settings
  once. (History: `0xCB02` original → … → `0xCB05` adds theme.)
- **Energy metering:** in the 2 Hz telemetry block the RP reads INA260 V/I/P and
  integrates into session + lifetime accumulators (`energyAccumulate`). **Reads are
  validated first:** negatives are floored to 0, and implausible values
  (`> 160000 mW` / `> 6000 mA` — above the supply's ~140 W / 5 A ceiling) mark the
  sample bad so it is **not** integrated (the INA260 power register tops out at
  655 W on a glitch, and a bad `µWh` add runs the odometer away). `dt > 2 s` is
  also rejected. A bad/off sample freezes the interval; the next good sample
  restarts cleanly at `dt = 0`. **The INA260 has no on-chip energy/charge
  accumulator** (INA228/229 do) — integration is firmware-side by necessity.
- **Theme (dark/light):** stored as `g_set.theme`, persisted, echoed on
  boot/sync (`0x0039`). Panel keeps a `g_dark` shadow; `theme_apply()` repaints
  themed controls (currently a TEST set on view1: `label16`/`label17`/`label1` =
  ids 19/20/2) via `grf_ctrl_style_set_bg_color(..., 0)`. Applied on `0x0039` RX,
  on user toggle (`view1_toggle_theme`, which also sends `0x0039`), and on view
  entry (`view1_apply_theme`). **NOTE:** the visible `#1C1C1E` on these "cards" is
  their **background fill**, not text — theming surfaces uses `set_bg_color`, not
  `set_txt_color`.
- **Boot "Last used" restore:** if boot-output state = Last used, after the PDO
  list is known the RP re-applies the saved rail (`lastSel`) at the saved voltage
  (`lastMV`/`lastMA`) and forces the saved output state — fired only once the
  initial source-attach is consumed, so the refreshed PDO array is used and the
  contract sticks. Restore is by **list position**; a different charger at next
  boot can map that position to a different rail.
- **Boot window:** the list is force-re-pushed for the first seconds and the
  settings for ~12 s, so a slow-booting HMI (~7 s) still gets both; `0x0024`
  (list) and `0x0033` (settings, on HMI ready) make this robust thereafter.

---

## Giraffe IDE constraints / gotchas (learned)

- **One style per control** — no per-state (Checked/Pressed) styles. "Selected"
  styling is done in firmware; the selection border is a separate `selbox`
  container moved over the active row (there is **no runtime border setter**).
- **`grf_animation_set` is effectively non-functional** here (slide-up panel was
  abandoned; the adjust panel uses instant `grf_ctrl_set_hidden`). By extension the
  view3 status-dot "breathing" must be an IDE-side animation, not firmware.
- **No border setters ship in this panel's headers.** Both
  `grf_ctrl_style_set_border_color` and `..._width` are in the manual but **not
  declared** in the SDK headers (compile error / implicit-decl). Set borders in the
  IDE (e.g. slider borders zeroed there) — they cannot follow the theme at runtime.
- **`GRF_SW_PART_*` / `GRF_BAR_PART_*` enums are not in the headers** either. Use the
  literal `part` values: switch/slider bg fill = `grf_ctrl_style_set_bg_color(ctrl,
  color, 0)` (part MAIN = 0).
- **Forward-declare + define order:** functions/vars used by an earlier function
  (e.g. `theme_apply_view4` calling `boot_state_paint` / `theme_state_paint`, or
  referencing `g_v4_boot`) need a forward decl / hoisted definition above the caller,
  or you hit implicit-declaration / undefined-reference errors.
- **Rolling Transfer** (scroll-chain to parent) must be enabled on the row
  background controls for the list to scroll while rows are tappable — and it
  **resets when those controls are edited in the IDE**, so re-check after IDE
  changes. No code API for it.
- **imgbtn has its own state machine** (`GRF_IMGBTN_STATE_*`), separate from
  `GRF_STATE_CHECKED`. To show the checked image programmatically use
  `grf_imgbtn_set_mode(..., GRF_IMGBTN_STATE_CHECKED_RELEASED)`, and apply it in
  `view1_entry` (post-load) — setting it before navigation gets wiped by the load.
- **Background "cards" are label widgets**; an unset label renders the IDE default
  "Text". Set such labels' **text opacity to 0** (no firmware needed) or give them
  empty text.
- The checkmark label text is set in `fill_row()` (`✓`) so all rows show a tick.
- **Views reset their controls on entry.** A value pushed while a view isn't
  current (or set just before navigating in) is wiped on load, so reflectable
  state (output toggle, Settings controls, active-rail highlight) is kept in a
  firmware-side **shadow** and re-applied in the view's `_entry`. Live updates
  while a view *is* current are guarded with `grf_view_get_cur_id(GRF_LAYER_UI)`.
- **HMI boots slowly (~7 s); the RP boots fast.** Anything pushed in the first
  seconds is lost. Drive reflect from a re-push window that outlasts HMI boot, or
  better, from an "HMI ready" request the panel sends in `view1_entry` (`0x0033`).
- **Handlers bind by numeric Control ID** in `viewX_cc.h`, not by label name —
  IDE names and IDs diverge (a view's `label2` symbol ≠ Control ID 2). Wire by ID.
- **`grf_sw_set_state()` fires no `VALUE_CHANGED`** — use it to reflect a switch
  from stored state without echoing back to the RP.
- **`grf_slider_set_value()` has no such "no-event" guarantee** — assume a
  programmatic set *can* fire `VALUE_CHANGED`, so wrap programmatic sets in a guard
  flag (e.g. `g_bright_guard`) to avoid echoing back to the RP.
- **A label's visible color is its background fill, not its text.** To theme/recolor
  a "card" use `grf_ctrl_style_set_bg_color(ctrl, color, 0)` (part 0); use
  `grf_label_set_txt_color()` only for the glyphs. Both take effect at runtime with
  no explicit refresh. A label used as a button needs a solid fill + radius +
  **clickable** set in the IDE, and any nested image must be **non-clickable** or it
  eats the touch.
- **`grf_disp_set_bright(u8)` takes 0–99** (99 brightest) — map a percent slider as
  `pct*99/100`. It's a global backlight call (no view guard needed).
- **Registers are 16-bit.** For 32-bit quantities (energy, odometer) split high/low
  across two registers; they arrive one at a time, so reassemble via a panel shadow.
- **Entry repaint kills the default-text flash.** On view load a control briefly
  renders its IDE-authored default (e.g. `88.88`, or `"Text"`) until the next 2 Hz
  push (≤500 ms) — a visible flash. Fix: shadow the last value for **every** dynamic
  control and repaint in `_entry`, not just state controls. Implemented as
  `view1_tele_apply` (V/I/P/energy/elapsed/profile), `view2_render_list` (rows from
  cached `g_prof[]`), `view3_tele_apply` (SoC/cell/state), and view4's
  `odo_paint(g_lifeWh)`. Telemetry handlers now also store their raw shadow.
- **Sliders don't repaint on `set_value` alone** — the knob shows the IDE default (0)
  for a frame on entry. Call `grf_ctrl_force_refresh(ctrl)` right after
  `grf_slider_set_value` (see `bright_slider`) to redraw immediately.
- **INA260 reads can be negative (near no-load) or railed (655 W on glitch).** Casting
  a negative float to unsigned yields garbage. Validate/clamp before display **and**
  before integrating, or an energy integrator runs away.
- **LittleFS on RP2354A needs two `platformio.ini` keys:** `board_build.filesystem_size
  = 1m` **and** `board_upload.maximum_size = 2097152`. The default filesystem_size is
  0 bytes (mount fails); and because `rpipico2` declares 4 MB while the chip has 2 MB,
  omitting the max size puts the FS past physical flash so writes fail silently. The
  odometer commits every 20 s (on change) + on output-off; values between commits live
  only in RAM and are lost on a bare power cut.
- **Panel-side persistence uses the file system API** (`grf_fs_open/read/write`,
  `D:/…`), not the manual's `grf_flash_*_Data` (those are **not linked** in this
  build). Theme is stored in `D:/theme.bin`.
- **Load persisted theme before `grf_prj_create()`** (in `grf_main`, not
  `grf_hw_init`) or the first view paints in the default theme for ~1 s then flips.
- **The TDO boot logo is a static PNG** set via **Tools → Set boot logo** (PNG,
  ≤768 KB, ≤screen res). No runtime API — it can't follow the theme; pick one.
- **Runtime-set text renders as white boxes (tofu) if the glyphs weren't compiled.**
  Giraffe subsets clipped TTF fonts to only the characters used by static IDE text.
  `grf_label_set_txt(ctrl, "Back")` boxes if that control's font never included
  `B/a/c/k`. Fix in the IDE: set the label's **static text** to include those chars
  and regenerate the font (Tools → Font Tools → scan → generate), or drop the custom
  font so it falls back to the built-in 16×16 English library.
- **Image controls aren't touchable by default.** An `image` fires no `CLICKED`
  until its IDE **touch mode** is set to "can touch"; to have it swallow taps beyond
  its bounds (e.g. a modal overlay dismissed by tapping anywhere), raise its
  **extended-touch range** and keep it top-most in z-order (view7 QR popup).
- **view6 IDE label names/IDs diverge from the codegen enum.** After deleting/adding
- **view6 IDE label names/IDs diverge from the codegen enum.** After deleting/adding
  controls, the IDE's displayed Control ID may not exist in `view6.h` (e.g. the "back"
  control the IDE calls id 164 had no enum; the bound control was id 162). Trust the
  `viewN.h` enum + `viewN_cc.h` binding table, not the IDE ID; an unbound handler is
  dead code even if it looks wired.

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
- [x] Row selection → reg 0x0023, PPS/AVS fine adjust → regs 0x0020/0x0021.
- [x] Apply selected profile on RP; return to Monitor armed; sync toggle to ON.
- [x] Adjust panel (sliders + Use), selection border, active-rail re-highlight.
- [x] Robustness: fresh list on view2 entry (0x0024), late-attach PDO refresh,
      current clamp ≤4999 mA, real PPS/AVS voltage_min floor, output re-assert.
- [x] 4-page tab navigation (instant); view2 back button.
- [x] Settings (view4): boot-output state + auto-arm, reflected on entry.
- [x] Persist settings to flash; "Last used" restores rail + voltage only — **output is never armed on boot** (safety).
- [x] Active-rail highlight after boot restore (reg 0x0017).
- [x] Settings: persistent **brightness** slider (reg 0x0030, RP-backed, debounced).
- [x] Monitor: output toggle as a colored **label** (green/red, reg 0x0022).
- [x] Monitor: **session energy** trip meter — energy `X.XXX` Wh (0x0013/0x0014,
      32-bit mWh), elapsed (0x0018), reset button + press-tint (0x0025). Charge
      (0x0015) pushed but intentionally **not** shown.
- [x] **Lifetime energy** accumulation + persistence (0x003A/0x003B). *(display TBD)*
- [x] Monitor: **active profile** label (0x0019 type + 0x001A mV).
- [x] INA read validation + `dt` cap so the energy integrator can't run away.
- [x] **Theme persistence** plumbing (reg 0x0039, RP-backed, re-applied on entry).
- [x] **Dark/light theme** extended across view1 + view2 (roles, image swaps, cards).
- [x] **Analog arc easing** (RP ramps toward measured V, reg 0x001B ~25 Hz on change).
- [x] Extend theme to **view3 / view4** and any remaining view2 elements.
- [x] Lifetime-energy **odometer display** in Settings (7-digit, Control IDs 42–49).
- [x] **Gift message**: view5 editor → `D:/gift.txt`; view1 boot popup (image4/label24).
- [x] **Gift popup enable** (view4 sw1 → `D:/giften.bin`, panel-local, defaults ON).
- [x] **Theme extended** to view4 lifetime/gift cards + **view5** (fully themed + toggle).
- [x] **Pin Map page (view6)** — full themed GPIO-breakout reference (chips, grid,
      legend); Back returns to originating view via `g_prev_view`.
- [x] **Per-page Pin-Map shortcut buttons** (view1–4 + Settings row), gated by
      view4 `sw2`, persisted panel-local to `D:/pinbtn.bin` (defaults ON).
- [x] **Anti-flash entry repaint** across view1–4 (telemetry/rows/odometer from
      shadow; slider `force_refresh`). Session energy now shows the `Wh` unit.
- [x] Extend theme to **view3 / view4 / view5 / view6** and remaining view2 elements.
- [x] Battery page (view3) content.
- [x] **About page (view7)** — themed product/credits reference; theme-aware QR
      popup overlay (Crowd Supply / GitHub / Website); Back → Settings; reached
      from Settings row `label61`/id80.
- [x] **Battery presence — resolved (charging-only model).** Heuristics off +BATT can't
      tell a full cell from the no-cell charger plateau, so presence = sustained charging
      (STAT `11`); full/none both read "no battery". See "Battery presence detection".
- [x] **Over-current protection (OCP).** AP33772S hardware OCP armed at the set current
      limit on every apply; STATUS bit5 polled (chip only flags) → firmware cuts VOUT,
      latches off until re-arm, raises a themed trip popup on all 7 views (reg `0x001F`,
      tap to dismiss). See "Over-current protection".
- [x] **Temperature widget (TMP102 @ 0x4B).** Reading pushed as reg `0x0026` (0.1 °C);
      shown on all 7 views with green/orange/red status color; tap to toggle °C/°F
      (persisted); Settings `sw3` shows/hides the widget (persisted). See "Temperature".
- [ ] Slide-up animation for the adjust panel (blocked: `grf_animation_set` no-op).