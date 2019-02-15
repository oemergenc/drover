#include <Arduino.h>
#include <TimerOne.h>
#include <Arduino.h>
#include <AFMotor.h>


#define LOOP_TIME 1000000
#define left_encoder_pin 20
#define right_encoder_pin 21

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

const float stepcount = 10.00;  // 10 Slots in disk, change if different
volatile int counter_left = 0;
volatile int counter_right = 0;
long start = 0;
char command;
int speed = 0;

volatile int rpmLeft = 0;
volatile int rpmRight = 0;

void docount_left()  // counts from the speed sensor
{
    counter_left++;  // increase +1 the counter value
}

void docount_right()  // counts from the speed sensor
{
    counter_right++;  // increase +1 the counter value
}

void timerIsr()
{
    Timer1.detachInterrupt();  //stop the timer
    rpmLeft = (counter_left / stepcount) * 60.00;  // calculate RPM for Motor 1
    rpmRight = (counter_right / stepcount) * 60.00;  // calculate RPM for Motor 2
    counter_right = 0;
    counter_left = 0;
    Timer1.attachInterrupt(timerIsr);  //enable the timer
}

void setSpeed(int speed)
{
    motor1.setSpeed(speed);
    motor2.setSpeed(speed);
    motor3.setSpeed(speed);
    motor4.setSpeed(speed);
}

void setup()
{
    Serial.begin(9600);
    //Setup for encoders
    pinMode(right_encoder_pin, INPUT_PULLUP);
    pinMode(left_encoder_pin, INPUT_PULLUP);

//    Timer1.initialize(LOOP_TIME);
    attachInterrupt(digitalPinToInterrupt(left_encoder_pin), docount_left,
                    CHANGE);  // increase counter when speed sensor pin goes High
    attachInterrupt(digitalPinToInterrupt(right_encoder_pin), docount_right,
                    CHANGE);  // increase counter when speed sensor pin goes High
//    Timer1.attachInterrupt(timerIsr); // enable the timer
}

void loop()
{
    motor1.run(BACKWARD); // and change this function to .run(FORWARD)
    motor2.run(FORWARD); // and change this function to .run(FORWARD)
    motor3.run(FORWARD); // and change this function to .run(FORWARD)
    motor4.run(BACKWARD); // and change this function to .run(FORWARD)
    if (millis() - start >= 1000)
    {

        rpmLeft = (counter_left / stepcount) * 60;  // calculate RPM for Motor 1
        rpmRight = (counter_right / stepcount) * 60;  // calculate RPM for Motor 2
//        Serial.println("left");
//        Serial.println("right");
        Serial.print("Motor Speed 1: ");
        Serial.print(counter_left);
        Serial.print(" Counts, ");
        Serial.print(rpmLeft);
        Serial.print(" RPM - ");
        Serial.print("Motor Speed 2: ");
        Serial.print(counter_right);
        Serial.print(" Counts, ");
        Serial.print(rpmRight);
        Serial.print(" RPM, ");
        Serial.print(speed);
        Serial.println(" Speed");
        counter_right = 0;
        counter_left = 0;
        start = millis();
    }
    if (Serial.available())
    {
        Serial.println("serial available");
        command = Serial.read();

        if (command == 'd')
        {
            setSpeed(100);
            Serial.println("d was pressed");
        }
        else if (command == 's')
        {
            setSpeed(0);
            Serial.println("s was pressed");
        }
        else if (command == 'p')
        {
            speed = speed + 10;
            if (speed > 255)
            {
                speed = 255;
            }
            setSpeed(speed);
        }
        else if (command == 'm')
        {
            speed = speed - 10;
            if (speed < 0)
            {
                speed = 0;
            }
            setSpeed(speed);
        }
    }
}