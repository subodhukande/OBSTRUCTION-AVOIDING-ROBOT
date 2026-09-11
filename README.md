# Obstacle Avoiding Robot

An autonomous robotic vehicle developed using ESP32, L298N motor driver, HC-SR04 ultrasonic sensor and a servo motor.

The robot detects obstacles in front of it and automatically changes its direction by checking the available space on the left and right sides.

## Components Used

* ESP32 DevKit V1
* L298N Motor Driver
* 4 × DC Geared Motors
* HC-SR04 Ultrasonic Sensor
* Servo Motor
* Battery Supply
* Robot Chassis
* Connecting Wires

## Working Principle

The HC-SR04 ultrasonic sensor continuously measures the distance between the robot and nearby objects.

When the measured distance is greater than the safety distance, the robot continues moving forward.

When an obstacle is detected within 30 cm:

1. The robot stops.
2. It moves backward for a short time.
3. The servo moves the ultrasonic sensor toward the left.
4. The distance on the left side is measured.
5. The servo moves toward the right.
6. The distance on the right side is measured.
7. The ESP32 compares both readings.
8. The robot turns toward the side having more available space.
9. The robot continues moving forward.

If both sides are blocked, the robot performs a longer right turn.

## Pin Configuration

| Component    | ESP32 Pin |
| ------------ | --------- |
| HC-SR04 TRIG | GPIO 5    |
| HC-SR04 ECHO | GPIO 18   |
| Servo Signal | GPIO 14   |
| L298N ENA    | GPIO 13   |
| L298N IN1    | GPIO 26   |
| L298N IN2    | GPIO 27   |
| L298N IN3    | GPIO 25   |
| L298N IN4    | GPIO 33   |
| L298N ENB    | GPIO 32   |

## Software

The program is written in Arduino C/C++ and developed for the ESP32 DevKit V1.

Required library:

* ESP32Servo

## Project Features

* Automatic obstacle detection
* Ultrasonic distance measurement
* Servo-based left and right scanning
* Automatic direction selection
* Four-wheel drive
* L298N motor control
* Serial Monitor status messages

## Repository Structure

## Project Status

Completed prototype of an autonomous obstacle avoiding robot.

## Author

Subodh Ukande
IOT-RS-T-14/JY-10912
B.Tech – Electronics and Communication Engineering
IPS Academy, Indore
RGPV
