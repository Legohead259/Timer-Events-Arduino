# Timer-Events-Arduino

A class for handling non-important events in Arduino programs

## Purpose

To develop a portable library that can be used to minimize the compute cycle resources required for managing non-time critical tasks within an Arduino program.

## Problem

The common logic implementation for running a non-critical non-blocking (without using `delay()`) timing task in Arduino (like blinking an LED) is to check the current system time using `millis()`, finding the length of time since the task was last performed, `millis() - lastTimePerformed`, and then compare this difference to a timeout value, `ACTION_TIME`.
The entire `if-statement` looks like this:

```cpp
if (millis() - lastTimePerformed > ACTION_TIME) {
    // Perform action
    lastTimePerformed = millis();
}
```

Despite being logical, this requires two computations to be performed every loop.
One to subtract the `lastTimePerformed` from the `millis()` value, and another to do a comparison to `TASK_PERIOD`.
Neither are computationally expense, but the former takes many more compute cycles to execute because it handles `uint32` values.
This issue also increases linearly with the number of tasks.
If there are hundreds of non-time critical tasks to be performed, the microcontroller will compute hundreds of subtractions every loop just to make a simple boolean comparison.

Additionally, This method is poor object-oriented practice because for every new task, the same `if-statement` block would have to be added into the main execution loop.
If a future maintainer wants to tweak the parameters of a task, they would have to first track down the specific `if` block and adjust it manually.

## Solution

To solve this problem, we will take a three-step approach.
First, we will tweak the execution determination logic to only calculate the next execution time
Second, we shall define a task in an object-oriented manner where the task and its parameters are encapsulated into a `TimerEvent` task.
This object will contain all of the information needed to execute the task at a specific time, enable/disable its execution, and have callback functions for when the event occurs.
Third, we will define a `TimerEventManager` object that can build an array of `TimerEvent`s that it will iterate through every loop and determine if the event needs to be executed and then do so.

### Step One: Tweaking the Logic

To solve the first problem of the timer events scenario, we shall change the `if-statement` logic to only compare the `nextTimeToBePerformed` value to the current timestamp.
This `nextTime` value is calculated at the conclusion of the task after the `TASK_PERIOD` has passed since the last execution.
The tweaked `if-statement` block looks like the following:

```cpp
uint32_t currentTime = millis();
if (currentTime >= nextTimeToBePerformed) {
    // Do event
    nextTimeToBePerformed = currentTime + TASK_PERIOD;
}
```

With this logic, we cut the number of computations by half while still retaining the same functionality as the original code.
Because of how unsigned integers behave, this method is also rollover transparent for when the `millis()` function exceeds its maximum value and rolls over near 0.

### Step Two: Encapsulating the Task

We define a non-critical timing task as an event that needs to occur with a certain `period`, can have a certain `duration`, can be `enabled` or `disabled`, and can perform different functions on `execution` and `finish`.
We can also give these tasks a name that makes them more human-readable and easier to maintain/debug in the application.

This task can be defined in a class, [`TimerEvent`](src/TimerEvents.h), that handles all these basic functions.

We can declare a basic task like blinking the on-board LED by implementing the following:

```cpp
    TimerEvent blinkEvent(  "Blink Event",  // Name of the event
                            1000,           // Period, ms. Valid [0, MAX_VALUE_UINT32]
                            ledCallback,    // On execution callback function 
                            true,           // Enabled
                            -1,             // Duration, ms. Valid [0, MAX_VALUE_LONG]. Default: -1
                            nullptr)        // On finish callback function. Default: nullptr
```

### Step Three: Managing `TimerEvent`s

If we want to declare and hnadle multiple events, we will need a helper object to help us manage them.
This manager must be able to build and maintain an array of pointers to `events` and handle their execution properly.
Some events may have a duration they need to be active before stopping (e.g. blink an LED purple and white rapidly for 5 seconds).
Some events may also need to trigger another function when they end (e.g. resetting a watch dog or cleaning up memory after an action).

This manager can be found in a class declared in [`TimerEvent.h`](src/TimerEvents.h) and defined in [`TimerEvents.cpp`](src/TimerEvents.cpp).

We can declare and implement this task manager in an application like so:

```cpp
    TimerEventsClass timerEvents;

    TimerEvent blinkEvent(  "Blink Event",      // Name of the event
                                1000,           // Period, ms. Valid [0, MAX_VALUE_UINT32]
                                ledCallback,    // On execution callback function 
                                true,           // Enabled
                                -1,             // Duration, ms. Valid [0, MAX_VALUE_LONG]. Default: -1
                                nullptr);       // On finish callback function. Default: nullptr
    
    timerEvents.add(&blinkEvent); // Add blinkEvent to task manager
    .
    .
    .
    void loop() {
        timerEvents.tasks(); // Loop over timer tasks and execute as appropriate
    }
```
