// =====================================================================
//  Task 3: Pedestrian crossing (a button causes a transition)
//  Display: 16x2 LCD with I2C backpack
// ---------------------------------------------------------------------
//  What this sketch does:
//    The same traffic light as Task 2, but GREEN no longer ends after
//    a fixed time. It stays GREEN until a pedestrian presses the button:
//
//        RED --(after 5 s)--> GREEN --(button pressed)--> YELLOW
//        YELLOW --(after 2 s)--> RED
//
//  Wiring (the same as Task 2, plus the button):
//    D5 -> 220 ohm -> RED LED    -> GND
//    D6 -> 220 ohm -> YELLOW LED -> GND
//    D7 -> 220 ohm -> GREEN LED  -> GND
//    Push button between D2 and GND
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

// The pins
const int RED_PIN    = 5;
const int YELLOW_PIN = 6;
const int GREEN_PIN  = 7;
const int BUTTON_PIN = 2;          // push button between D2 and GND

// 1. The states
enum TrafficLightState
{
    RED,
    GREEN,
    YELLOW
};

// 2. The current state
TrafficLightState currentState = RED;


void setup()
{
    Serial.begin(9600);

    pinMode(RED_PIN,    OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN,  OUTPUT);

    // INPUT_PULLUP: D2 reads HIGH normally, and LOW while the button
    // is pressed (the button connects D2 to GND)
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    lcd.init();
    lcd.backlight();
}


void loop()
{
    // 3. Do the work of the current state
    switch (currentState)
    {
        case RED:
            digitalWrite(RED_PIN,    HIGH);
            digitalWrite(YELLOW_PIN, LOW);
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
            digitalWrite(GREEN_PIN,  HIGH);
            lcd.clear();
            lcd.print("State: GREEN");
            lcd.setCursor(0, 1);               // bottom row
            lcd.print("Press button");
            Serial.println("State: GREEN");

            // Wait here until the button is pressed (D2 goes LOW)
            while (digitalRead(BUTTON_PIN) == HIGH)
            {
                // do nothing
            }
            Serial.println("Button pressed");
            currentState = YELLOW;             // GREEN -> YELLOW
            break;

        case YELLOW:
            digitalWrite(RED_PIN,    LOW);
            digitalWrite(YELLOW_PIN, HIGH);
            digitalWrite(GREEN_PIN,  LOW);
            lcd.clear();
            lcd.print("State: YELLOW");
            Serial.println("State: YELLOW");
            delay(2000);                       // stay in YELLOW for 2 seconds
            currentState = RED;                // YELLOW -> RED
            break;
    }
}
