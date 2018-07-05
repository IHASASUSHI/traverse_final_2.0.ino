#include <AccelStepper.h> // Installed when installing Teensyduino
#include <MultiStepper.h> // Installed when installing Teensyduino

// References:
// Accelstepper GitHub: https://github.com/adafruit/AccelStepper
// AccelStepper class/function reference: http://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html
// Stepper Drivers Big Easy


#define TRAVERSE_STEP_PIN 3 // Traverse motor controller step pin
#define TRAVERSE_DIR_PIN 2  // Traverse motor controller direction pin
#define WINDER_STEP_PIN 5   // Winder motor controller step pin
#define WINDER_DIR_PIN 4    // Winder motor controller direction pin
#define PAUSE_BUTTON 6     // Button on pin 6
#define KNOB_1_A 23        // encoder on pin 23
#define KNOB_2_A 22        // encoder on pin 22
#define LIMIT_A 7         // tactile switch on pin 7
#define LIMIT_B 8         // tactile switch on pin 8

float traverse_speed = 15000; // 10,000 ticks per second angular velocity
float traverse_accel = 100000; // 100,000 ticks per second^2 angular acceleration
float winder_speed = 15000;
float winder_accel = 100000;

// Determine the number of ticks needed to traverse the bobbin
float bobbin_length = 4.3; // inches
float pully_dia = 0.625; // inches
float traverse_dist = bobbin_length / (pully_dia*PI); //4.3/(.625*pi)
int ticks_per_turn = 3200;
float traverse_ticks = traverse_dist * ticks_per_turn;
float goal[] = {0, traverse_ticks}; // to go from 0 to trav ticks
volatile uint8_t idx = 0;

long winderposition = 0; // winder position
long traverseposition = 0; //traverse position
String val; // Data received from the serial port


AccelStepper traverse(AccelStepper::DRIVER, TRAVERSE_STEP_PIN, TRAVERSE_DIR_PIN); //traverse pins
AccelStepper winder(AccelStepper::DRIVER, WINDER_STEP_PIN, WINDER_DIR_PIN); //winder pins
IntervalTimer timer; //precise timing interval

void setup() {
  //Serial Communication
  Serial.begin(115200);
  //establishContact();  // send a byte to establish contact until receiver responds

  // put your setup code here, to run once:
  traverse.moveTo(traverse_ticks);
  traverse.setAcceleration(100000);
  traverse.setMaxSpeed(traverse_speed);
  traverse.setSpeed(traverse_speed);

  winder.setMaxSpeed(winder_speed);
  winder.setSpeed(winder_speed);

  pinMode(LED_BUILTIN, OUTPUT); // LED Outputs directiondirection
  pinMode(PAUSE_BUTTON, INPUT_PULLUP);  // Button/ switch (input pullup)
  pinMode(LIMIT_A, INPUT);
  pinMode(LIMIT_B, INPUT);
  pinMode(KNOB_1_A, INPUT);
  pinMode(KNOB_2_A, INPUT);

  // TODO: "Pause" the program to adjust the carraige
  while (digitalRead(PAUSE_BUTTON) == LOW) {
    delay(100);
  }
  timer.begin(pulse, 20); //timing interval w/ pulse 20
}

void loop() {

  /*if (Serial.available() > 0) {
    val = Serial.read();

    if (val == "Right") {
      traverse.move(100);
      Serial.println("moved 100");
    }
    if (val == "Left") {
      traverse.move(-100);
      Serial.println("moved -100");
    }
    if (val == "p") {
      winder.setSpeed(10000);
      winder.move(100);
      Serial.println("moved 100");
    }
    if (val == "n") {
      winder.setSpeed(-10000);
      winder.move(-100);
      Serial.println("moved -100");
    }
    }
    else {
    Serial.println(val);
    delay(300);
    }*/


  boolean state = digitalRead(PAUSE_BUTTON);// holds value as it reads input pin for button
  // put your main code here, to run repeatedly:
  noInterrupts(); // disables interrupts
  traverseposition = traverse.currentPosition();
  winderposition = winder.currentPosition();
  int Knob_1_Value = analogRead(KNOB_1_A);
  int Knob_2_Value = analogRead(KNOB_2_A);
  
  if (state == 1) { // LOW //wait for code to start after button is pressed while digitalread button ==HIGH, do nothing
    traverse.setSpeed(0);
    winder.setSpeed(0);
  }
  else if (idx == 0) {
    if ((Knob_1_Value + Knob_2_Value) / 10 - floor((Knob_1_Value + Knob_2_Value) / 10) == .5 || (Knob_1_Value + Knob_2_Value) / 10 - floor((Knob_1_Value + Knob_2_Value) / 10) == .4) {
      traverse.setSpeed(floor((Knob_1_Value*3 + Knob_2_Value*3))); // otherwise, setSpeed is 10000
      winder.setSpeed(floor((Knob_1_Value*3 + Knob_2_Value*3)));
    }
    else {
      traverse.setSpeed(round((Knob_1_Value*3 + Knob_2_Value*3))); // otherwise, setSpeed is 10000
      winder.setSpeed(round((Knob_1_Value*3 + Knob_2_Value*3)));
    }
  }
  else {
    if ((Knob_1_Value + Knob_2_Value) / 10 - floor((Knob_1_Value + Knob_2_Value) / 10) || (Knob_1_Value + Knob_2_Value) / 10 - floor((Knob_1_Value + Knob_2_Value) / 10) >= .4) {
      traverse.setSpeed(-floor((Knob_1_Value*3 + Knob_2_Value*3))); // otherwise, setSpeed is 10000
      winder.setSpeed(floor((Knob_1_Value*3 + Knob_2_Value*3)));
    }
    else {
      traverse.setSpeed(-round((Knob_1_Value*3 + Knob_2_Value*3))); // otherwise, setSpeed is 10000
      winder.setSpeed(round((Knob_1_Value*3 + Knob_2_Value*3)));
    }
  }
  delay (100);
  Serial.println(floor((Knob_1_Value*3 + Knob_2_Value*3)));
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("A");   // send a capital A
    delay(300);
  }
}

void   () {
  traverse.runSpeed ();
  winder.runSpeed ();
  if (traverse.distanceToGo() == 0) {
    traverse.moveTo(goal[idx]); // Change dir between goal positions
    digitalWrite(LED_BUILTIN, idx); // LED off in one direction, LED on in opposite dir
    idx = !idx; //! - logical; !0=1,!1 =0
  }

}
