// =====================================================================
//  Task 3: Pedestrian crossing (no delay(): uses millis())
//  Display: I2C backpack LCD OR Adafruit RGB LCD shield.
//           Set RGB_SHIELD below to match the display you have.
// ---------------------------------------------------------------------
//  What this sketch does:
//    The cars' light stays GREEN until a pedestrian presses the button.
//    Then the light goes YELLOW, then RED while the pedestrians WALK
//    (with a countdown on the LCD), then stays RED for a moment to
//    CLEAR the crossing, and goes back to GREEN.
//
//        GREEN --(button pressed AND green for at least 5 s)--> YELLOW
//        YELLOW --(2 s)--> WALK --(6 s)--> CLEAR --(2 s)--> GREEN
//
//    There is no delay() anywhere. loop() runs thousands of times a
//    second, and each time it checks:
//      - has the button been pressed?
//      - has the current state lasted long enough to move on?
//    On the RGB shield the backlight also changes colour to match the cars' light.
//    millis() is the number of milliseconds since the board started,
//    so  millis() - stateStart  is how long we have been in this state.
//
//  Wiring (the same as Task 2, plus the button):
//    D5 -> 220 ohm -> RED LED    -> GND
//    D6 -> 220 ohm -> YELLOW LED -> GND
//    D7 -> 220 ohm -> GREEN LED  -> GND
//    Push button between D2 and GND
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
int  lastSecondsShown = -1;        // for the WALK countdown on the LCD


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
            lcdColour(BL_GREEN);
            lcd.print(F("GREEN"));
            lcd.setCursor(0, 1);
            lcd.print(F("DON'T WALK"));
            break;

        case YELLOW:
            setLights(LOW, HIGH, LOW);
            lcdColour(BL_YELLOW);
            lcd.print(F("YELLOW"));
            lcd.setCursor(0, 1);
            lcd.print(F("WAIT..."));
            walkRequested = false;     // the request is now being served
            break;

        case WALK:
            setLights(HIGH, LOW, LOW);
            lcdColour(BL_RED);
            lcd.print(F("RED"));
            lastSecondsShown = -1;     // so the countdown is drawn straight away
            break;

        case CLEAR:
            setLights(HIGH, LOW, LOW);
            lcdColour(BL_RED);
            lcd.print(F("RED"));
            lcd.setCursor(0, 1);
            lcd.print(F("DON'T WALK"));
            break;
    }
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

    lcdStart();                        // start the display (see the top of the sketch)

    Serial.println(F("Crossing ready"));
    enterState(GREEN);
}


void loop()
{
    unsigned long elapsed = millis() - stateStart;    // time in this state so far

    // ----- Input: read the button on EVERY pass of loop() -----
    if (digitalRead(BUTTON_PIN) == LOW && !walkRequested
        && (currentState == GREEN || currentState == CLEAR))
    {
        walkRequested = true;          // remember it until it is served
        Serial.println(F("Button pressed"));
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
