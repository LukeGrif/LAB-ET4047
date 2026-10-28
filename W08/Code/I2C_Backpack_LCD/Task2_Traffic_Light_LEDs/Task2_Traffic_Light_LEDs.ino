// =====================================================================
//  Task 2: Traffic light with real LEDs
//  Display: 16x2 LCD with I2C backpack
// ---------------------------------------------------------------------
//  What this sketch does:
//    The same RED -> GREEN -> YELLOW state machine as Task 1, but each
//    state now also turns its LED on and the other two LEDs off.
//
//  Wiring:
//    D5 -> 220 ohm resistor -> RED LED    (long leg) ; short leg -> GND
//    D6 -> 220 ohm resistor -> YELLOW LED (long leg) ; short leg -> GND
//    D7 -> 220 ohm resistor -> GREEN LED  (long leg) ; short leg -> GND
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

// The LED pins
const int RED_PIN    = 5;
const int YELLOW_PIN = 6;
const int GREEN_PIN  = 7;

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
    Serial.begin(9600);

    pinMode(RED_PIN,    OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN,  OUTPUT);

    lcd.init();
    lcd.backlight();
}


void loop()
{
    // 3. Do the work of the current state
    switch (currentState)
    {
        case RED:
            digitalWrite(RED_PIN,    HIGH);    // red on
            digitalWrite(YELLOW_PIN, LOW);     // the others off
            digitalWrite(GREEN_PIN,  LOW);
            lcd.clear();
            lcd.print("State: RED");
            Serial.println("State: RED");
            delay(5000);                       // stay in RED for 5 seconds
            currentState = GREEN;              // RED -> GREEN
            break;

        case GREEN:
            digitalWrite(RED_PIN,    LOW);
            digitalWrite(YELLOW_PIN, LOW);
            digitalWrite(GREEN_PIN,  HIGH);    // green on
            lcd.clear();
            lcd.print("State: GREEN");
            Serial.println("State: GREEN");
            delay(5000);                       // stay in GREEN for 5 seconds
            currentState = YELLOW;             // GREEN -> YELLOW
            break;

        case YELLOW:
            digitalWrite(RED_PIN,    LOW);
            digitalWrite(YELLOW_PIN, HIGH);    // yellow on
            digitalWrite(GREEN_PIN,  LOW);
            lcd.clear();
            lcd.print("State: YELLOW");
            Serial.println("State: YELLOW");
            delay(2000);                       // stay in YELLOW for 2 seconds
            currentState = RED;                // YELLOW -> RED
            break;
    }
}
