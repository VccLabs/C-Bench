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
(`datalen=1`, base addr each time). Multi-register payloads are read back from
`ctrlreg[]` via `grf_reg_get()` on the `0x0101` "ready" trigger — that handler
also caches each row into the panel's `g_prof[]` array, which the adjust panel
and apply logic depend on. (Do **not** rely on the `datalen>=4` branch in the
`0x0110` handler; with `datalen=1` it never fires.)

### Register map

**RP → HMI — telemetry / state** (pushed at 2 Hz; output state also on change):

| Reg      | Value                                               | Units    |
| -------- | --------------------------------------------------- | -------- |
| `0x0010` | Output voltage                                      | mV       |
| `0x0011` | Output current                                      | mA       |
| `0x0012` | Output power                                        | 0.1 W    |
| `0x0013` | Session energy — **low 16 bits** of a 32-bit value  | mWh      |
| `0x0014` | Session energy — **high 16 bits**                   | mWh      |
| `0x0015` | Session charge (pushed; no panel label yet)         | mAh      |
| `0x0016` | Real output state (drives the view1 toggle)         | 0/1      |
| `0x0017` | Active list position for highlight; `0xFFFF` = none | index    |
| `0x0018` | Session elapsed time                                | s        |
| `0x0019` | Active profile type: 0 none, 1 Fixed, 2 PPS, 3 AVS, 4 EPR | enum |
| `0x001A` | Active profile setpoint voltage                     | mV       |
| `0x003A` | Lifetime energy odometer — **high 16 bits** of cWh  | 0.01 Wh  |
| `0x003B` | Lifetime energy odometer — **low 16 bits** of cWh   | 0.01 Wh  |

**32-bit values over a 16-bit bus:** session energy (`0x0013/0x0014`) and the
lifetime odometer (`0x003A/0x003B`) are 32-bit, split high/low across two
registers. Registers arrive **one at a time**, so the panel keeps a 32-bit shadow
(`g_sess_mWh`) and recombines on each half (`(hi<<16)|lo`), repainting on either.
Units were chosen to fit `u16` halves where single-register: session/charge use
mWh/mAh, the odometer uses cWh. Session energy is shown as `X.XXX` Wh (mWh
resolution) on `label3` (id 4); elapsed as `M:SS` / `H:MM:SS` on `label22` (id 26).

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

| Reg      | Value                                           | Units | Status |
| -------- | ----------------------------------------------- | ----- | ------ |
| `0x0020` | Requested voltage (PPS/AVS)                     | mV    | done   |
| `0x0021` | Current limit (PPS/AVS)                         | mA    | done   |
| `0x0022` | Output enable                                   | 0/1   | done   |
| `0x0023` | Selected profile position                       | index | done   |
| `0x0024` | Refresh-list request                            | 1     | done   |
| `0x0025` | Session trip reset (zeros energy/charge/elapsed)| 1     | done   |
| `0x0030` | Display brightness                              | 10–100% | done |
| `0x0031` | Boot output state (0 Off / 1 Last used)         | 0/1   | done   |
| `0x0032` | Auto-arm output after apply                     | 0/1   | done   |
| `0x0033` | Settings-sync request (view4 entry / HMI ready) | 1     | done   |
| `0x0039` | Theme (0 dark / 1 light)                        | 0/1   | done   |

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
session. The **lifetime odometer** accumulates always, is persisted (debounced:
every ~60 s while running + on output-off) and echoed on `0x0033`/boot window.

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
  - **Theme toggle (TEST)** `label24` (id 28) → `view1_toggle_theme()`; see Theme.
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
- **view3 — Battery:** scaffolded; content planned.
- **view4 — Settings:** boot output state (segmented Off / Last used) and auto-arm
  output (switch), wired to `0x0031`/`0x0032`. Controls are painted from a
  panel-side shadow on entry (`view4_apply_settings`), kept in sync by RP pushes.
  - **Brightness** (done): `slider0` (id 19) 10–100% + `label16` (id 23) percent.
    `view4_set_bright()` applies the backlight **live** via `grf_disp_set_bright()`
    (panel maps `pct → pct*99/100`, range 0–99) and sends `0x0030`; the RP
    persists it (debounced) and echoes it back on boot/sync. A guard
    (`g_bright_guard`) suppresses the slider's `VALUE_CHANGED` echo when the value
    is set programmatically.
  - Appearance (theme) currently lives as a TEST toggle on view1; lifetime-energy
    odometer is pushed (`0x003A/0x003B`) but **not yet displayed** here.

Row data lives in a `ROW_ID[13][6]` table in `grf_hw_uart.c` mapping each row's
six Control IDs; `fill_row()` / `show_row()` / `highlight_row()` / selection all
index through it.

**Navigation:** every page carries the same bottom tab bar — three touchable
labels per view jumping to the other three pages via
`grf_view_set_dis_view_anim(GRF_VIEWx_ID, GRF_SCR_LOAD_ANIM_NONE, …)`. Transitions
are **instant** (no slide). Handlers bind by numeric Control ID in `viewX_cc.h`,
which does **not** always match the IDE's label name — map by ID, not symbol.

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
- **Source attach:** a rising-edge detector (`g_prevSource` → `g_outAttach`, plus
  a fast ~150 ms I2C presence poll) re-asserts the output state (default OFF) and
  calls **`usbpd.begin()` to refresh the library's PDO array** — required so a
  source plugged in *after* boot can actually be requested (the library only
  reads PDOs in `begin()`; a stale array makes `setFixPDO/PPS` silently no-op).
  A brief 5 V blip on attach is unavoidable (the sink brings up its default
  contract before firmware can react).
- **Settings persistence:** a small `Settings` struct (magic **`0xCB05`**) lives in
  flash via the earlephilhower **EEPROM** emulation — `loadSettings()` at boot,
  `saveSettings()` on each change (guarded so only real changes write). Fields:
  boot-output state, auto-arm, last output on/off, last list position, last mV/mA,
  **brightness** (`0x0030`), **lifetime odometer `lifeCWh`** (`0x003A/0x003B`),
  **theme** (`0x0039`). Changing the layout bumps the magic and resets settings
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
  abandoned; the adjust panel uses instant `grf_ctrl_set_hidden`).
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
- **Continuous 2 Hz pushes self-heal the view-entry control reset** — telemetry-style
  labels (V/I/P, session energy, elapsed, active profile) need no shadow because the
  next push (≤500 ms) repaints them. Only *state* controls (toggles, settings,
  theme) need a shadow + `_entry` re-apply.
- **INA260 reads can be negative (near no-load) or railed (655 W on glitch).** Casting
  a negative float to unsigned yields garbage. Validate/clamp before display **and**
  before integrating, or an energy integrator runs away.

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
- [x] Persist settings to flash; "Last used" restore of rail + voltage + output.
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
- [~] **Dark/light theme — IN PROGRESS.** Proven on a TEST set (view1 `label16`/
      `label17`/`label1`, ids 19/20/2) + toggle `label24` (id 28). **Next: extend to
      all objects across all 4 views.** See the "Theme" notes in Firmware behavior.
- [ ] Settings UI for theme (move the toggle off view1 into view4 appearance).
- [ ] Lifetime-energy **odometer display** in Settings (data already on 0x003A/3B).
- [ ] Battery page (view3) content.
- [ ] Slide-up animation for the adjust panel (blocked: `grf_animation_set` no-op).