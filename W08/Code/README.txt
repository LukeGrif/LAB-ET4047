W08 LAB - State Machines and the LCD Display (ET4047)
=====================================================

1. Find out which display you have:

     16x2 LCD with I2C backpack  (separate screen, 4 wires)  -> I2C_Backpack_LCD
     Adafruit RGB LCD shield     (has 5 buttons)              -> Adafruit_RGB_LCD_Shield

2. Use ONLY the folder for your display. Both folders contain the same
   sketches; only the lines that set up the LCD are different. On the RGB
   shield the backlight also changes colour to match the light.

3. Install the library for your display (Tools -> Manage Libraries...):

     I2C backpack LCD:  "LiquidCrystal I2C" by Frank de Brabander
     RGB LCD shield:    "Adafruit RGB LCD Shield Library" by Adafruit

4. Open each sketch with File -> Open:

   Task1_Traffic_Light         LCD only
   Task2_Traffic_Light_LEDs    LEDs on D5 (red), D6 (yellow), D7 (green)
   Task3_Pedestrian_Crossing   LEDs + push button between D2 and GND

Each LED needs a 220 ohm resistor in series.
