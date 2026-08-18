# bths scioly robot tour 2025-2026
An autonomous robot developed for BTHS Science Olympiad during the **2025–2026** season.

The system combines odometry, distance-sensing, and careful calibration to navigate a competition course and interact with objects under strict time and dimensional constraints.
#

![Robot Tour robot](assets/robot.png)

## Overview
<u>Robot Tour</u> requires an autonomous vehicle to navigate a predefined course, interact with objects, and complete a sequence of movements accurately within a specified amount of time.

I developed and iterated on the robot's mechanical and software systems throughout the 2024–2025 and 2025–2026 seasons. The 2025–2026 implementation significantly expanded on its original distance-measuring system by **adding a magnetic encoder that directly measured how far the wheels rotated**. Instead of brute-forcing distance based mainly on time, the robot could use the physical circumference of its wheels to calculate how far it had traveled.

- **MT6701 magnetic encoder** — measures wheel rotation for distance estimation
- **MPU6050 IMU** — provides yaw measurements for heading correction
- **Ultrasonic sensor** — measures distance to objects and course features
- **RGB LED** — provides real-time motion and calibration feedback

## Motion Control
### Encoder-Based Movement
The **2024-2025** version mainly used carefully calibrated timing to control how long the motors ran to reach a certain distance. (having us later running into issues of friction and voltage)

For **2025–2026**, I trasitioned to an encoder-based measurement approach.

## Heading Control
The MPU6050 provides a continuous yaw feedback during movement to keep the robot straight (a basic proportional controller)

The controller compares the measured yaw against a target heading:

- **yaw error = target heading − measured heading**

If the robot begins to stray from its target heading, the motor speeds adjust asymmetrically to correct the trajectory. 

The same feedback loop is also used during turns. The robot progressively reduces its turning speed as it approaches a target heading, allowing it to settle within approximately a one-dgree tolerance during calibration.

## Calibration
Because the robot is affected by real-world physical factors, I'm considerating factors, such as robot dimensions, wheel diameter, motor characteristics, battery voltage, surface friction, alignment, and sensor offsets. So therefore I calibrated movement systems around the physical robot.

This repository includes a [`Robot Tour Data Sheet`](Robot-Tour-Data-Sheet.md) documenting measured movements and the corresponding movement commands that we commonly need for competitions.

The calibration data covers:
* Forward movement
* Distance-based approaches
* Distance-based corrections
* 90° turns
* 180° turns
* Positioning relative to course objects

The goal was not simply to make the robot complete one track, but to develop reusable movement primitives that could be calibrated and integrated into different tracks.

## Movement Glossary
| Fuction: | Purpose: |
| :---- | ----- |
| fw(d) | Move forward **d** centimeters |
| bw(d) | Move backward **d** centimeters |
| r(d) | Rotate 90° right |
| l(d) | Rotate 90° left |
| r180(d) | Rotate 180° right |
| l180(d) | Rotate 180° left |
| fu(d) | Move forward until **d** centimeters from an object |
| until(d) | Move backwards until **d** centimeters from an object |

## Example Track

The `Example Track/` directory contains a complete track that won us 3rd at the SOUPS (Science Olympiad at the University of Pennsylvania).

For example:

fu(21.5);<br>
l();<br>
fw(50);<br>
fw(50);<br>
r();<br>
fw(50);<br>
l();<br>
fw(50);<br>
l180();<br>
fw(50);<br>

The corresponding `directions.txt` file provides a human-readable representation of the route before it is inputted into the Arduino program; This separation makes it easier to design and revise routes.

## Development History
### 2024–2025

The original version relied heavily on empirically calibrated timing constants.

For example, forward and backward movement were determined using relationships between:

- movement distance
- motor speed
- elapsed time
- braking behavior

This approach was functional but sensitive to changes in battery voltage, motor behavior, and physical conditions.

### 2025–2026

The movement system was redesigned around measured wheel rotation.

Major improvements included:

- Added MT6701 magnetic encoder
- Replaced primary distance estimation with encoder-based odometry
- Added acceleration/deceleration profiles
- Improved heading correction
- Added dedicated 180° turning routines
- Refined ultrasonic distance-based positioning
- Reorganized movement into reusable high-level primitives
- Added documented physical calibration data

## Hardware

constructed utilizing parts from a [ELEGOO Robot Car Kit](https://www.amazon.com/dp/B07KPZ8RSZ?lv=shuf&channelId=500&plpRedirect=mhFallback) (Arduino UNO, Kit Shield, MPU6050 gyroscoope, Yellow DC gearbox motors), two custom cadded baseplates and claw grips, dowel with a cadded ducky for good luck, ultasonic sensors, 8-pack battery holder (w/ nonrechargable alkaline batteries as per construction parameters), magnetic encoders (MT6701), [DC Buck Converter](https://a.co/d/06xhJ2Bz), 

CADDED on Bambu Studio and printed on Bambu Lab A1 Mini

constructed following the 2025-2026 SciOly build parameters: [Link](https://drive.google.com/file/d/1CcOrLIdCGCBEwNyRmbU7zTlQBbmRYcFV/view)
## Credits
The 2024–2025 implementation was based in part on a kit design and includes code credited to Jason Wei. (CWRU' 29)