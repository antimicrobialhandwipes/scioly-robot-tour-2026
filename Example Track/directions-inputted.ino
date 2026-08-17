#include <Wire.h>
#include <MPU6050_light.h>
#include "FastLED.h"
#include "MT6701.h"

// ======= PINS & SETUP ======
#define STBY 3           // power of both motors; high-on
#define PWMA 5           // power of right motor; high-on
#define PWMB 6           // power of left motor; high-on
#define AIN 7            // direction of right motor; high-forward
#define BIN 8            // direction of left motor; high-forward
#define TRIG 13          // ultrasonic trigger
#define ECHO 12          // ultrasonic echo
#define VOL_MEASURE_PIN A3
#define PIN_RBGLED 4
#define NUM_LEDS 1

// ======= GLOBALS ===========
long duration;
float distance;
double yawTo = 0;         // target yaw angle

// ======= ENCODERS ==========
MT6701 encoder;

const double WHEEL_DIAMETER_CM = 7.22;
const double WHEEL_CIRCUMFERENCE_CM = WHEEL_DIAMETER_CM * PI;
float last_angle = 0;
double total_rotations = 0;

// ======= LED COLORS ========
CRGB leds[NUM_LEDS];
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
    return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

const uint32_t RED = Color(255, 0, 0);
const uint32_t ORANGE = Color(255, 165, 0);
const uint32_t YELLOW = Color(255, 255, 0);
const uint32_t GREEN = Color(0, 255, 0);
const uint32_t BLUE = Color(0, 0, 255);

// ======= MPU6050 ===========
MPU6050 mpu(Wire);

// ======= MOTOR CLASS =======
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
Motor motor1(STBY, AIN, PWMA);  // right side
Motor motor2(STBY, BIN, PWMB);  // left side

// ===========================
// ======= SETUP HELPERS =====
// ===========================
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
    encoder.initializeI2C();
    last_angle = encoder.angleRead();
}

const int target_time = 60;
// note = if the last move is a forward / backward, subtract 1 from NUM_OF_FB
const int NUM_OF_FB = 5;
const int NUM_OF_FU = 5;
const int NUM_OF_LR = 5;
const int NUM_OF_180s = 5;
const int NUM_OF_UN = 5;

void setup() {
    pinSetup();
    check();

    // directions start here:
    fw(30); 
    fw(50);
    fw(50);
    l180();
    fw(45); //
    r();
    fw(50);
    fw(50);
    l();
    fw(50);
    l();
    fw(50);
    bw(50);
    l();
    fw(50);
    r();
    fw(50);
    fu(21.5);
    l();
    fu(21.5);
    l();
    fw(50);
    l();
    fw(50);
    r();
    fw(50);
    r();
    fw(50);
    fw(50);
    bw(50);
    bw(35);
    r();
    fw(50);
    l();
    fw(50);
    fw(50);
    r();
    fw(50);
    bw(45);
    r();
    fw(50);
    l();
    fu(21.5);
    r();
    fw(50);
    fw(50);
    l();
    fw(50);
    fw(50);
    l();
    fw(50);
    l();
    fu(21.5);
    r();
    fu(21.5);
    r();
    fw(50);
    l();
    fw(50);
    l();
    fw(50);
    bw(50);
    l();
    fw(50);
    fw(50);
    fw(50);
    r();
    fw(50);
    fu(21.5);
    r();
    fw(50);
    fu(21.5);
    l();
    fu(25);

    //directions end here

    Serial.println("*END*");
    FastLED.showColor(BLUE);
}

void loop() {
    Serial.println(Voltage_Measure());
}

const int until_time = 1000; // calibrate
const int turn_time = 1000;  // calibrate
const int time_180s = 4500;  // calibrate
const int FU_time = 1600;    // calibrate
//const int FB_time = (((target_time * 1000) - ((NUM_OF_LR * turn_time) + (NUM_OF_FU * FU_time) + (NUM_OF_UN * until_time + (NUM_OF_180s * time_180s)))) / NUM_OF_FB );
const int FB_time = 1500;

const int SPEED = 204;

void check() {
    if (FB_time > 3500 || FU_time > 2500) {
        delay(1000000000);
    }
    if (turn_time > 2000) {
        delay(1000000000);
    }
}

void fw(int d) {
    double timer = millis();
    total_rotations = 0;
    last_angle = encoder.angleRead();

    double accelStart = d * 0.125;
    double decelStart = d * 0.875;
    double minSpeed = SPEED / 3.0;
    double currentDistance = 0;

    while (!(currentDistance >= d && millis() >= timer + FB_time)) {
        currentDistance = updateEncoder() * WHEEL_CIRCUMFERENCE_CM;
        if (currentDistance >= d) {
            motor1.Drive(0);
            motor2.Drive(0);
            continue;
        }

        mpu.update();
        double yaw = mpu.getAngleZ();
        double offset = 0;
        int currentSpeed = SPEED;

        if (currentDistance < accelStart) {
            double scale = currentDistance / accelStart;
            currentSpeed = minSpeed + (SPEED - minSpeed) * (1 - exp(-4 * scale));
        }
        if (currentDistance > decelStart) {
            double scale = (d - currentDistance) / (d - decelStart);
            currentSpeed = minSpeed + (SPEED - minSpeed) * exp(-4 * (1 - scale));
        }

        if (yaw <= yawTo + 1.5 && yaw >= yawTo - 1) {
            motor1.Drive(currentSpeed);
            motor2.Drive(currentSpeed + offset);
            FastLED.showColor(GREEN);
        }else if (yaw < yawTo - 1) {
            motor1.Drive(currentSpeed);
            motor2.Drive(currentSpeed - 30 + offset);
            FastLED.showColor(RED);
        }else {
            motor1.Drive(currentSpeed - 30);
            motor2.Drive(currentSpeed + offset);
            FastLED.showColor(RED);
        }
    }

    motor1.Drive(0);
    motor2.Drive(0);
}

void bw(int d) {
    double timer = millis();
    total_rotations = 0;
    last_angle = encoder.angleRead();

    double accelStart = d * 0.125;
    double decelStart = d * 0.875;
    double minSpeed = SPEED / 3.0;
    double currentDistance = 0;

    while (!(abs(currentDistance) >= d && millis() >= timer + FB_time)) {
        currentDistance = updateEncoder() * WHEEL_CIRCUMFERENCE_CM;
        if (abs(currentDistance) >= d) {
            motor1.Drive(0);
            motor2.Drive(0);
            continue;
        }

        mpu.update();
        double yaw = mpu.getAngleZ();
        double offset = 0;
        int currentSpeed = SPEED;

        if (abs(currentDistance) < accelStart) {
            double scale = abs(currentDistance) / accelStart;
            currentSpeed = minSpeed + (SPEED - minSpeed) * (1 - exp(-4 * scale));
        }
        if (abs(currentDistance) > decelStart) {
            double scale = (d - abs(currentDistance)) / (d - decelStart);
            currentSpeed = minSpeed + (SPEED - minSpeed) * exp(-4 * (1 - scale));
        }

        if (yaw <= yawTo + 1.5 && yaw >= yawTo - 1) {
            motor1.Drive(-currentSpeed);
            motor2.Drive(-currentSpeed - offset);
            FastLED.showColor(GREEN);
        }else if (yaw < yawTo - 1) {
            motor1.Drive(-currentSpeed + 30);
            motor2.Drive(-currentSpeed - offset);
            FastLED.showColor(RED);
        }else {
            motor1.Drive(-currentSpeed);
            motor2.Drive(-currentSpeed + 30 - offset);
            FastLED.showColor(RED);
        }
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

double updateEncoder() {
    float current_angle = encoder.angleRead();
    float delta = current_angle - last_angle;
    if (delta > 180.0) delta -= 360.0;
    else if (delta < -180.0) delta += 360.0;
    total_rotations += delta / 360.0;
    last_angle = current_angle;
    return total_rotations;
}

void r() {
    yawTo -= 90;
    calibrate();
}

void l() {
    yawTo += 90;
    calibrate();
}

void r180() {
    yawTo -= 180;
    double timer = millis();
    while (!(abs(mpu.getAngleZ() - yawTo) <= 1 && millis() >= timer + time_180s)) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        double error = yawTo - yaw;
        if (abs(error) <= 1) {
            motor1.Drive(0);
            motor2.Drive(0);
            FastLED.showColor(GREEN);
            continue;
        }

        int speed = 60;
        if (error > 0) {
            motor1.Drive(speed);
            motor2.Drive(-speed);
        } else {
            motor1.Drive(-speed);
            motor2.Drive(speed);
        }
        FastLED.showColor(RED);
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

void l180() {
    yawTo += 180;
    double timer = millis();
    while (!(abs(mpu.getAngleZ() - yawTo) <= 1 && millis() >= timer + time_180s)) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        double error = yawTo - yaw;
        if (abs(error) <= 1) {
            motor1.Drive(0);
            motor2.Drive(0);
            FastLED.showColor(GREEN);
            continue;
        }
        int speed = 60;
        if (error > 0) {
            motor1.Drive(speed);
            motor2.Drive(-speed);
        } else {
            motor1.Drive(-speed);
            motor2.Drive(speed);
        }
        FastLED.showColor(RED);
    }
    motor1.Drive(0);
    motor2.Drive(0);
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
                speed = 90;
                FastLED.showColor(ORANGE);
            } else if (yaw > yawTo + 10) {
                speed = -90;
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

void fu(double amount) {
    double timer = millis();
    amount = amount - 5.5;

    while (millis() < timer + FU_time) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        double dist = get_distance();
        int speed;
        if (dist > amount + 20) {
            speed = 180;
            FastLED.showColor(RED);
        }else if (dist > amount + 10) {
            speed = 150;
            FastLED.showColor(ORANGE);
        }else if (dist > amount + 5) {
            speed = 80;
            FastLED.showColor(YELLOW);
        }else {
            motor1.Drive(0);
            motor2.Drive(0);
            FastLED.showColor(GREEN);
            continue;
        }

        if (yaw <= yawTo + 1 && yaw >= yawTo - 1) {
            motor1.Drive(speed);
            motor2.Drive(speed);
        }else if (yaw < yawTo - 1) {
            motor1.Drive(speed);
            motor2.Drive(speed - 30);
        }else {
            motor1.Drive(speed - 30);
            motor2.Drive(speed);
        }
    }
    motor1.Drive(0);
    motor2.Drive(0);
}

void until(double amount) {
    double timer = millis();
    int offset = 0;
    amount = amount - 2.75;
    while (millis() < timer + until_time) {
        mpu.update();
        double yaw = mpu.getAngleZ();
        int speed = 80;
        if (get_distance() >= amount - 1 && get_distance() <= amount + 1) {
            motor1.Drive(0);
            motor2.Drive(0);
        } else if (get_distance() > amount) {
            if (yaw <= yawTo + 1 && yaw >= yawTo - 1) {
                motor1.Drive(speed);
                motor2.Drive(speed + offset);
            } else if (yaw < yawTo - 1) {
                motor1.Drive(speed);
                motor2.Drive(speed - 30 + offset);
            } else if (yaw > yawTo + 1) {
                motor1.Drive(speed - 30);
                motor2.Drive(speed + offset);
            }
        } else if (get_distance() < amount){
            if (yaw <= yawTo + 1 && yaw >= yawTo - 1) {
                motor1.Drive(-speed);
                motor2.Drive(-speed - offset);
                FastLED.showColor(GREEN);
            } else if (yaw < yawTo - 1) {
                motor1.Drive(-speed + 30);
                motor2.Drive(-speed - offset);
                FastLED.showColor(RED);
            } else if (yaw > yawTo + 1) {
                motor1.Drive(-speed);
                motor2.Drive(-speed + 30 - offset);
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