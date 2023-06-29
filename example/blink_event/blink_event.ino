/**
 * @file blink_event.ino
 * @author Braidan Duffy (braidan dot duffy at gmail dot com)
 * 
 * @brief A demonstration of using the TimerEvents class to do the basic Arduino
 * blink script
*/

#include <Arduino.h>
#include <TimerEvents.h>

bool ledOn = false;

static void ledCallback() {
    ledOn = !ledOn;
    digitalWrite(LED_BUILTIN, ledOn);
}

void setup() {
    Serial.begin(9600);
    while(!Serial); // Wait for serial port to open

    Serial.println("Timer Event Demonstration!");

    pinMode(LED_BUILTIN, OUTPUT);

    TimerEvent blinkEvent("Blink Event", 1000, ledCallback, true);
    timerEvents.add(blinkEvent);
    timerEvents.printTasking(&Serial);
}

void loop() {
    timerEvents.tasks();
}