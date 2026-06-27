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

| Reg      | Value                                               | Units |
| -------- | --------------------------------------------------- | ----- |
| `0x0010` | Output voltage                                      | mV    |
| `0x0011` | Output current                                      | mA    |
| `0x0012` | Output power                                        | 0.1 W |
| `0x0016` | Real output state (drives the view1 toggle)         | 0/1   |
| `0x0017` | Active list position for highlight; `0xFFFF` = none | index |

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
| `0x0031` | Boot output state (0 Off / 1 Last used)         | 0/1   | done   |
| `0x0032` | Auto-arm output after apply                     | 0/1   | done   |
| `0x0033` | Settings-sync request (view4 entry / HMI ready) | 1     | done   |

`0x0024` is sent by the panel on **every view2 entry** (`view2_reset_panel`); the
RP forces a fresh PDO re-read and re-pushes the list, so the list is always
current regardless of HMI boot timing or a late source attach. On **apply** the
panel sends `0x0020`/`0x0021` (range rails only), then `0x0023`; the RP latches
the adjust values, maps the list position to the real PDO, requests it, and —
**if auto-arm is enabled** (`0x0032`) — arms the output, then the panel animates
back to Monitor. The toggle colour follows the **real** output state pushed on
`0x0016`, not the apply event.

`0x0031`/`0x0032` are written when the user changes them in Settings and are
persisted to flash. `0x0033` is sent both on **view4 entry** and on **HMI ready**
(`view1_entry`, after the panel's ~7 s boot); the RP replies by pushing the stored
`0x0031`/`0x0032` so the panel reflects the saved state. The RP also re-pushes
these for the first ~12 s after boot to cover the slow HMI bring-up.

---

---

## HMI UI (Giraffe)

Apple-style dark UI, 720×720. Pages are Giraffe **views**, navigated with
`grf_view_set_dis_view_anim()`. Per-view control limit raised from 64 to 80.

- **view1 — Monitor (boot):** voltage ring (`arc`, reg `0x0010`), V/I/P labels,
  output toggle (`imgbtn`, toggle → reg `0x0022`). The toggle visual is driven by
  the **real** output state (reg `0x0016`): `view1_entry` and the `0x0016` handler
  both call `view1_set_output_btn()`, so it reads red/green correctly on every
  entry — not just after an apply. `view1_entry` also sends `0x0033` (HMI ready).
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
  Brightness, appearance, and lifetime-energy widgets are mocked, not yet wired.

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
- **Settings persistence:** a small `Settings` struct (magic `0xCB02`) lives in
  flash via the earlephilhower **EEPROM** emulation — `loadSettings()` at boot,
  `saveSettings()` on each change (guarded so only real changes write). Fields:
  boot-output state, auto-arm, last output on/off, last list position, last mV/mA.
  Changing the layout bumps the magic and resets settings once.
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
- [ ] Battery page (view3) content.
- [ ] Settings: brightness, dark/light theme, lifetime-energy odometer (mocked).
- [ ] Session energy widget on Monitor (mocked; regs 0x0013–0x0015 reserved).
- [ ] Slide-up animation for the adjust panel (blocked: `grf_animation_set` no-op).