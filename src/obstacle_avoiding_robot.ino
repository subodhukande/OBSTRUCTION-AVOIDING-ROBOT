#include <ESP32Servo.h>

// Ultrasonic sensor pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// Servo motor
#define SERVO_PIN 14

// L298N motor driver pins
#define ENA 13
#define IN1 26
#define IN2 27

#define IN3 25
#define IN4 33
#define ENB 32

Servo scannerServo;

// Motor PWM settings
const int PWM_FREQ = 1000;
const int PWM_RESOLUTION = 8;

const int LEFT_CHANNEL = 0;
const int RIGHT_CHANNEL = 1;

// Robot speed settings
int motorSpeed = 180;
int turnSpeed = 190;

// Robot stops and checks when an object is closer than this
const int SAFE_DISTANCE = 30;

// Timing values
const int REVERSE_TIME = 300;
const int TURN_TIME = 500;

// Servo positions
const int CENTER = 90;
const int LEFT = 150;
const int RIGHT = 30;


// --------------------------------------------------
// Function declarations
// --------------------------------------------------

float getDistance();
float getStableDistance();

void moveForward();
void moveBackward();
void stopRobot();

void turnLeft();
void turnRight();

float lookLeft();
float lookRight();

void avoidObstacle();


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(115200);

  // Ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Motor direction pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Set up PWM for the two sides of the robot
  ledcSetup(LEFT_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(RIGHT_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

  ledcAttachPin(ENA, LEFT_CHANNEL);
  ledcAttachPin(ENB, RIGHT_CHANNEL);

  // Servo setup
  scannerServo.setPeriodHertz(50);
  scannerServo.attach(SERVO_PIN, 500, 2400);
  scannerServo.write(CENTER);

  stopRobot();

  Serial.println();
  Serial.println("Obstacle Avoiding Robot");
  Serial.println("Initializing...");

  delay(1500);

  Serial.println("Robot ready.");
  Serial.println("Starting automatic mode.");

  scannerServo.write(CENTER);
  delay(500);
}


// --------------------------------------------------
// Main loop
// --------------------------------------------------

void loop()
{
  float distance = getStableDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= SAFE_DISTANCE)
  {
    Serial.println("Obstacle detected.");
    avoidObstacle();
  }
  else
  {
    moveForward();
  }

  delay(50);
}


// --------------------------------------------------
// Read distance from HC-SR04
// --------------------------------------------------

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // If there is no echo, assume there is no nearby obstacle
  if (duration == 0)
  {
    return 400;
  }

  float distance = duration * 0.0343 / 2.0;

  if (distance <= 0 || distance > 400)
  {
    return 400;
  }

  return distance;
}


// Take a few readings to make the distance measurement more stable
float getStableDistance()
{
  const int samples = 3;
  float total = 0;

  for (int i = 0; i < samples; i++)
  {
    total += getDistance();
    delay(10);
  }

  return total / samples;
}


// --------------------------------------------------
// Motor control
// --------------------------------------------------

void moveForward()
{
  // Left side
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right side
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(LEFT_CHANNEL, motorSpeed);
  ledcWrite(RIGHT_CHANNEL, motorSpeed);
}


void moveBackward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(LEFT_CHANNEL, motorSpeed);
  ledcWrite(RIGHT_CHANNEL, motorSpeed);
}


void stopRobot()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  ledcWrite(LEFT_CHANNEL, 0);
  ledcWrite(RIGHT_CHANNEL, 0);
}


void turnLeft()
{
  // Left wheels move backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Right wheels move forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(LEFT_CHANNEL, turnSpeed);
  ledcWrite(RIGHT_CHANNEL, turnSpeed);
}


void turnRight()
{
  // Left wheels move forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right wheels move backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(LEFT_CHANNEL, turnSpeed);
  ledcWrite(RIGHT_CHANNEL, turnSpeed);
}


// --------------------------------------------------
// Servo scanning
// --------------------------------------------------

float lookLeft()
{
  scannerServo.write(LEFT);
  delay(450);

  float distance = getStableDistance();

  Serial.print("Left: ");
  Serial.print(distance);
  Serial.println(" cm");

  scannerServo.write(CENTER);
  delay(250);

  return distance;
}


float lookRight()
{
  scannerServo.write(RIGHT);
  delay(450);

  float distance = getStableDistance();

  Serial.print("Right: ");
  Serial.print(distance);
  Serial.println(" cm");

  scannerServo.write(CENTER);
  delay(250);

  return distance;
}


// --------------------------------------------------
// Obstacle avoidance
// --------------------------------------------------

void avoidObstacle()
{
  // First stop the robot
  stopRobot();
  delay(250);

  // Move backward a little to create some space
  Serial.println("Moving backward...");
  moveBackward();
  delay(REVERSE_TIME);

  stopRobot();
  delay(250);

  // Check the left side
  Serial.println("Checking left side...");
  float leftDistance = lookLeft();

  // Check the right side
  Serial.println("Checking right side...");
  float rightDistance = lookRight();

  Serial.print("Left = ");
  Serial.print(leftDistance);
  Serial.print(" cm, Right = ");
  Serial.print(rightDistance);
  Serial.println(" cm");

  // Choose the side having more free space
  if (leftDistance > rightDistance &&
      leftDistance > SAFE_DISTANCE)
  {
    Serial.println("Turning left.");

    turnLeft();
    delay(TURN_TIME);
  }
  else if (rightDistance > leftDistance &&
           rightDistance > SAFE_DISTANCE)
  {
    Serial.println("Turning right.");

    turnRight();
    delay(TURN_TIME);
  }
  else
  {
    // If both sides are blocked, make a larger right turn
    Serial.println("Both sides blocked.");
    Serial.println("Making a longer right turn.");

    turnRight();
    delay(TURN_TIME + 250);
  }

  stopRobot();
  delay(150);

  scannerServo.write(CENTER);

  Serial.println("Continuing forward.");
  Serial.println();
}
