# Simple Event


### Overview
SimpleEvent is a header-only C++ framework containing a template
Dispatcher class, which maintains a collection of event listener
functions to be sequentially invoked when an event is dispatched.

Designed primarily for simple applications that need to dispatch
generic events to a number of listeners based on their priority.

The primary design consideration was ease-of-use, although every
effort has been made to not compromise efficiency or flexibility.


### Features
#### Dispatcher
Instantiate the Simple::Event::Dispatcher template class with a
parameter pack defining the signature of events to be dispatched.

#### Listener
An instantiated Simple::Event::Dispatcher class defines Function
and Listener types, used to register and retain (respectively) a
callable object that will be invoked when an event is dispatched.

The lifetime of Listener objects is managed with smart pointers,
and it is the responsibility of the caller to retain registered
listeners (along with the underlying callable used to register).

Once all references to a Listener have been released it will no
longer be invoked when an event is dispatched; internally it is
stored as a weak pointer and removed during dispatch if expired.

#### Events
Call Dispatch on a Simple::Event::Dispatcher object instance to
send an event to all listeners registered with that dispatcher.

#### Priority
When an event is dispatched, listeners are invoked sequentially
based on the (optional) sort order defined during registration.

#### Consumable
All listener functions must return a Simple::Event::Status enum
value (either Continue or Consumed) that determines whether to
continue dispatching the event to any lower priority listeners.


### API Documentation
The public API documentation is built using the DOC_BUILD target
generated by CMake (but only if the Doxygen dependency is found).


### Unit Tests
CMake can be used to generate test projects (eg. VS, Xcode, make)
that build/run the suite of unit tests found in the tests folder.


### Supported Platforms
This project has been tested using the following C++17 compilers:
- msvc (Visual Studio 2022)
- clang (Xcode 15)
- gcc (make)


### Example
The following is a small sample of the core functionality which
is provided by the framework. The above mentioned test suite is
also a good reference that demonstrates a broader range of uses.

#### main.cpp
```
#include <simple/event/dispatcher.h>

Status TestListener(float a_float)
{
    printf("TestListener function called with %f\n", a_float);

    // Consume the event if passed a positive number.
    return a_float > 0.0f ? Status::Consumed : Status::Continue;
}

int main(int argc, char* argv[])
{
    using TestDispatcher = Dispatcher<float>;
    TestDispatcher dispatcher;

    // Register two listeners.
    TestDispatcher::Listener listener1 = dispatcher.Register(TestListener, -1);
    TestDispatcher::Listener listener2 = dispatcher.Register([](float a_float)
    {
        printf("TestListener lambda called with %f\n", a_float);
        return Status::Continue;
    });

    // Dispatch a positive number.
    dispatcher.Dispatch(9.0f);
    // Expected output:
    // "TestListener function called with 9.0"

    // Dispatch a negative number.
    dispatcher.Dispatch(0.0f);
    // Expected output:
    // "TestListener function called with 0.0"
    // "TestListener lambda called with 0.0"

    // Register another listener with a higher priority.
    TestDispatcher::Listener listener3 = dispatcher.Register([](float a_float)
    {
        printf("TestListener higher priority called with %f\n", a_float);
        return Status::Continue;
    }, -1);

    // Dispatch a negative number.
    dispatcher.Dispatch(-9.0f);
    // Expected output:
    // "TestListener higher priority called with -9.0"
    // "TestListener function called with -9.0"
    // "TestListener lambda called with -9.0"

    // Dispatch a positive number.
    dispatcher.Dispatch(1.0f);
    // Expected output:
    // "TestListener higher priority called with 1.0"
    // "TestListener function called with 1.0"

    // Release the higher priority listener.
    listener3.release();

    // Dispatch a positive number.
    dispatcher.Dispatch(3.14f);
    // Expected output:
    // "TestListener function called with 3.14"

    // Dispatch zero.
    dispatcher.Dispatch(0.0f);
    // Expected output:
    // "TestListener function called with 0.0"
    // "TestListener lambda called with 0.0"
}
```


### Authors
- David Bosnich - <david.bosnich.public@gmail.com>


### License
Copyright (c) David Bosnich <david.bosnich.public@gmail.com>

This code is licensed under the MIT License, a copy of which
can be found in the license.txt file included at the root of
this distribution, or at https://opensource.org/licenses/MIT