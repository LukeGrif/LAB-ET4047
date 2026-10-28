// =====================================================================
//  Task 4: A code lock with SHA-256 (a state machine that uses a hash)
//  Display: I2C backpack LCD OR Adafruit RGB LCD shield.
//           Set RGB_SHIELD below to match the display you have.
// ---------------------------------------------------------------------
//  What this sketch does:
//    You type a code into the Serial Monitor and press Enter. The board
//    works out the SHA-256 hash of what you typed and compares it with
//    the hash stored below. The code itself is NOT stored anywhere in
//    the sketch: only its hash.
//
//        LOCKED --right code--> OPEN  --(5 s)--> LOCKED
//        LOCKED --wrong code--> WRONG --(2 s)--> LOCKED
//        LOCKED --3rd wrong code in a row--> ALARM --(15 s)--> LOCKED
//
//    The LEDs show the state: yellow = LOCKED, green = OPEN,
//    red = WRONG, flashing red = ALARM. On the RGB shield the
//    backlight changes colour too.
//
//  Wiring: the same LEDs as Tasks 2 and 3 (the button is not used)
//    D5 -> 220 ohm -> RED LED    -> GND
//    D6 -> 220 ohm -> YELLOW LED -> GND
//    D7 -> 220 ohm -> GREEN LED  -> GND
//    I2C backpack LCD:  GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//    RGB LCD shield:    plug it onto the breakout board (no wires)
//
//  Serial Monitor: 9600 baud, line ending "Newline".
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

// ----- The SHA-256 hash of the secret code -----
// This is the hash of "ET4047". To use your own code, type it in while
// the sketch is running, copy the "SHA-256:" line from the Serial
// Monitor, and paste it here (64 lowercase hex characters).
const char PASSWORD_HASH[] = "e0fe78d09229a737c8b66424c9b4bd456828da3b547128503a4cd41c6aa51fd1";

// ----- 1. The states -----
enum LockState
{
    LOCKED,     // waiting for a code
    OPEN,       // right code: unlocked for a few seconds
    WRONG,      // wrong code: short pause before the next try
    ALARM       // too many wrong codes: ignore everything for a while
};

const char* const stateNames[] = {"LOCKED", "OPEN", "WRONG", "ALARM"};

// ----- 2. The current state, and when we entered it -----
LockState currentState = LOCKED;
unsigned long stateStart = 0;

const unsigned long OPEN_TIME  = 5000;
const unsigned long WRONG_TIME = 2000;
const unsigned long ALARM_TIME = 15000;
const uint8_t MAX_TRIES = 3;

uint8_t wrongTries = 0;            // wrong codes in a row

// ----- The line buffer for what is typed (same idea as Week 05) -----
const uint8_t MAX_LEN = 20;
char typed[MAX_LEN + 1];
uint8_t len = 0;


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


// Change to a new state: note the time, set the LEDs, update the LCD
void enterState(LockState newState)
{
    currentState = newState;
    stateStart = millis();

    Serial.print(F("-> "));
    Serial.println(stateNames[currentState]);

    lcd.clear();
    switch (currentState)
    {
        case LOCKED:
            setLights(LOW, HIGH, LOW);
            lcdColour(BL_YELLOW);
            lcd.print(F("LOCKED"));
            lcd.setCursor(0, 1);
            lcd.print(F("Enter code"));
            break;

        case OPEN:
            setLights(LOW, LOW, HIGH);
            lcdColour(BL_GREEN);
            lcd.print(F("OPEN"));
            lcd.setCursor(0, 1);
            lcd.print(F("Welcome!"));
            break;

        case WRONG:
            setLights(HIGH, LOW, LOW);
            lcdColour(BL_RED);
            lcd.print(F("WRONG CODE"));
            lcd.setCursor(0, 1);
            lcd.print(F("Tries: "));
            lcd.print(wrongTries);
            lcd.print('/');
            lcd.print(MAX_TRIES);
            break;

        case ALARM:
            lcd.print(F("!! ALARM !!"));
            lcdColour(BL_RED);
            lcd.setCursor(0, 1);
            lcd.print(F("Locked out"));
            break;
    }
}


// A complete line has been typed: hash it and decide what happens next
void checkCode(const char* code)
{
    char hash[65];                     // 64 hex characters + '\0'
    sha256Hex(code, hash);

    Serial.print(F("Typed:   "));
    Serial.println(code);
    Serial.print(F("SHA-256: "));
    Serial.println(hash);

    if (strcmp(hash, PASSWORD_HASH) == 0)      // strcmp gives 0 when they match
    {
        wrongTries = 0;
        enterState(OPEN);
    }
    else
    {
        wrongTries++;
        if (wrongTries >= MAX_TRIES)
        {
            enterState(ALARM);
        }
        else
        {
            enterState(WRONG);
        }
    }
}


void setup()
{
    Serial.begin(9600);

    pinMode(RED_PIN,    OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN,  OUTPUT);

    lcdStart();                        // start the display (see the top of the sketch)

    Serial.println(F("Code lock ready. Type the code and press Enter."));
    enterState(LOCKED);
}


void loop()
{
    unsigned long elapsed = millis() - stateStart;

    // ----- Input: characters from the Serial Monitor -----
    while (Serial.available() > 0)
    {
        char c = Serial.read();

        if (currentState != LOCKED)    // only listen while LOCKED
        {
            continue;                  // throw the character away
        }

        if (c == '\r')                 // ignore carriage returns
        {
            continue;
        }

        if (c == '\n')                 // Enter: the code is complete
        {
            typed[len] = '\0';
            len = 0;
            checkCode(typed);
        }
        else if (len < MAX_LEN)
        {
            typed[len] = c;
            len++;
        }
    }

    // ----- 3. Timed transitions -----
    switch (currentState)
    {
        case LOCKED:
            break;                     // nothing to do: wait for a code

        case OPEN:
            if (elapsed >= OPEN_TIME)
            {
                enterState(LOCKED);
            }
            break;

        case WRONG:
            if (elapsed >= WRONG_TIME)
            {
                enterState(LOCKED);
            }
            break;

        case ALARM:
            // Flash the red LED: on for 250 ms, off for 250 ms
            digitalWrite(YELLOW_PIN, LOW);
            digitalWrite(GREEN_PIN,  LOW);
            digitalWrite(RED_PIN, (elapsed / 250) % 2 == 0 ? HIGH : LOW);

            if (elapsed >= ALARM_TIME)
            {
                wrongTries = 0;
                enterState(LOCKED);
            }
            break;
    }
}


// =====================================================================
//  SHA-256 HASH FUNCTION
// ---------------------------------------------------------------------
//  You do NOT need to change anything below this line, or understand
//  every detail. All you need to know is how to call it:
//
//      char hash[65];                // 64 hex characters + '\0'
//      sha256Hex("abc", hash);       // hash is now
//          // "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
//
//  SHA-256 works like the SHA-1 code from Week 05, in three steps:
//    1. Padding: add a 1 bit, then 0 bits, then the message length,
//       so the total is a whole number of 64-byte blocks.
//    2. Mixing: each 64-byte block goes through 64 rounds that scramble
//       EIGHT 32-bit numbers h[0]..h[7] (the function sha256Block).
//    3. Output: the final h[0]..h[7] (8 x 32 = 256 bits) is the hash,
//       written as 64 hexadecimal characters.
// =====================================================================

// The 64 round constants, fixed by the SHA-256 standard.
// PROGMEM keeps them in flash memory instead of using 256 bytes of RAM.
const uint32_t SHA256_K[64] PROGMEM = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

// Rotate a 32-bit number right by n bits (bits pushed out on the right
// come back in on the left)
uint32_t rotateRight(uint32_t x, uint8_t n)
{
    return (x >> n) | (x << (32 - n));
}

// Step 2: mix one 64-byte block into the eight state numbers h[0..7]
void sha256Block(const uint8_t* block, uint32_t* h)
{
    // Turn the 64 bytes into 16 32-bit words.
    // (Textbook SHA-256 uses 64 words here; keeping only 16 and reusing
    // them saves 192 bytes of RAM, which matters on a Nano.)
    uint32_t w[16];
    for (uint8_t i = 0; i < 16; i++)
    {
        w[i] = ((uint32_t)block[i * 4]     << 24) |
               ((uint32_t)block[i * 4 + 1] << 16) |
               ((uint32_t)block[i * 4 + 2] << 8)  |
                (uint32_t)block[i * 4 + 3];
    }

    uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
    uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];

    for (uint8_t t = 0; t < 64; t++)       // 64 rounds of mixing
    {
        if (t >= 16)                       // make the next word from earlier ones
        {
            uint32_t w15 = w[(t + 1) & 15];    // the word from 15 rounds ago
            uint32_t w2  = w[(t + 14) & 15];   // the word from 2 rounds ago
            uint32_t s0 = rotateRight(w15, 7)  ^ rotateRight(w15, 18) ^ (w15 >> 3);
            uint32_t s1 = rotateRight(w2, 17)  ^ rotateRight(w2, 19)  ^ (w2 >> 10);
            w[t & 15] += s0 + w[(t + 9) & 15] + s1;
        }

        uint32_t S1   = rotateRight(e, 6) ^ rotateRight(e, 11) ^ rotateRight(e, 25);
        uint32_t ch   = (e & f) ^ (~e & g);
        uint32_t temp1 = hh + S1 + ch + pgm_read_dword(&SHA256_K[t]) + w[t & 15];
        uint32_t S0   = rotateRight(a, 2) ^ rotateRight(a, 13) ^ rotateRight(a, 22);
        uint32_t maj  = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;

        hh = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    h[0] += a;  h[1] += b;  h[2] += c;  h[3] += d;
    h[4] += e;  h[5] += f;  h[6] += g;  h[7] += hh;
}

// Work out the SHA-256 of the text msg and write it into out
// as 64 lowercase hex characters plus '\0' (so out needs 65 chars)
void sha256Hex(const char* msg, char* out)
{
    // Starting values, fixed by the SHA-256 standard
    uint32_t h[8] = {0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
                     0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL};

    uint16_t len = strlen(msg);            // message length in bytes
    uint16_t pos = 0;                      // how much we have processed
    uint8_t block[64];

    // Process every complete 64-byte block of the message
    while (len - pos >= 64)
    {
        memcpy(block, msg + pos, 64);
        sha256Block(block, h);
        pos += 64;
    }

    // Step 1: padding. Copy what is left, then add a single 1 bit (0x80).
    uint8_t rest = len - pos;
    memset(block, 0, 64);                  // fill with zeros
    memcpy(block, msg + pos, rest);
    block[rest] = 0x80;

    // The last 8 bytes must hold the length. If there is no room,
    // finish this block and use one more block of zeros.
    if (rest >= 56)
    {
        sha256Block(block, h);
        memset(block, 0, 64);
    }

    // Message length in BITS, most significant byte first
    uint32_t bits = (uint32_t)len * 8;
    block[60] = bits >> 24;
    block[61] = bits >> 16;
    block[62] = bits >> 8;
    block[63] = bits;
    sha256Block(block, h);

    // Step 3: write h[0..7] out as 64 hex characters
    const char hexDigits[] = "0123456789abcdef";
    for (uint8_t i = 0; i < 32; i++)
    {
        uint8_t value = h[i / 4] >> (24 - 8 * (i % 4));   // one byte of the hash
        out[i * 2]     = hexDigits[value >> 4];           // high 4 bits
        out[i * 2 + 1] = hexDigits[value & 0x0F];         // low 4 bits
    }
    out[64] = '\0';
}
