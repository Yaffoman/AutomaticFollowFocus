//const int lidar_1;
//const int lidar_2;
//const int lidar_3;
//const int lidar_4;
//const int lidar_5;
//const int lidar_6;

const int stepper_1 = 8;
const int stepper_2 = 9;
const int stepper_3 = 10;
const int stepper_4 = 11;

int step_number = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(stepper_1, OUTPUT);
  pinMode(stepper_2, OUTPUT);
  pinMode(stepper_3, OUTPUT);
  pinMode(stepper_4, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  OneStep(true);
//  Serial.println("At step");
//  Serial.println(step_number);

  delay(2);
}

void OneStep(bool dir){
  if(dir){
    switch(step_number){
//      case 0:
//        digitalWrite(stepper_1, LOW);
//        digitalWrite(stepper_2, HIGH);
//        digitalWrite(stepper_3, HIGH);
//        digitalWrite(stepper_4, HIGH);
//        break;
//      case 1:
//        digitalWrite(stepper_1, HIGH);
//        digitalWrite(stepper_2, LOW);
//        digitalWrite(stepper_3, HIGH);
//        digitalWrite(stepper_4, HIGH);
//        break;
//      case 2:
//        digitalWrite(stepper_1, HIGH);
//        digitalWrite(stepper_2, HIGH);
//        digitalWrite(stepper_3, LOW);
//        digitalWrite(stepper_4, HIGH);
//        break;
//      case 3:
//        digitalWrite(stepper_1, HIGH);
//        digitalWrite(stepper_2, HIGH);
//        digitalWrite(stepper_3, HIGH);
//        digitalWrite(stepper_4, LOW);
//        break;
      
      case 0:
        digitalWrite(stepper_1, HIGH);
        digitalWrite(stepper_2, LOW);
        digitalWrite(stepper_3, LOW);
        digitalWrite(stepper_4, LOW);
        Serial.println("At step");
  Serial.println(step_number);
        break;
      case 1:
        digitalWrite(stepper_1, LOW);
        digitalWrite(stepper_2, HIGH);
        digitalWrite(stepper_3, LOW);
        digitalWrite(stepper_4, LOW);
        Serial.println("At step");
  Serial.println(step_number);
        break;
      case 2:
        digitalWrite(stepper_1, LOW);
        digitalWrite(stepper_2, LOW);
        digitalWrite(stepper_3, HIGH);
        digitalWrite(stepper_4, LOW);
        Serial.println("At step");
  Serial.println(step_number);
        break;
      case 3:
        digitalWrite(stepper_1, LOW);
        digitalWrite(stepper_2, LOW);
        digitalWrite(stepper_3, LOW);
        digitalWrite(stepper_4, HIGH);
        Serial.println("At step");
  Serial.println(step_number);
        break;
    }
  }
  else{
    switch(step_number){
      case 0:
        digitalWrite(stepper_1, LOW);
        digitalWrite(stepper_2, LOW);
        digitalWrite(stepper_3, LOW);
        digitalWrite(stepper_4, HIGH);
        break;
      case 1:
        digitalWrite(stepper_1, LOW);
        digitalWrite(stepper_2, LOW);
        digitalWrite(stepper_3, HIGH);
        digitalWrite(stepper_4, LOW);
        break;
      case 2:
        digitalWrite(stepper_1, LOW);
        digitalWrite(stepper_2, HIGH);
        digitalWrite(stepper_3, LOW);
        digitalWrite(stepper_4, LOW);
        break;
      case 3:
        digitalWrite(stepper_1, HIGH);
        digitalWrite(stepper_2, LOW);
        digitalWrite(stepper_3, LOW);
        digitalWrite(stepper_4, LOW);
        break;
    }
  }
  step_number++;
  step_number = step_number % 4;
}
