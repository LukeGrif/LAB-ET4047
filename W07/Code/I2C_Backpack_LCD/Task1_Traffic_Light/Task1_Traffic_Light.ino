// =====================================================================
//  Task 1: A traffic light state machine (shown on the LCD)
//  Display: 16x2 LCD with I2C backpack
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
//  Wiring:
//    LCD:    GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//
//  Library (Tools -> Manage Libraries...):
//    "LiquidCrystal I2C" by Frank de Brabander
// =====================================================================

#include <Wire.h>                  // I2C bus: the LCD is controlled over SDA and SCL
#include <LiquidCrystal_I2C.h>     // driver for the I2C backpack LCD

// The LCD object: I2C address 0x20, 16 columns, 2 rows.
// The lab backpacks are at address 0x20. If your screen stays blank,
// your backpack may use another address: try 0x27 or 0x3F.
LiquidCrystal_I2C lcd(0x20, 16, 2);

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

    lcd.init();                        // start the LCD
    lcd.backlight();                   // switch the backlight on

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
            Serial.println(F("State: RED"));
            delay(RED_TIME);           // stay in RED for 5 seconds
            currentState = GREEN;      // transition: RED -> GREEN
            break;

        case GREEN:
            lcd.clear();
            lcd.print(F("State: GREEN"));
            Serial.println(F("State: GREEN"));
            delay(GREEN_TIME);         // stay in GREEN for 5 seconds
            currentState = YELLOW;     // transition: GREEN -> YELLOW
            break;

        case YELLOW:
            lcd.clear();
            lcd.print(F("State: YELLOW"));
            Serial.println(F("State: YELLOW"));
            delay(YELLOW_TIME);        // stay in YELLOW for 2 seconds
            currentState = RED;        // transition: YELLOW -> RED
            break;
    }
    // loop() now ends and starts again. currentState has changed,
    // so next time round a different case runs.
}
