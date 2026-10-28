// =====================================================================
//  Task 2: Traffic light with real LEDs
//  Display: 16x2 LCD with I2C backpack
// ---------------------------------------------------------------------
//  What this sketch does:
//    The same RED -> GREEN -> YELLOW state machine as Task 1, but now
//    each state also drives three LEDs. Every state sets ALL THREE
//    LEDs (one on, two off), so no light is ever left on by mistake.
//
//    The LCD shows the state name on the top row and what it means
//    for the driver on the bottom row. The names come from two arrays
//    that are in the same order as the enum, so
//        stateNames[currentState]  gives  "RED", "GREEN" or "YELLOW".
//
//  Wiring:
//    D5 -> 220 ohm resistor -> RED LED    (long leg) ; short leg -> GND
//    D6 -> 220 ohm resistor -> YELLOW LED (long leg) ; short leg -> GND
//    D7 -> 220 ohm resistor -> GREEN LED  (long leg) ; short leg -> GND
//    LCD:    GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//
//    If your breakout shield has its own LEDs, you can use those
//    instead: change RED_PIN, YELLOW_PIN and GREEN_PIN below.
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

// ----- The LED pins -----
const uint8_t RED_PIN    = 5;
const uint8_t YELLOW_PIN = 6;
const uint8_t GREEN_PIN  = 7;

// ----- 1. The states -----
enum TrafficLightState
{
    RED,        // = 0
    GREEN,      // = 1
    YELLOW      // = 2
};

// Text for each state, in the SAME ORDER as the enum above
const char* const stateNames[]   = {"RED",  "GREEN", "YELLOW"};
const char* const stateMeaning[] = {"STOP", "GO",    "GET READY"};

// ----- 2. The current state -----
TrafficLightState currentState = RED;

// How long each state lasts, in milliseconds
const unsigned long RED_TIME    = 5000;
const unsigned long GREEN_TIME  = 5000;
const unsigned long YELLOW_TIME = 2000;


// Set all three LEDs in one go: HIGH = on, LOW = off
void setLights(uint8_t red, uint8_t yellow, uint8_t green)
{
    digitalWrite(RED_PIN,    red    ? HIGH : LOW);
    digitalWrite(YELLOW_PIN, yellow ? HIGH : LOW);
    digitalWrite(GREEN_PIN,  green  ? HIGH : LOW);
}


// Show the current state on the LCD and the Serial Monitor
void showState()
{
    lcd.clear();
    lcd.print(F("State: "));
    lcd.print(stateNames[currentState]);       // top row, e.g. "State: RED"
    lcd.setCursor(0, 1);                       // column 0, row 1 (bottom row)
    lcd.print(stateMeaning[currentState]);     // bottom row, e.g. "STOP"

    Serial.print(F("State "));
    Serial.print((int)currentState);           // the number behind the name
    Serial.print(F(": "));
    Serial.println(stateNames[currentState]);
}


void setup()
{
    Serial.begin(9600);

    pinMode(RED_PIN,    OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN,  OUTPUT);
    setLights(LOW, LOW, LOW);          // start with everything off

    lcd.init();
    lcd.backlight();

    Serial.println(F("Traffic light ready"));
}


void loop()
{
    // ----- 3. Do the work of the current state -----
    switch (currentState)
    {
        case RED:
            setLights(HIGH, LOW, LOW);         // red on, the others off
            showState();
            delay(RED_TIME);
            currentState = GREEN;
            break;

        case GREEN:
            setLights(LOW, LOW, HIGH);         // green on, the others off
            showState();
            delay(GREEN_TIME);
            currentState = YELLOW;
            break;

        case YELLOW:
            setLights(LOW, HIGH, LOW);         // yellow on, the others off
            showState();
            delay(YELLOW_TIME);
            currentState = RED;
            break;
    }
}
