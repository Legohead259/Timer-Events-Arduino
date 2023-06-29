#ifndef TIMER_EVENTS_H
#define TIMER_EVENTS_H

#include <Arduino.h>

#define MAX_NUMBER_OF_EVENTS 32

typedef std::function<void()> TimerEventHandler;

class TimerEvent {

public:
    TimerEvent(const char* n, unsigned long p, TimerEventHandler onExecute, bool e, long d=-1, TimerEventHandler onFinish=nullptr);

    TimerEvent();
    const char* name;
    unsigned long period;
    TimerEventHandler onExecutePtr = nullptr;
    TimerEventHandler onFinishPtr = nullptr;
    bool isEnabled;
    long duration;
    unsigned long firstCalled = 0;
    int index = -1;

    void execute() { if (onExecutePtr != nullptr) onExecutePtr(); }
    void finish() { if (onFinishPtr != nullptr) onFinishPtr(); }
    void setEventCallback(TimerEventHandler cbPtr) { onExecutePtr = cbPtr; }
    void setFinishCallback(TimerEventHandler cbPtr) { onFinishPtr = cbPtr; }
    void enable() { isEnabled = true; }
    void disable() { isEnabled = false; }
};

class TimerEventsClass {

public:
    TimerEventsClass();
    void add(TimerEvent* event);
    void update(TimerEvent* event);
    void tasks();

    void printTasking(Stream* out);

    int getNumberOfEvents() { return numberOfEvents; }

private:
    int numberOfEvents;
    TimerEvent* events[MAX_NUMBER_OF_EVENTS];
    unsigned long timeouts[MAX_NUMBER_OF_EVENTS];
    unsigned long durations[MAX_NUMBER_OF_EVENTS];
};

extern TimerEventsClass timerEvents;

#endif
