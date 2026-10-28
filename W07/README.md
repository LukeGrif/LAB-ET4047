# Week 07 Lab — State Machines and the LCD Display (ET4047)

**Module:** ET4047 · **Author:** Luke Griffin
**University of Limerick — Department of Electronic & Computer Engineering**

🌐 **Open the lab online:** https://lukegrif.github.io/LAB-ET4047/W07/

In this lab you build a traffic light as a **finite state machine**, drive real
LEDs from it, and turn it into a **pedestrian crossing** that reacts to a button
using `millis()` instead of `delay()`.

| # | Task | What you do |
|---|------|-------------|
| 1 | A traffic light on the LCD | RED → GREEN → YELLOW with an `enum`, a state variable and `switch`-`case`; add a fourth state |
| 2 | Traffic light with real LEDs | LEDs on D5/D6/D7; every state sets every output; state names from arrays |
| 3 | A pedestrian crossing | Button on D2, four states, no `delay()`: timing with `millis()` |

The lab document has a wiring diagram for every task and a state diagram for
every state machine. The diagrams are also in [`Diagrams/`](Diagrams).

---

## Choose the code for your display

Use **only** the folder for your LCD. Both folders contain the same sketches;
only the lines that set up the LCD are different (on the RGB shield the
backlight also follows the light's colour).

| Display | Folder | Library (Tools → Manage Libraries…) |
|---------|--------|-------------------------------------|
| 16×2 LCD with I2C backpack | [`Code/I2C_Backpack_LCD`](Code/I2C_Backpack_LCD) | LiquidCrystal I2C (by Frank de Brabander) |
| Adafruit RGB LCD shield | [`Code/Adafruit_RGB_LCD_Shield`](Code/Adafruit_RGB_LCD_Shield) | Adafruit RGB LCD Shield Library (by Adafruit) |

| Task | Sketch | Hardware |
|------|--------|----------|
| 1 | `Task1_Traffic_Light` | LCD |
| 2 | `Task2_Traffic_Light_LEDs` | LCD + LEDs on D5 (red), D6 (yellow), D7 (green), each with 220 Ω |
| 3 | `Task3_Pedestrian_Crossing` | Task 2 + push button between D2 and GND |

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
