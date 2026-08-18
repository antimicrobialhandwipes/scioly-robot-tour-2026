# bths scioly robot tour 2025-2026
An autonomous robot developed for BTHS Science Olympiad'ss Robot Tour during the **2025–2026** season.

The system combines  odometry, distance sensing, and careful calibration to navigate a competition course and interact with objects under strict time and dimensional constraints.
#

![robot.png](assets\robot.png)

## Overview
<u>Robot Tour</u> requires an autonomous vehicle to navigate a predefined course, interact with objects, and complete a sequence of movements accurately within a specified amount of time.

I developed and iterated on the robot's mechanical and software systems throughout the 2024–2025 and 2025–2026 seasons. The 2025–2026 implementation significantly expanded its original brute-forced measurement approach by **introducing direct wheel-rotation measurement through a magnetic encoder**, basically using the circumference of the physical wheel to measure distance rather tha relying solely on just time.

- **MT6701 magnetic encoder** — measures wheel rotation for distance estimation
- **MPU6050 IMU** — provides yaw measurements for heading correction
- **Ultrasonic sensor** — measures distance to objects and course features
- **RGB LED** — provides real-time motion and calibration feedback

## Motion Control
### Encoder-Based Movement
The **2024-2025** implementation primarily relied on carefully calibrated timing constants to determine how long the motors should run. (running into issues of friction and voltage)

For **2025–2026**, I trasitioned to forward and backward movement with encoder-based distance measurement.

## Heading Control
MPU6050 provides a continuous yaw feedback during movement to keep the robot straight (a basic proportional controller)

The controller compares the measured yaw against a target heading:

&emsp; **yaw error = target heading − measured heading**

If the robot begins to deviate from it target heading, the motor speeds adjust to correct the trajectory. 

The same feedback loop is also used during turns. The robot progressively reduces its turning speed as it approaches a target heading, allowing it to settle within approximately a one-dgree tolerance during calibration.

## Calibration
Because the robot operates under physical constraints 
<!--
**constructed following the 2025-2026 SciOly build parameters:** [Link](https://drive.google.com/file/d/1CcOrLIdCGCBEwNyRmbU7zTlQBbmRYcFV/view)

constructed utilizing parts from a [ELEGOO Robot Car Kit](https://www.amazon.com/dp/B07KPZ8RSZ?lv=shuf&channelId=500&plpRedirect=mhFallback) (Arduino UNO, Kit Shield, MPU6050 gyroscoope, Yellow DC gearbox motors), two custom cadded baseplates and claw grips, dowel with a cadded ducky for good luck, ultasonic sensors, 8-pack battery holder (w/ nonrechargable alkaline batteries as per construction parameters), magnetic encoders (MT6701), [DC Buck Converter](https://a.co/d/06xhJ2Bz), 

CADDED on Bambu Studio and printed on Bambu Lab A1 Mini
-->
 