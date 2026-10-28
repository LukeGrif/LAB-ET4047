W07 LAB - State Machines and the LCD Display (ET4047)
=====================================================

Every sketch works with BOTH displays.

1. Find out which display you have:

     16x2 LCD with I2C backpack  (separate screen, 4 wires)
     Adafruit RGB LCD shield     (plugs on, has 5 buttons)

2. Install the library for your display (Tools -> Manage Libraries...):

     I2C backpack LCD:  "LiquidCrystal I2C" by Frank de Brabander
     RGB LCD shield:    "Adafruit RGB LCD Shield Library" by Adafruit

   Wire is built in.

3. Open each sketch with File -> Open. Near the top of every sketch is:

     #define RGB_SHIELD 0

   Leave it at 0 for the I2C backpack LCD.
   Change it to 1 for the Adafruit RGB LCD shield.
   On the RGB shield the backlight also changes colour with the light.

4. The sketches:

   Task1_Traffic_Light         LCD only
   Task2_Traffic_Light_LEDs    LEDs on D5 (red), D6 (yellow), D7 (green)
   Task3_Pedestrian_Crossing   LEDs + push button between D2 and GND
   Task4_SHA256_Lock           LEDs; type the code in the Serial Monitor
                               (9600 baud, line ending "Newline")

Each LED needs a 220 ohm resistor in series. If your breakout shield has
its own LEDs, change RED_PIN, YELLOW_PIN and GREEN_PIN at the top of the
sketch instead.
