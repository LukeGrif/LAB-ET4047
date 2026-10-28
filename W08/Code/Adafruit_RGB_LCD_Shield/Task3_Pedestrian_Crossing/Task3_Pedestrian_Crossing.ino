// =====================================================================
//  Task 3: Pedestrian crossing (no delay(): uses millis())
//  Display: Adafruit RGB LCD shield
// ---------------------------------------------------------------------
//  What this sketch does:
//    The cars' light stays GREEN until a pedestrian presses the button.
//    Then the light goes YELLOW, then RED while the pedestrians WALK
//    (with a countdown on the LCD), then stays RED for a moment to
//    CLEAR the crossing, and goes back to GREEN.
//
//        GREEN --(button released AND green for at least 5 s)--> YELLOW
//        YELLOW --(2 s)--> WALK --(6 s)--> CLEAR --(2 s)--> GREEN
//
//    There is no delay() anywhere. loop() runs thousands of times a
//    second, and each time it checks:
//      - has the button been pressed and released?
//      - has the current state lasted long enough to move on?
//    The shield's backlight also changes colour to match the cars' light.
//    millis() is the number of milliseconds since the board started,
//    so  millis() - stateStart  is how long we have been in this state.
//
//    The button reacts on the RISING edge: when it is RELEASED and D2
//    goes from LOW back to HIGH. It is also DEBOUNCED: a mechanical
//    button "bounces" for a few milliseconds, so a new reading only
//    counts once it has stayed the same for DEBOUNCE_TIME (50 ms).
//
//  Wiring (the same as Task 2, plus the button):
//    D5 -> 220 ohm -> RED LED    -> GND
//    D6 -> 220 ohm -> YELLOW LED -> GND
//    D7 -> 220 ohm -> GREEN LED  -> GND
//    Push button between D2 and GND
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

// ----- Pins -----
const uint8_t RED_PIN    = 5;
const uint8_t YELLOW_PIN = 6;
const uint8_t GREEN_PIN  = 7;
const uint8_t BUTTON_PIN = 2;      // pedestrian button, D2 to GND

// ----- 1. The states -----
enum CrossingState
{
    GREEN,      // cars go, pedestrians wait
    YELLOW,     // cars get ready to stop
    WALK,       // cars stopped, pedestrians cross
    CLEAR       // cars still stopped while the crossing empties
};

const char* const stateNames[] = {"GREEN", "YELLOW", "WALK", "CLEAR"};

// ----- 2. The current state, and when we entered it -----
CrossingState currentState = GREEN;
unsigned long stateStart = 0;      // millis() when the current state began

// How long the states last, in milliseconds
const unsigned long MIN_GREEN   = 5000;    // cars always get at least 5 s
const unsigned long YELLOW_TIME = 2000;
const unsigned long WALK_TIME   = 6000;
const unsigned long CLEAR_TIME  = 2000;

bool walkRequested = false;        // true once someone has pressed the button

// ----- Button debouncing -----
const unsigned long DEBOUNCE_TIME = 50;    // a reading must stay the same this long (ms)
uint8_t buttonState = HIGH;        // the debounced (steady) state: HIGH = not pressed
uint8_t lastReading = HIGH;        // the raw reading from the previous pass of loop()
unsigned long lastChange = 0;      // millis() when the raw reading last changed
int  lastSecondsShown = -1;        // for the WALK countdown on the LCD


void setLights(uint8_t red, uint8_t yellow, uint8_t green)
{
    digitalWrite(RED_PIN,    red    ? HIGH : LOW);
    digitalWrite(YELLOW_PIN, yellow ? HIGH : LOW);
    digitalWrite(GREEN_PIN,  green  ? HIGH : LOW);
}


// Change to a new state. Everything that happens ONCE, on the way
// into a state, is done here: note the time, set the LEDs, update the LCD.
void enterState(CrossingState newState)
{
    currentState = newState;
    stateStart = millis();

    Serial.print(F("-> "));
    Serial.println(stateNames[currentState]);

    lcd.clear();
    lcd.print(F("Cars: "));

    switch (currentState)
    {
        case GREEN:
            setLights(LOW, LOW, HIGH);
            lcd.setBacklight(BL_GREEN);
            lcd.print(F("GREEN"));
            lcd.setCursor(0, 1);
            lcd.print(F("DON'T WALK"));
            break;

        case YELLOW:
            setLights(LOW, HIGH, LOW);
            lcd.setBacklight(BL_YELLOW);
            lcd.print(F("YELLOW"));
            lcd.setCursor(0, 1);
            lcd.print(F("WAIT..."));
            walkRequested = false;     // the request is now being served
            break;

        case WALK:
            setLights(HIGH, LOW, LOW);
            lcd.setBacklight(BL_RED);
            lcd.print(F("RED"));
            lastSecondsShown = -1;     // so the countdown is drawn straight away
            break;

        case CLEAR:
            setLights(HIGH, LOW, LOW);
            lcd.setBacklight(BL_RED);
            lcd.print(F("RED"));
            lcd.setCursor(0, 1);
            lcd.print(F("DON'T WALK"));
            break;
    }
}


// Read the button and debounce it. Returns true ONCE, on the rising
// edge: when the button is released (D2 goes LOW -> HIGH) and has
// stayed released for DEBOUNCE_TIME. Call it on every pass of loop().
bool buttonReleased()
{
    uint8_t reading = digitalRead(BUTTON_PIN);

    if (reading != lastReading)        // the pin changed: a real change or a bounce
    {
        lastChange = millis();         // restart the debounce timer
        lastReading = reading;
    }

    // Has the reading been steady for long enough, and is it a new state?
    if (millis() - lastChange >= DEBOUNCE_TIME && reading != buttonState)
    {
        buttonState = reading;         // accept the new steady state
        if (buttonState == HIGH)       // LOW -> HIGH: the rising edge (released)
        {
            return true;
        }
    }
    return false;                      // no new release this time
}


void setup()
{
    Serial.begin(9600);

    pinMode(RED_PIN,    OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN,  OUTPUT);

    // INPUT_PULLUP: the pin reads HIGH normally, and LOW while the
    // button is pressed (the button connects D2 to GND).
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    lcd.begin(16, 2);                  // start the LCD: 16 columns, 2 rows
    lcd.setBacklight(BL_WHITE);        // switch the backlight on (white)

    Serial.println(F("Crossing ready"));
    enterState(GREEN);
}


void loop()
{
    unsigned long elapsed = millis() - stateStart;    // time in this state so far

    // ----- Input: read the button on EVERY pass of loop() -----
    // buttonReleased() is called first so the debouncing runs every pass
    if (buttonReleased() && !walkRequested
        && (currentState == GREEN || currentState == CLEAR))
    {
        walkRequested = true;          // remember it until it is served
        Serial.println(F("Button released"));
        lcd.setCursor(0, 1);
        lcd.print(F("WAIT...   "));    // spaces wipe out "DON'T WALK"
    }

    // ----- 3. Decide whether it is time to move to another state -----
    switch (currentState)
    {
        case GREEN:
            // Two conditions: someone is waiting AND cars have had their minimum green
            if (walkRequested && elapsed >= MIN_GREEN)
            {
                enterState(YELLOW);
            }
            break;

        case YELLOW:
            if (elapsed >= YELLOW_TIME)
            {
                enterState(WALK);
            }
            break;

        case WALK:
            if (elapsed >= WALK_TIME)
            {
                enterState(CLEAR);
            }
            else
            {
                // Count down the seconds left, but only redraw when the number changes
                int secondsLeft = (WALK_TIME - elapsed + 999) / 1000;
                if (secondsLeft != lastSecondsShown)
                {
                    lastSecondsShown = secondsLeft;
                    lcd.setCursor(0, 1);
                    lcd.print(F("WALK  "));
                    lcd.print(secondsLeft);
                    lcd.print(F("s "));
                }
            }
            break;

        case CLEAR:
            if (elapsed >= CLEAR_TIME)
            {
                enterState(GREEN);
                if (walkRequested)     // pressed during CLEAR: keep showing it
                {
                    lcd.setCursor(0, 1);
                    lcd.print(F("WAIT...   "));
                }
            }
            break;
    }
}
