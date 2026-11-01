float Elapsed_Time = 0.0;      // seconds
enum TimerCommand { RESET, TIMECHECK };
struct TimerStatus {
  bool Running;
  bool Pause;
  bool Done;
};

 bool rawSwitch;   // HIGH = released, LOW = pressed
 bool sw;         // invert: TRUE = pressed
 float target = 10.0;                // 10-second target

TimerStatus TimerFunction(bool SwitchInput, TimerCommand command, float Time_Target) {

  static unsigned long lastCallTime = 0;         // internal for delta_t
  unsigned long now = millis();
  float delta_t = (now - lastCallTime) / 1000.0; // seconds
  lastCallTime = now;

  TimerStatus status;

  // ============================================================
  // RESET COMMAND
  // ============================================================
  if (command == RESET) {

    Elapsed_Time = 0.0;

    status.Running = false;
    status.Pause   = false;
    status.Done    = false;

    return status;
  }

  // ============================================================
  // TIMECHECK COMMAND
  // ============================================================
  if (command == TIMECHECK) {

    // Update elapsed time only when Switch = TRUE
    if (SwitchInput) {
      Elapsed_Time += delta_t;
    }

    // Logic definitions
    status.Pause = !SwitchInput;
    status.Done  = (Elapsed_Time >= Time_Target);

    // Running = NOT( NOT(SwitchInput) OR Done )
    //         = SwitchInput AND NOT(Done)
    status.Running = SwitchInput && !status.Done;

    return status;
  }

  // Default
  return {false,false,false};
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);   // DI2 active-low switch
  

  TimerStatus T = TimerFunction(sw, RESET, target);

}

void loop() {

  // Read active-low switch (DI2 = pin 2)
   rawSwitch = digitalRead(2);   // HIGH = released, LOW = pressed
   sw = !rawSwitch;              // invert: TRUE = pressed

  

  TimerStatus T = TimerFunction(sw, TIMECHECK, target);

  // Example debug prints
  Serial.println(Elapsed_Time,2);
  if (T.Running) Serial.println("Timer running...");
  if (T.Pause)   Serial.println("Timer paused.");
  if (T.Done){
     Serial.println("Timer done!");
     delay(10000);
     TimerStatus T = TimerFunction(sw, RESET, target);
  }    
 
  delay(100);
 
  }