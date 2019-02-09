#include <Arduino.h>
#include <TimerOne.h>

//#define LOOP_TIME 2000000
#define LOOP_TIME 500000

//Function	        Channel A	Channel B
//Direction	        Digital 12	Digital 13
//Speed (PWM)	    Digital 3	Digital 11
//Brake	            Digital 9	Digital 8
//Current Sensing	Analog 0	Analog 1

#define DIRECTION_CHANNEL_A 12
#define DIRECTION_CHANNEL_B 13
#define SPEED_CHANNEL_A 3
#define SPEED_CHANNEL_B 11
#define BRAKE_CHANNEL_A 9
#define BRAKE_CHANNEL_B 8

#define left_encoder_pin 2
#define right_encoder_pin 3

const float pi = 3.1415;
const float stepcount = 10.00;  // 10 Slots in disk, change if different
const float wheeldiameter = 66.10; // Wheel diameter in millimeters, change if different
volatile int counter_left = 0;
volatile int counter_right = 0;
long start = 0;
char command;
int speed = 100;

volatile float rpmLeft = 0;
volatile float rpmRight = 0;

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
//    rpmLeft = (counter_left / stepcount) * 60.00;  // calculate RPM for Motor 1
//    rpmRight = (counter_right / stepcount) * 60.00;  // calculate RPM for Motor 2
//    counter_right = 0;
//    counter_left = 0;
    Timer1.attachInterrupt(timerIsr);  //enable the timer
}

void startMotors(int speed)
{
    //Motor A forward @ full speed
    digitalWrite(DIRECTION_CHANNEL_A, HIGH); //Establishes forward direction of Channel A
    digitalWrite(BRAKE_CHANNEL_A, LOW);   //Disengage the Brake for Channel A

    //Motor B forward @ full speed
    digitalWrite(DIRECTION_CHANNEL_B, HIGH); //Establishes forward direction of Channel B
    digitalWrite(BRAKE_CHANNEL_B, LOW);   //Disengage the Brake for Channel B
    setSpeed(speed);
}


void stopMotors()
{
    digitalWrite(BRAKE_CHANNEL_A, HIGH);  //Engage the Brake for Channel A
    digitalWrite(BRAKE_CHANNEL_B, HIGH);  //Engage the Brake for Channel B
    digitalWrite(SPEED_CHANNEL_A, LOW);
    digitalWrite(SPEED_CHANNEL_B, LOW);
    setSpeed(0);
    counter_right = 0;
    counter_left = 0;
}


void setSpeed(int speed)
{
    analogWrite(SPEED_CHANNEL_A, speed);   //Spins the motor on Channel A at full speed
    analogWrite(SPEED_CHANNEL_B, speed);   //Spins the motor on Channel B at full speed
}

void setup()
{
    Serial.begin(9600);
    //Setup Channel A
    pinMode(DIRECTION_CHANNEL_A, OUTPUT); //Initiates Motor Channel A pin
    pinMode(BRAKE_CHANNEL_A, OUTPUT); //Initiates Brake Channel A pin

    //Setup Channel B
    pinMode(DIRECTION_CHANNEL_B, OUTPUT); //Initiates Motor Channel B pin
    pinMode(BRAKE_CHANNEL_B, OUTPUT);  //Initiates Brake Channel B pin

    analogWrite(SPEED_CHANNEL_A, 0);
    analogWrite(SPEED_CHANNEL_B, 0);
    digitalWrite(BRAKE_CHANNEL_A, HIGH);  //Engage the Brake for Channel A
    digitalWrite(BRAKE_CHANNEL_B, HIGH);  //Engage the Brake for Channel B

    //Setup for encoders
    pinMode(right_encoder_pin, INPUT_PULLUP);
    pinMode(left_encoder_pin, INPUT_PULLUP);

    Timer1.initialize(LOOP_TIME);
    attachInterrupt(digitalPinToInterrupt(left_encoder_pin), docount_left,
                    CHANGE);  // increase counter when speed sensor pin goes High
    attachInterrupt(digitalPinToInterrupt(right_encoder_pin), docount_right,
                    CHANGE);  // increase counter when speed sensor pin goes High
    Timer1.attachInterrupt(timerIsr); // enable the timer
}

void loop()
{
    if (millis() - start >= 10)
    {
        start = millis();
        Serial.println("left");
        Serial.println(counter_left);
        Serial.println("right");
        Serial.println(counter_right);
//        Serial.print("Motor Speed 1: ");
//        Serial.print(rpmLeft);
//        Serial.print(" RPM - ");
//        Serial.print("Motor Speed 2: ");
//        Serial.print(rpmRight);
//        Serial.println(" RPM");
    }
    if (Serial.available())
    {
        Serial.println("serial available");
        command = Serial.read();

        if (command == 'd')
        {
            Serial.println("d was pressed");
            //forward @ full speed
            startMotors(100);
        }
        else if (command == 's')
        {
            Serial.println("s was pressed");
            stopMotors();
        }
        else if (command == 'p')
        {
            if (speed < 255)
            {
                speed = speed + 10;
            }
            else
            {
                speed = 255;
            }
            setSpeed(speed);
        }
        else if (command == 'm')
        {
            if (speed > 0)
            {
                speed = speed - 10;
            }
            else
            {
                speed = 0;
            }
            setSpeed(speed);
        }
    }
}