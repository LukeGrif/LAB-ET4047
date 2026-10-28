// =====================================================================
//  Task 1: A traffic light state machine (shown on the LCD)
//  Display: Adafruit RGB LCD shield
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
//    The shield's backlight also changes colour to match the light.
//
//  Wiring:
//    LCD shield: 5V -> 5V, GND -> GND, SDA -> A4, SCL -> A5
//                (the I2C pins; see the wiring diagram at the start of the lab)
//
//  Library (Tools -> Manage Libraries...):
//    "Adafruit RGB LCD Shield Library" by Adafruit
// =====================================================================

#include <Wire.h>                  // I2C bus: the shield is controlled over SDA and SCL
#include <Adafruit_RGBLCDShield.h> // driver for the Adafruit RGB LCD shield

// The LCD object. The shield is always at I2C address 0x20,
// so no address is needed.
Adafruit_RGBLCDShield lcd;

// Backlight colours for lcd.setBacklight()
// (called BL_... so they do not clash with the state names RED, GREEN, ...)
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


void setup()
{
    Serial.begin(9600);                // USB Serial Monitor

    lcd.begin(16, 2);                  // start the LCD: 16 columns, 2 rows
    lcd.setBacklight(BL_WHITE);        // switch the backlight on (white)

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
            lcd.setBacklight(BL_RED);         // backlight matches the light
            Serial.println(F("State: RED"));
            delay(RED_TIME);           // stay in RED for 5 seconds
            currentState = GREEN;      // transition: RED -> GREEN
            break;

        case GREEN:
            lcd.clear();
            lcd.print(F("State: GREEN"));
            lcd.setBacklight(BL_GREEN);         // backlight matches the light
            Serial.println(F("State: GREEN"));
            delay(GREEN_TIME);         // stay in GREEN for 5 seconds
            currentState = YELLOW;     // transition: GREEN -> YELLOW
            break;

        case YELLOW:
            lcd.clear();
            lcd.print(F("State: YELLOW"));
            lcd.setBacklight(BL_YELLOW);         // backlight matches the light
            Serial.println(F("State: YELLOW"));
            delay(YELLOW_TIME);        // stay in YELLOW for 2 seconds
            currentState = RED;        // transition: YELLOW -> RED
            break;
    }
    // loop() now ends and starts again. currentState has changed,
    // so next time round a different case runs.
}
