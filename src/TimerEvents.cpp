#include "TimerEvents.h"

TimerEvent::TimerEvent(const char* n, unsigned long p, TimerEventHandler onExecute, bool e, long d, TimerEventHandler onFinish) {
    name = n;
    period = p;
    onExecutePtr = onExecute;
    isEnabled = e;
    duration = d;
    onFinishPtr = onFinish;
}

TimerEventsClass::TimerEventsClass() {
    numberOfEvents = 0;
}

void TimerEventsClass::add(TimerEvent* event) {
    if (numberOfEvents >= MAX_NUMBER_OF_EVENTS) {
        return;
    }
    events[numberOfEvents] = event;
    timeouts[numberOfEvents] = millis();
    durations[numberOfEvents] = millis();
    event->index = numberOfEvents;
    numberOfEvents++;
}

void TimerEventsClass::update(TimerEvent* event) {
    if (event->index == -1) return; // Check if the event has been assigned to the task list
    events[event->index] = event;
}

void TimerEventsClass::tasks() {
    const unsigned long time = millis();
    for (int i=0; i<numberOfEvents; i++) {
        if (!events[i]->isEnabled) continue; // Check if the event is enabled, skip if not
        
        if (events[i]->duration != -1) {                    // Check if the event has a specified duration
            if (events[i]->firstCalled == 0) {              // Check if the event has been called yet
                events[i]->firstCalled = time;              // Set the called flag
                durations[i] = time + events[i]->duration;  // Set a stop time for the event
            }
            if (time >= durations[i]) {                     // Check if the event duration has passed
                events[i]->disable();                       // Disable the event
                events[i]->firstCalled = 0;                 // Reset the called flag
                events[i]->finish();                        // Call the events onFinish callback
            }
        }

        if (time >= timeouts[i]) { // Check if the event timeout has passed
            timeouts[i] = time + events[i]->period;
            events[i]->execute();
        }
    }
}

void TimerEventsClass::printTasking(Stream* out) {
    for (int i=0; i<numberOfEvents; i++) {
        out->printf("Task name: %s \t| Period: %d ms \t| Enabled: %s \t \r\n",  events[i]->name, 
                                                                                events[i]->period, 
                                                                                events[i]->isEnabled ? "TRUE" :  "FALSE");
    }
}

TimerEventsClass timerEvents;
