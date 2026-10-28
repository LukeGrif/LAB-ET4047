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
//  Wiring:
//    LCD shield: 5V -> 5V, GND -> GND, SDA -> A4, SCL -> A5
//
//  Library (Tools -> Manage Libraries...):
//    "Adafruit RGB LCD Shield Library" by Adafruit
// =====================================================================

#include <Wire.h>                  // I2C bus: the shield is controlled over SDA and SCL
#include <Adafruit_RGBLCDShield.h> // driver for the Adafruit RGB LCD shield

// The LCD object. The shield is always at I2C address 0x20.
Adafruit_RGBLCDShield lcd;

// 1. The states
enum TrafficLightState
{
    RED,
    GREEN,
    YELLOW
};

// 2. The current state. A traffic light starts on RED
TrafficLightState currentState = RED;


void setup()
{
    Serial.begin(9600);                // Serial Monitor at 9600 baud

    lcd.begin(16, 2);                  // start the LCD: 16 columns, 2 rows
    lcd.setBacklight(0x7);             // switch the backlight on (0x7 = white)
}


void loop()
{
    // 3. Do the work of the current state
    switch (currentState)
    {
        case RED:
            lcd.clear();
            lcd.print("State: RED");
            Serial.println("State: RED");
            delay(5000);                       // stay in RED for 5 seconds
            currentState = GREEN;              // RED -> GREEN
            break;

        case GREEN:
            lcd.clear();
            lcd.print("State: GREEN");
            Serial.println("State: GREEN");
            delay(5000);                       // stay in GREEN for 5 seconds
            currentState = YELLOW;             // GREEN -> YELLOW
            break;

        case YELLOW:
            lcd.clear();
            lcd.print("State: YELLOW");
            Serial.println("State: YELLOW");
            delay(2000);                       // stay in YELLOW for 2 seconds
            currentState = RED;                // YELLOW -> RED
            break;
    }
    // loop() now ends and starts again. currentState has changed,
    // so next time round a different case runs.
}
