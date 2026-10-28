# Week 07 Lab — State Machines and the LCD Display (ET4047)

**Module:** ET4047 · **Author:** Luke Griffin
**University of Limerick — Department of Electronic & Computer Engineering**

🌐 **Open the lab online:** https://lukegrif.github.io/LAB-ET4047/W07/

In this lab you build a traffic light as a **finite state machine**, drive real
LEDs from it, turn it into a **pedestrian crossing** that reacts to a button
using `millis()` instead of `delay()`, and finish with a **code lock** that
checks what you type with a **SHA-256 hash** (following on from SHA-1 in Week 05).

| # | Task | What you do |
|---|------|-------------|
| 1 | A traffic light on the LCD | RED → GREEN → YELLOW with an `enum`, a state variable and `switch`-`case`; add a fourth state |
| 2 | Traffic light with real LEDs | LEDs on D5/D6/D7; every state sets every output; state names from arrays |
| 3 | A pedestrian crossing | Button on D2, four states, no `delay()`: timing with `millis()` |
| 4 | A code lock with SHA-256 | LOCKED / OPEN / WRONG / ALARM; codes typed in the Serial Monitor are checked by hash |

The lab document has a wiring diagram for every task and a state diagram for
every state machine. The diagrams are also in [`Diagrams/`](Diagrams).

---

## One set of code for both displays

Every sketch works with **both** displays. Near the top of each one is a single
setting:

```cpp
#define RGB_SHIELD 0   // 0 = 16x2 LCD with I2C backpack, 1 = Adafruit RGB LCD shield
```

| Display | Setting | Library (Tools → Manage Libraries…) |
|---------|---------|-------------------------------------|
| 16×2 LCD with I2C backpack | `RGB_SHIELD 0` (default) | LiquidCrystal I2C (by Frank de Brabander) |
| Adafruit RGB LCD shield | `RGB_SHIELD 1` | Adafruit RGB LCD Shield Library (by Adafruit) |

Only the library for your display is needed. On the RGB shield the backlight
also changes colour to match the light. Every sketch is a **single `.ino`
file**; the SHA-256 code is at the bottom of the Task 4 sketch.

| Task | Sketch | Hardware |
|------|--------|----------|
| 1 | [`Task1_Traffic_Light`](Code/Task1_Traffic_Light) | LCD |
| 2 | [`Task2_Traffic_Light_LEDs`](Code/Task2_Traffic_Light_LEDs) | LCD + LEDs on D5 (red), D6 (yellow), D7 (green), each with 220 Ω |
| 3 | [`Task3_Pedestrian_Crossing`](Code/Task3_Pedestrian_Crossing) | Task 2 + push button between D2 and GND |
| 4 | [`Task4_SHA256_Lock`](Code/Task4_SHA256_Lock) | Task 2 LEDs; code typed in the Serial Monitor (Newline) |

---

## Repository contents

| Path | Description |
|------|-------------|
| `index.html` | GitHub Pages landing page for the lab |
| `viewer.html` | Renders the lab PDF in the browser |
| `assets/` | University logos used on the landing page |
| `W07_LAB_State_Machines_and_the_LCD_Display.pdf` | The lab document (no code listings: students open the sketches) |
| `W07 LAB State Machines and the LCD Display.docx` | Editable Word version |
| `W07_State_Machines_Lab_Code.zip` | All the sketches in one zip, ready to hand out |
| `Code/` | The same sketches, unzipped |
| `Diagrams/` | Wiring, state and timing diagrams used in the document |
