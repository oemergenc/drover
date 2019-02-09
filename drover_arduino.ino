#include <Arduino.h>
#include "User_Setup.h"

char command;
// Include the TimerOne Library from Paul Stoffregen
#include "TimerOne.h"

// Constants for Interrupt Pins
// Change values if not using Arduino Uno

const byte MOTOR1 = 2;  // Motor 1 Interrupt Pin - INT 0
const byte MOTOR2 = 3;  // Motor 2 Interrupt Pin - INT 1

// Integers for pulse counters
unsigned int counter1 = 0;
unsigned int counter2 = 0;

// Float for number of slots in encoder disk
float diskslots = 10;  // Change to match value of encoder disk

// Interrupt Service Routines

// Motor 1 pulse count ISR
void ISR_count1()
{
    counter1++;  // increment Motor 1 counter value
}

// Motor 2 pulse count ISR
void ISR_count2()
{
    counter2++;  // increment Motor 2 counter value
}

// TimerOne ISR
void ISR_timerone()
{
    Timer1.detachInterrupt();  // Stop the timer
    Serial.print("Motor Speed 1: ");
    float rotation1 = (counter1 / diskslots) * 60.00;  // calculate RPM for Motor 1
    Serial.print(rotation1);
    Serial.print(" RPM - ");
    counter1 = 0;  //  reset counter to zero
    Serial.print("Motor Speed 2: ");
    float rotation2 = (counter2 / diskslots) * 60.00;  // calculate RPM for Motor 2
    Serial.print(rotation2);
    Serial.println(" RPM");
    counter2 = 0;  //  reset counter to zero
    Timer1.attachInterrupt(ISR_timerone);  // Enable the timer
}

void startMotors(int speed)
{
    digitalWrite(9, LOW);  //ENABLE CH A
    digitalWrite(12, HIGH);   //Sets direction of CH A

    digitalWrite(8, LOW); //ENABLE CH B
    digitalWrite(13, LOW); //Sets direction of CH B

    analogWrite(3, speed);   //Moves CH A
    analogWrite(11, speed);   //Moves CH B
}

void stopMotors()
{
    digitalWrite(9, HIGH);  //DISABLE CH A
    digitalWrite(8, HIGH); //DISABLE CH B
}

void setup()
{
    Serial.begin(9600);

    //Setup Channel A
    pinMode(12, OUTPUT); //Initiates Motor Channel A pin
    pinMode(9, OUTPUT); //Initiates Brake Channel A pin
    //Setup Channel B
    pinMode(13, OUTPUT); //Initiates Motor Channel B pin
    pinMode(8, OUTPUT);  //Initiates Brake Channel B pin

    Timer1.initialize(1000000); // set timer for 1sec
    attachInterrupt(0, ISR_count1, RISING);  // Increase counter 1 when speed sensor pin goes High
    attachInterrupt(1, ISR_count2, RISING);  // Increase counter 2 when speed sensor pin goes High
    Timer1.attachInterrupt(ISR_timerone); // Enable the timer
}

void loop()
{
    if (Serial.available())
    {
        Serial.println("serial available");
        command = Serial.read();

        if (command == 'd')
        {
            Serial.println("d was pressed");
            //forward @ full speed
            startMotors(50);
        }
        else if (command == 's')
        {
            Serial.println("s was pressed");
            stopMotors();
        }
    }
}
