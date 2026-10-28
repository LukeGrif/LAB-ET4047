// =====================================================================
//  Task 1: A traffic light state machine (shown on the LCD)
//  Display: I2C backpack LCD OR Adafruit RGB LCD shield.
//           Set RGB_SHIELD below to match the display you have.
// ---------------------------------------------------------------------
//  What this sketch does:
//    The traffic light is always in exactly ONE of three states:
//        RED  ->  GREEN  ->  YELLOW  ->  back to RED ...
//    Each state shows its name on the LCD, waits for a fixed time,
//    and then moves on (a "transition") to the next state.
//
//    The three parts of every state machine are marked 1, 2 and 3:
//      1. an enum that lists the states
//      2. a variable that remembers the current state
//      3. a switch-case that does the work of the current state
//
//    On the RGB shield the backlight also changes colour to match the light.
//
//  Wiring:
//    I2C backpack LCD:  GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//    RGB LCD shield:    plug it onto the breakout board (no wires)
//
//  Library (Tools -> Manage Libraries...), for your display only:
//    I2C backpack LCD:  "LiquidCrystal I2C" by Frank de Brabander
//    RGB LCD shield:    "Adafruit RGB LCD Shield Library" by Adafruit
// =====================================================================

// ----- WHICH DISPLAY DO YOU HAVE? (the only line you may need to change) -----
//   0 = 16x2 LCD with I2C backpack  (a separate screen with four wires)
//   1 = Adafruit RGB LCD shield     (plugs on and has five buttons)
#define RGB_SHIELD 0

#include <Wire.h>                      // I2C bus: the display is controlled over SDA and SCL

#if RGB_SHIELD
  #include <Adafruit_RGBLCDShield.h>   // driver for the Adafruit RGB LCD shield
  Adafruit_RGBLCDShield lcd;           // the shield is always at I2C address 0x20
#else
  #include <LiquidCrystal_I2C.h>       // driver for the I2C backpack LCD
  // I2C address 0x20, 16 columns, 2 rows. The lab backpacks are at 0x20.
  // If your screen stays blank, your backpack may use 0x27 or 0x3F.
  LiquidCrystal_I2C lcd(0x20, 16, 2);
#endif

// Backlight colours. Only the RGB shield can change colour; on the
// I2C backpack lcdColour() does nothing. (Called BL_... so they do not
// clash with the state names RED, GREEN, ...)
const uint8_t BL_RED    = 0x1;
const uint8_t BL_GREEN  = 0x2;
const uint8_t BL_YELLOW = 0x3;
const uint8_t BL_WHITE  = 0x7;

// ----- 1. The states, listed in an enum -----
// The compiler numbers them for us (RED = 0, GREEN = 1, YELLOW = 2),
// but in the code we only ever use the names.
enum TrafficLightState
{
    RED,
    GREEN,
    YELLOW
};

// ----- 2. The current state. A traffic light starts on RED -----
TrafficLightState currentState = RED;

// How long each state lasts, in milliseconds
const unsigned long RED_TIME    = 5000;    // 5 seconds
const unsigned long GREEN_TIME  = 5000;    // 5 seconds
const unsigned long YELLOW_TIME = 2000;    // 2 seconds


// Start the display (each display's library uses different names for this)
void lcdStart()
{
#if RGB_SHIELD
    lcd.begin(16, 2);                  // 16 columns, 2 rows
    lcd.setBacklight(BL_WHITE);        // backlight on (white)
#else
    lcd.init();
    lcd.backlight();                   // backlight on
#endif
}

// Change the backlight colour (RGB shield only)
void lcdColour(uint8_t colour)
{
#if RGB_SHIELD
    lcd.setBacklight(colour);
#else
    (void)colour;                      // the I2C backpack has one colour only
#endif
}


void setup()
{
    Serial.begin(9600);                // USB Serial Monitor

    lcdStart();                        // start the display (see the top of the sketch)

    Serial.println(F("Traffic light ready"));
}


void loop()
{
    // ----- 3. Do the work of the current state -----
    switch (currentState)
    {
        case RED:
            lcd.clear();
            lcd.print(F("State: RED"));
            lcdColour(BL_RED);         // backlight matches the light
            Serial.println(F("State: RED"));
            delay(RED_TIME);           // stay in RED for 5 seconds
            currentState = GREEN;      // transition: RED -> GREEN
            break;

        case GREEN:
            lcd.clear();
            lcd.print(F("State: GREEN"));
            lcdColour(BL_GREEN);         // backlight matches the light
            Serial.println(F("State: GREEN"));
            delay(GREEN_TIME);         // stay in GREEN for 5 seconds
            currentState = YELLOW;     // transition: GREEN -> YELLOW
            break;

        case YELLOW:
            lcd.clear();
            lcd.print(F("State: YELLOW"));
            lcdColour(BL_YELLOW);         // backlight matches the light
            Serial.println(F("State: YELLOW"));
            delay(YELLOW_TIME);        // stay in YELLOW for 2 seconds
            currentState = RED;        // transition: YELLOW -> RED
            break;
    }
    // loop() now ends and starts again. currentState has changed,
    // so next time round a different case runs.
}
