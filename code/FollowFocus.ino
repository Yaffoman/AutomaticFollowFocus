#include <SoftwareSerial.h>
SoftwareSerial Serial1(2,3);

#include <Stepper.h>
#define STEPS 2038 // the number of steps in one revolution of your motor (28BYJ-48)

// Auto stepper
Stepper stepper(STEPS, 8, 10, 9, 11);
int rpm = 6;
int dir = 1;

// Manual stepper code
const int stepper_1 = 8;
const int stepper_2 = 9;
const int stepper_3 = 10;
const int stepper_4 = 11;
int step_number = 0;

// For testing
int stepsToComplete = 0;
int fineIncrement = 10;
int normalIncrement = 100;
bool motorOn = false;
bool debug;

// For LiDAR
int dist = 0;
int check; //save check value
int i;
int uart[9]; //save data measured by LiDAR
const int HEADER=0x59; //frame header of data package
bool printLiDAR = false;

// For automatic focusing
bool autofocus = false;
int currentStep = 0;

void setup() {
  
  // Set up motor
  pinMode(stepper_1, OUTPUT);
  pinMode(stepper_2, OUTPUT);
  pinMode(stepper_3, OUTPUT);
  pinMode(stepper_4, OUTPUT);

  // Set up LiDAR
  Serial1.begin(115200);

  // Set up Arduino
  Serial.begin(9600);

  // Set up motor
  resetMotor();
}

void loop() {

  systemControl();
  readLidar();
  if(autofocus) {
    driveAuto();
  } else {
    driveManual();
    if(printLiDAR) {
      Serial.print("Target Distance: ");
      Serial.println(dist);
    }
  }
}

/*
 * Takes in input from I/O and sets the amount of steps the motor needs to take.
 * 'W' increases the step count, 'S' decreases it, and 'R' resets it. Also reads
 * in control for LiDAR (whether it should print). 
 */
void systemControl() {

  if(Serial.available()) {
    char input = Serial.read();  
    switch(input) {
      case 'w' :      
        stepsToComplete += fineIncrement;
        Serial.print("--- Increasing Steps to: ");
        Serial.println(stepsToComplete);
        break;
      case 's':
        stepsToComplete -= fineIncrement;
        Serial.print("--- Decreasing Steps to: ");
        Serial.println(stepsToComplete);
        break;
      case 'W' :      
        stepsToComplete += normalIncrement;
        Serial.print("--- Increasing Steps to: ");
        Serial.println(stepsToComplete);
        break;
      case 'S':
        stepsToComplete -= normalIncrement;
        Serial.print("--- Decreasing Steps to: ");
        Serial.println(stepsToComplete);
        break;
      case 'r':
        resetMotor();
        autofocus = false;
        stepsToComplete = 0;
        Serial.println("--- Resetting Steps");
        break;
      case 'l':
        printLiDAR = !printLiDAR;
        Serial.print("--- Print LiDAR reading: ");
        Serial.println(printLiDAR);
        break;
      case '1':
        rpm--;
        stepper.setSpeed(rpm);
        Serial.print("--- Decreasing RPM to: ");
        Serial.println(rpm);
        break;
      case '2':
        rpm++;
        stepper.setSpeed(rpm);
        Serial.print("--- Increasing RPM to: ");
        Serial.println(rpm);
        break;
      case 'd':
        motorOn = !motorOn;
        Serial.print("--- Motor ");
        Serial.println(motorOn ? "On" : "Off");
        break;
      case 'a':
        dir = -dir;
        break;
      case 'o':
        autofocus = !autofocus;
        Serial.print("--- Autofocus ");
        Serial.println(autofocus ? "On" : "Off");
        break;
      case 'q':
        autofocus = false;
        setMotorLow();
        Serial.println("--- Setting Motor Low");
        break;
      case 'v':
        autofocus = false;
        stepsToComplete = -getTarget(dist);
        motorOn = true;
        break;
      case 'm':
        debug = !debug;
    }
  }
}

/*
 * Receives LiDAR reading and sets 'dist' value. If printLiDAR, then also
 * prints LiDAR distance reading to serial.
 */
void readLidar() {
  
  if (Serial1.available()) { // Check if serial port has data input
    if(Serial1.read() == HEADER) { // Assess data package frame header 0x59
      uart[0]=HEADER;
      if (Serial1.read() == HEADER) { // Assess data package frame header 0x59
        uart[1] = HEADER;
        for (i = 2; i < 9; i++) { // Save data in array
          uart[i] = Serial1.read();
        }
        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        if (uart[8] == (check & 0xff)) { // Verify the received data as per protocol
          int temp = uart[2] + uart[3] * 256; // Calculate & output distance
          if (temp <= 500 && temp >= 0)
            dist = temp;
            
        }
      }
    }
  }
}

void resetMotor() {
  
  // Bring motor back to start state
  // stepper.setSpeed(6);
  // stepper.step(1600);
  currentStep = 0;
  setMotorLow();

  // Set motor to desired speed
  stepper.setSpeed(rpm);
}

void setMotorLow() {

  digitalWrite(stepper_1, LOW);
  digitalWrite(stepper_2, LOW);
  digitalWrite(stepper_3, LOW);
  digitalWrite(stepper_4, LOW);
}

void driveManual() {

  // Do nothing if the motor is not on
  if(!motorOn) return;

  // If there are steps to complete, drive motor
  if(stepsToComplete != 0) {
    int dir = stepsToComplete > 0 ? -1 : 1;
    stepper.step(dir);
    stepsToComplete += dir;
  } else {
    motorOn = false;
    setMotorLow();
  }
}

void driveAuto() {

  // Number of steps to turn
  int targetStep = getTarget(dist);
  int deltaSteps = targetStep - currentStep;
  if(debug){
    Serial.print("target: ");
    Serial.println(targetStep);
    Serial.print("current: ");
    Serial.println(currentStep);
  }
  
  // If there are steps to complete, drive motor
  if(abs(deltaSteps) >= 1/double(dist) * 1000) {
    while (abs(deltaSteps) > 0) {
      int dir = deltaSteps > 0 ? 1 : -1;
      stepper.step(dir);
      currentStep += dir;
      deltaSteps = targetStep - currentStep;
    }
  } else {
    setMotorLow();
  }
}

double getTarget(double dist) {
   
  double numSteps = dist < 200 ? distFunctionClose(dist) : distFunctionFar(dist);
  return clamp(numSteps, 0, 1600);
}

double distFunctionClose(double x) {
  return -1267 + (41.1 * x) + (-0.269 * pow(x, 2)) + (0.000597 * pow(x, 3));
//  return -2052 + (76 * x) + (-0.788 * pow(x, 2)) + (0.00374 * pow(x, 3)) + (-0.00000663 * pow(x, 4));
}

double distFunctionFar(double x) {
  return 414 + (3.86 * x) + (-.00624 * pow(x, 2));
}

double clamp(double x, double minimum, double maximum) {
  return max(min(maximum, x), minimum);
}

double powd(double value, double exp) {
  double output = 1;
  for(int i = 0; i < exp; i++) {
    output *= value;
  }
  return output;
}

double maxd(double val1, double val2) {
  if(val1 > val2) {
    return val1;
  }
  return val2;
}

double mind(double val1, double val2) {
  if(val1 < val2) {
    return val1;
  }
  return val2;
}
