/* Arduino mobile robot utilizing ultrasonic sensor to detect distance and servo launcher to launch projectile when in range.  */
#include <Servo.h>

// PIN DEFINITIONS
// Ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

// (L298N) motor driver 
const int motor1pin1 = 2;    
const int motor1pin2 = 3;    
const int ENA = 5; // PWM pin for motor speed

// Launcher 
const int servoPin = 11;

// CONSTANTS
#define OBSTACLE_DISTANCE 15 // Stop within 15cm of line
#define LAUNCH_DISTANCE 30 // Launch when within 30cm of target
#define SLOWDOWN_DISTANCE 50
#define SERVO_ARMED   0 // Holding trash
#define SERVO_LAUNCH  90 // Release trash. 
#define SPEED_STOP    0
#define SPEED_SLOW    150
#define SPEED_MEDIUM  200
#define SPEED_FAST    255

#define RETURN_TIME 3000
long duration;
float distance;
bool hasLaunched = false;
bool missionComplete = false;

Servo launcher; 

void setup() {
  
  // Configure sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Configure motor pins
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(ENA, OUTPUT);
  
launcher.attach(servoPin);
launcher.write(SERVO_ARMED);  

delay(1000);
}

void loop() {
  distance = measureDistance();

if (!hasLaunched) {
  if (distance < OBSTACLE_DISTANCE) {
    stopMotor(); 
    
  } else if (distance < LAUNCH_DISTANCE) {
    // In launch range and haven't launched yet
    stopMotor();
    delay(500); 
    launchProjectile();
    hasLaunched = true;  // Don't launch again
    delay (1000);

    } else if (distance < SLOWDOWN_DISTANCE) {
      // close
      moveForward(SPEED_SLOW);
      
    } else {
      // Far away
      moveForward(SPEED_FAST);
    }
  } 

  else if (hasLaunched && !missionComplete) {
    // Travel back
    moveBackward(SPEED_MEDIUM);
    delay(RETURN_TIME);
    
    stopMotor();
    missionComplete = true;
  }
    else {
    stopMotor();
  }
  
  delay(100); 
} 
//sensor function
float measureDistance() {
  // Send ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echo
  duration = pulseIn(echoPin, HIGH);
  
  // Convert to cm
  return duration * 0.034 / 2;
}
// motor functions

void moveForward(int speed) {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  analogWrite(ENA, speed);
}
void moveBackward(int speed) {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  analogWrite(ENA, speed);
}

void stopMotor() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  analogWrite(ENA, 0);
}

// launcher functions

void launchProjectile() {
  launcher.write(SERVO_ARMED);
  delay(500);
  
  launcher.write(SERVO_LAUNCH);
  delay(800);
  
  launcher.write(SERVO_ARMED);
  delay(500);
}
