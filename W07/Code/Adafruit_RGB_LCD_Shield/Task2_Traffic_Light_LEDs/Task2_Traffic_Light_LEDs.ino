// =====================================================================
//  Task 2: Traffic light with real LEDs
//  Display: Adafruit RGB LCD shield
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
//    The shield's backlight also changes colour to match the light.
//
//  Wiring:
//    D5 -> 220 ohm resistor -> RED LED    (long leg) ; short leg -> GND
//    D6 -> 220 ohm resistor -> YELLOW LED (long leg) ; short leg -> GND
//    D7 -> 220 ohm resistor -> GREEN LED  (long leg) ; short leg -> GND
//    LCD shield: 5V -> 5V, GND -> GND, SDA -> A4, SCL -> A5
//                (the I2C pins; see the wiring diagram at the start of the lab)
//
//    If your breakout shield has its own LEDs, you can use those
//    instead: change RED_PIN, YELLOW_PIN and GREEN_PIN below.
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
const uint8_t     stateColour[]  = {BL_RED, BL_GREEN, BL_YELLOW};

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
    lcd.setBacklight(stateColour[currentState]);   // backlight matches the light

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
