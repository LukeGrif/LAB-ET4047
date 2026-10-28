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

    lcd.init();                        // start the LCD
    lcd.backlight();                   // switch the backlight on
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
