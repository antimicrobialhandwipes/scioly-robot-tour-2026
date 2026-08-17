//courtesy of Jason Wei (CWRU '29)

#include <Wire.h>
#include <MPU6050_light.h>
MPU6050 mpu(Wire);
#define STBY 3  //power of both motors;     high-on
#define PWMA 5  //power of right motor;     high-on
#define PWMB 6  //power of left motor;      high-on
#define AIN 7   //direction of right motor; high-forward
#define BIN 8   //direction of left motor;  high-forward
#define TRIG 13
#define ECHO 12
#define VOL_MEASURE_PIN A3

long duration;
float distance;
double yawTo = 0; // getanglez

#include "FastLED.h"
#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
uint32_t Color(uint8_t r, uint8_t g, uint8_t b){
    return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}
const uint32_t RED = Color(255, 0, 0);
const uint32_t ORANGE = Color(255, 165, 0);
const uint32_t YELLOW = Color(255, 255, 0);
const uint32_t GREEN = Color(0, 255, 0);
const uint32_t BLUE = Color(0, 0, 255);

class Motor {
    int stbyPin;
    int inPin;
    int pwmPin;
public:
    Motor(int stbyPin, int inPin, int pwmPin) {
        this->stbyPin = stbyPin;
        this->inPin = inPin;
        this->pwmPin = pwmPin;
    }
    void Drive(int speed) {
        digitalWrite(inPin, speed > 0);
        speed = abs(speed);
        analogWrite(pwmPin, speed);
    }
};
Motor motor1(STBY, AIN, PWMA);  // right motor
Motor motor2(STBY, BIN, PWMB);  // left motor

void pinSetup() {
    pinMode(AIN, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(BIN, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(STBY, OUTPUT);
    digitalWrite(STBY, HIGH);
    pinMode(ECHO, INPUT);
    pinMode(TRIG, OUTPUT);
    pinMode(VOL_MEASURE_PIN, INPUT);
    FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
    FastLED.setBrightness(1);
    FastLED.showColor(ORANGE);
    Serial.begin(9600);
    Wire.begin();
    mpu.begin();
    delay(1000);
    mpu.calcGyroOffsets();
}

void setup() {
    pinSetup();
    check();
    Serial.println("*END*");
    FastLED.showColor(BLUE);
}

void loop() {
    Serial.println(Voltage_Measure());
    /*
    TIME LIMIT: __000 milliseconds
    numForwardBackward = ___
    numberLeftRight = ___
    totalDistance = ___
    timeForwardBackward = totalDistance*21 + brakeTime*numForwardBackward
    timeLeftRight = numberLeftRight * 445 + brakeTime*numberLeftRight
              = numberLeftRight (445 + brakeTime)
    TIME LIMIT = timeForwardBackward + timeLeftRight =
    SOLVE for brake constant
    */








    /* Time without forwardUntil:
    TimeLimit: __000 milliseconds
    FB_time = (TimeLimit - numberLeftRight * 1000) / numberForwardBackward
    */








    // time with forwardUntil: may have to subtract an additional (numberForwardUntil * 1600) from the time limit
}

const int FB_time = 1622;
const int FU_time = 1600;
const int turn_time = 1000;

void check() {
    if (FB_time > 3500 || FU_time > 2500) {
        delay(1000000000);
    }
    if (turn_time > 2000) {
        delay(1000000000);
    }
}

// MOTOR SPEED
const int SPEED = 170;
const double FB_CONSTANT = 29;

void forward(int d) {
    double timer = millis();
    while (millis() < timer + FB_time || millis() < timer + d * FB_CONSTANT) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        if (millis() >= timer + d * FB_CONSTANT) {
            motor1.Drive(0);
            motor2.Drive(0);
        } else {
            if (yaw <= yawTo + 1.5 && yaw >= yawTo - 1) {
                motor1.Drive(SPEED);
                motor2.Drive(SPEED);
                FastLED.showColor(GREEN);
            } else if (yaw < yawTo - 1) {
                motor1.Drive(SPEED);
                motor2.Drive(SPEED - 30);
                FastLED.showColor(RED);
            } else if (yaw > yawTo + 1) {
                motor1.Drive(SPEED - 30);
                motor2.Drive(SPEED);
                FastLED.showColor(RED);
            }
        }
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

void backward(int d) {
    double timer = millis();
    while (millis() < timer + FB_time || millis() < timer + d * FB_CONSTANT) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        Serial.println(yaw);
        if (millis() >= timer + d * FB_CONSTANT) {
            motor1.Drive(0);
            motor2.Drive(0);
        } else {
            if (yaw <= yawTo + 1 && yaw >= yawTo - 1) {
                motor1.Drive(-SPEED);
                motor2.Drive(-SPEED);
                FastLED.showColor(GREEN);
            } else if (yaw < yawTo - 1) {
                motor1.Drive(-SPEED + 30);
                motor2.Drive(-SPEED);
                FastLED.showColor(RED);
            } else if (yaw > yawTo + 1) {
                motor1.Drive(-SPEED);
                motor2.Drive(-SPEED + 30);
                FastLED.showColor(RED);
            }
        }
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

void right() {
    yawTo -= 90;
    calibrate();
}

void left() {
    yawTo += 90;
    calibrate();
}

void calibrate() {
    double timer = millis();
    while (millis() < timer + turn_time) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        Serial.println(yaw);
        if (abs(yaw - yawTo) <= 1) {
            motor1.Drive(0);
            motor2.Drive(0);
            FastLED.showColor(GREEN);
        } else {
            int speed = 0;
            if (yaw < yawTo - 30) {
                speed = 170;
                FastLED.showColor(RED);
            } else if (yaw > yawTo + 30) {
                speed = -170;
                FastLED.showColor(RED);
            } else if (yaw < yawTo - 10) {
                speed = 140;
                FastLED.showColor(ORANGE);
            } else if (yaw > yawTo + 10) {
                speed = -140;
                FastLED.showColor(ORANGE);
            } else if (yaw < yawTo - 1) {
                speed = 50;
                FastLED.showColor(YELLOW);
            } else if (yaw > yawTo + 1) {
                speed = -50;
                FastLED.showColor(YELLOW);
            }
            motor1.Drive(speed);
            motor2.Drive(-speed);
        }
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

void forwardUntil(double amount) {
    double timer = millis();
    while (millis() < timer + FU_time) { // we may have to do 1600 * the total number of forward untils
        mpu.update();
        int speed;
        double yaw = mpu.getAngleZ();
        if (get_distance() > amount + 20) {
            speed = 180;
            FastLED.showColor(RED);
        } else if (get_distance() > amount + 10) {
            speed = 150;
            FastLED.showColor(ORANGE);
        } else if (get_distance() > amount + 5) {
            speed = 80;
            FastLED.showColor(YELLOW);
        } else {
            speed = 0;
            FastLED.showColor(GREEN);
        }
       
        if (yaw <= yawTo + 1 && yaw >= yawTo - 1) {
            motor1.Drive(speed);
            motor2.Drive(speed);
        } else if (yaw < yawTo - 1) {
            motor1.Drive(speed);
            motor2.Drive(speed - 30);
        } else if (yaw > yawTo + 1) {
            motor1.Drive(speed - 30);
            motor2.Drive(speed);
        }
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

void until(double amount) {
    double timer = millis();
    while (millis() < timer + 1000) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        int speed = 80;
        if (get_distance() >= amount - 1 && get_distance() <= amount + 1) {
            motor1.Drive(0);
            motor2.Drive(0);
        } else if (get_distance() > amount) {
            if (yaw <= yawTo + 1 && yaw >= yawTo - 1) {
                motor1.Drive(speed);
                motor2.Drive(speed);
            } else if (yaw < yawTo - 1) {
                motor1.Drive(speed);
                motor2.Drive(speed - 30);
            } else if (yaw > yawTo + 1) {
                motor1.Drive(speed - 30);
                motor2.Drive(speed);
            }
        } else if (get_distance() < amount){
            if (yaw <= yawTo + 1 && yaw >= yawTo - 1) {
                motor1.Drive(-speed);
                motor2.Drive(-speed);
                FastLED.showColor(GREEN);
            } else if (yaw < yawTo - 1) {
                motor1.Drive(-speed + 30);
                motor2.Drive(-speed);
                FastLED.showColor(RED);
            } else if (yaw > yawTo + 1) {
                motor1.Drive(-speed);
                motor2.Drive(-speed + 30);
                FastLED.showColor(RED);
            }
        } else {
            motor1.Drive(0);
            motor2.Drive(0);
            FastLED.showColor(GREEN);
        }
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

double get_distance() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = duration * 0.034 / 2;
    Serial.println(distance - 2);
    if (distance > 150) {
        distance = 0;
    }
    return distance - 2;
}

double Voltage_Measure() {
    double voltage = (analogRead(VOL_MEASURE_PIN) * 5 ) * ((10 + 1.5) / 1.5)/ 1024;
    return voltage + (voltage * 0.08);
}

double brake_constant(double numFB, double numLR, double target_time) {
  double constant = ((target_time * 1000) - (numFB * 50 * 21) - (numLR * 445)) / (numFB + numLR);
  return constant;
}

// ((target_time * 1000) - (numFB * 50 * 21) - (numLR * 445)) / (numFB + numLR)
// ((20000) - (10 * 50) - (10 * 445)) / (10 + 4);