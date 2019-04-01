#include <Arduino.h>
#include "../../../../../.platformio/packages/framework-arduinoavr/cores/arduino/HardwareSerial.h"

#define left_encoder_pin 20
#define right_encoder_pin 21

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

void setSpeed(int speed)
{
    //Motor A
    digitalWrite(12, HIGH);     //Establishes forward direction of Channel A
    digitalWrite(9, LOW);       //Disengage the Brake for Channel A
    analogWrite(3, speed);      //Spins the motor on Channel A at full speed

    //Motor B
    digitalWrite(13, LOW);     //Establishes backward direction of Channel B
    digitalWrite(8, LOW);       //Disengage the Brake for Channel B
    analogWrite(11, speed);     //Spins the motor on Channel B at half speed
}

void setup()
{
    Serial.begin(9600);
    //Setup for encoders
    pinMode(right_encoder_pin, INPUT_PULLUP);
    pinMode(left_encoder_pin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(left_encoder_pin), docount_left, CHANGE);
    attachInterrupt(digitalPinToInterrupt(right_encoder_pin), docount_right, CHANGE);

    //Setup Channel A
    pinMode(12, OUTPUT); //Initiates Brake Channel A pin
    pinMode(9, OUTPUT); //Initiates Brake Channel A pin
    //Setup Channel B
    pinMode(13, OUTPUT); //Initiates Motor Channel A pin
    pinMode(8, OUTPUT);  //Initiates Brake Channel A pin

}

void loop()
{
    if (millis() - start >= 500)
    {

        rpmLeft = (counter_left / stepcount) * 60;  // calculate RPM for Motor 1
        rpmRight = (counter_right / stepcount) * 60;  // calculate RPM for Motor 2
        Serial.println("left");
        Serial.println("right");
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
            speed = 0;
            setSpeed(speed);
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
            if (speed < -255)
            {
                speed = -255;
            }
            setSpeed(speed);
        }
    }
}