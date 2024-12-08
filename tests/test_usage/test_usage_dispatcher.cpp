//--------------------------------------------------------------
// Copyright (c) David Bosnich <david.bosnich.public@gmail.com>
//
// This code is licensed under the MIT License, a copy of which
// can be found in the license.txt file included at the root of
// this distribution, or at https://opensource.org/licenses/MIT
//--------------------------------------------------------------

#include <simple/event/dispatcher.h>
#include <catch2/catch.hpp>
#include <climits>
#include <atomic>
#include <thread>
#include <vector>

using namespace Simple::Event;
using namespace std;

//--------------------------------------------------------------
struct TestStruct
{
    //----------------------------------------------------------
    bool operator==(const TestStruct& a_other) const
    {
        return m_uint32_t == a_other.m_uint32_t &&
               m_int32_t == a_other.m_int32_t &&
               m_float == a_other.m_float &&
               m_bool == a_other.m_bool;
    }

    uint32_t m_uint32_t = 0;
    int32_t m_int32_t = 0;
    float m_float = 0.0f;
    bool m_bool = false;
};

//--------------------------------------------------------------
namespace
{
    TestStruct s_expectedStruct = {};
    string s_expectedString = {};
    float s_expectedFloat = {};
    bool s_expectedBool = {};
}

//--------------------------------------------------------------
class TestClass
{
public:

    //----------------------------------------------------------
    Status TestClassFunctionVoidArg()
    {
        REQUIRE(true);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionVoidArgConst() const
    {
        REQUIRE(true);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionBoolArg(bool a_bool)
    {
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionFloatArg(float a_float)
    {
        REQUIRE(a_float == s_expectedFloat);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionStringArg(string a_string)
    {
        REQUIRE(a_string == s_expectedString);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionStructArg(TestStruct a_struct)
    {
        REQUIRE(a_struct == s_expectedStruct);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionMultiArg(TestStruct a_struct,
                                     string a_string,
                                     float a_float,
                                     bool a_bool)
    {
        REQUIRE(a_struct == s_expectedStruct);
        REQUIRE(a_string == s_expectedString);
        REQUIRE(a_float == s_expectedFloat);
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionBoolRefArg(const bool& a_bool)
    {
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionFloatRefArg(const float& a_float)
    {
        REQUIRE(a_float == s_expectedFloat);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionStringRefArg(const string& a_string)
    {
        REQUIRE(a_string == s_expectedString);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionStructRefArg(const TestStruct& a_struct)
    {
        REQUIRE(a_struct == s_expectedStruct);
        return Status::Continue;
    }

    //----------------------------------------------------------
    Status TestClassFunctionMultiRefArg(const TestStruct& a_struct,
                                        const string& a_string,
                                        const float& a_float,
                                        const bool& a_bool)
    {
        REQUIRE(a_struct == s_expectedStruct);
        REQUIRE(a_string == s_expectedString);
        REQUIRE(a_float == s_expectedFloat);
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    }
};

//--------------------------------------------------------------
Status TestFreeFunctionVoidArg()
{
    REQUIRE(true);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionBoolArg(bool a_bool)
{
    REQUIRE(a_bool == s_expectedBool);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionFloatArg(float a_float)
{
    REQUIRE(a_float == s_expectedFloat);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionStringArg(string a_string)
{
    REQUIRE(a_string == s_expectedString);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionStructArg(TestStruct a_struct)
{
    REQUIRE(a_struct == s_expectedStruct);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionMultiArg(TestStruct a_struct,
                                string a_string,
                                float a_float,
                                bool a_bool)
{
    REQUIRE(a_struct == s_expectedStruct);
    REQUIRE(a_string == s_expectedString);
    REQUIRE(a_float == s_expectedFloat);
    REQUIRE(a_bool == s_expectedBool);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionBoolRefArg(const bool& a_bool)
{
    REQUIRE(a_bool == s_expectedBool);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionFloatRefArg(const float& a_float)
{
    REQUIRE(a_float == s_expectedFloat);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionStringRefArg(const string& a_string)
{
    REQUIRE(a_string == s_expectedString);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionStructRefArg(const TestStruct& a_struct)
{
    REQUIRE(a_struct == s_expectedStruct);
    return Status::Continue;
}

//--------------------------------------------------------------
Status TestFreeFunctionMultiRefArg(const TestStruct& a_struct,
                                   const string& a_string,
                                   const float& a_float,
                                   const bool& a_bool)
{
    REQUIRE(a_struct == s_expectedStruct);
    REQUIRE(a_string == s_expectedString);
    REQUIRE(a_float == s_expectedFloat);
    REQUIRE(a_bool == s_expectedBool);
    return Status::Continue;
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher void", "[dispatcher][void]")
{
    using TestDispatcher = Dispatcher<>;
    TestDispatcher dispatcher;
    TestClass testClass;
    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionVoidArg);
    TestDispatcher::Listener listener2 = dispatcher.Register(bind(&TestClass::TestClassFunctionVoidArg, &testClass));
    TestDispatcher::Listener listener3 = dispatcher.Register(bind(&TestClass::TestClassFunctionVoidArgConst, &testClass));
    TestDispatcher::Listener listener4 = dispatcher.Register([]()
    {
        REQUIRE(true);
        return Status::Continue;
    });
    TestDispatcher::Listener listener5 = dispatcher.Register([&testClass]()
    {
        testClass.TestClassFunctionVoidArg();
        return Status::Continue;
    });

    dispatcher.Dispatch();
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher bool", "[dispatcher][bool]")
{
    using TestDispatcher = Dispatcher<bool>;
    TestDispatcher dispatcher;
    TestClass testClass;
    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionBoolArg);
    TestDispatcher::Listener listener2 = dispatcher.Register(TestFreeFunctionBoolRefArg);
    TestDispatcher::Listener listener3 = dispatcher.Register(bind(&TestClass::TestClassFunctionBoolArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener4 = dispatcher.Register(bind(&TestClass::TestClassFunctionBoolRefArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener5 = dispatcher.Register([](bool a_bool)
    {
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    });
    TestDispatcher::Listener listener6 = dispatcher.Register([&testClass](bool a_bool)
    {
        testClass.TestClassFunctionBoolArg(a_bool);
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    });

    s_expectedBool = true;
    dispatcher.Dispatch(true);

    s_expectedBool = false;
    dispatcher.Dispatch(false);
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher float", "[dispatcher][float]")
{
    using TestDispatcher = Dispatcher<float>;
    TestDispatcher dispatcher;
    TestClass testClass;
    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionFloatArg);
    TestDispatcher::Listener listener2 = dispatcher.Register(TestFreeFunctionFloatRefArg);
    TestDispatcher::Listener listener3 = dispatcher.Register(bind(&TestClass::TestClassFunctionFloatArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener4 = dispatcher.Register(bind(&TestClass::TestClassFunctionFloatRefArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener5 = dispatcher.Register([](float a_float)
    {
        REQUIRE(a_float == s_expectedFloat);
        return Status::Continue;
    });
    TestDispatcher::Listener listener6 = dispatcher.Register([&testClass](float a_float)
    {
        testClass.TestClassFunctionFloatArg(a_float);
        REQUIRE(a_float == s_expectedFloat);
        return Status::Continue;
    });

    s_expectedFloat = 9.0f;
    dispatcher.Dispatch(9.0f);

    s_expectedFloat = 0.0f;
    dispatcher.Dispatch(0.0f);

    constexpr float PI = 3.14159265358979323846f;
    s_expectedFloat = PI;
    dispatcher.Dispatch(PI);
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher string", "[dispatcher][string]")
{
    using TestDispatcher = Dispatcher<string>;
    TestDispatcher dispatcher;
    TestClass testClass;

    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionStringArg);
    TestDispatcher::Listener listener2 = dispatcher.Register(TestFreeFunctionStringRefArg);
    TestDispatcher::Listener listener3 = dispatcher.Register(bind(&TestClass::TestClassFunctionStringArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener4 = dispatcher.Register(bind(&TestClass::TestClassFunctionStringRefArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener5 = dispatcher.Register([](const string& a_string)
    {
        REQUIRE(a_string == s_expectedString);
        return Status::Continue;
    });
    TestDispatcher::Listener listener6 = dispatcher.Register([&testClass](const string& a_string)
    {
        testClass.TestClassFunctionStringArg(a_string);
        REQUIRE(a_string == s_expectedString);
        return Status::Continue;
    });

    s_expectedString = "Haggis";
    dispatcher.Dispatch("Haggis");

    s_expectedString = "";
    dispatcher.Dispatch("");

    s_expectedString = "PI For 9!";
    dispatcher.Dispatch("PI For 9!");
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher struct", "[dispatcher][struct]")
{
    using TestDispatcher = Dispatcher<TestStruct>;
    TestDispatcher dispatcher;
    TestClass testClass;
    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionStructArg);
    TestDispatcher::Listener listener2 = dispatcher.Register(TestFreeFunctionStructRefArg);
    TestDispatcher::Listener listener3 = dispatcher.Register(bind(&TestClass::TestClassFunctionStructArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener4 = dispatcher.Register(bind(&TestClass::TestClassFunctionStructRefArg, &testClass, placeholders::_1));
    TestDispatcher::Listener listener5 = dispatcher.Register([](const TestStruct& a_struct)
    {
        REQUIRE(a_struct == s_expectedStruct);
        return Status::Continue;
    });
    TestDispatcher::Listener listener6 = dispatcher.Register([&testClass](const TestStruct& a_struct)
    {
        testClass.TestClassFunctionStructArg(a_struct);
        REQUIRE(a_struct == s_expectedStruct);
        return Status::Continue;
    });

    TestStruct testStruct;
    testStruct.m_uint32_t = 9;
    testStruct.m_int32_t = -9;
    testStruct.m_float = 9.0f;
    testStruct.m_bool = false;
    s_expectedStruct = testStruct;
    dispatcher.Dispatch(testStruct);

    testStruct.m_bool = true;
    s_expectedStruct = testStruct;
    dispatcher.Dispatch(s_expectedStruct);
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher multi", "[dispatcher][multi]")
{
    using TestDispatcher = Dispatcher<TestStruct, string, float, bool>;
    TestDispatcher dispatcher;
    TestClass testClass;
    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionMultiArg);
    TestDispatcher::Listener listener2 = dispatcher.Register(TestFreeFunctionMultiRefArg);
    TestDispatcher::Listener listener3 = dispatcher.Register(bind(&TestClass::TestClassFunctionMultiArg, &testClass, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
    TestDispatcher::Listener listener4 = dispatcher.Register(bind(&TestClass::TestClassFunctionMultiRefArg, &testClass, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
    TestDispatcher::Listener listener5 = dispatcher.Register([](const TestStruct& a_struct,
                                                                const string& a_string,
                                                                float a_float,
                                                                bool a_bool)
    {
        REQUIRE(a_struct == s_expectedStruct);
        REQUIRE(a_string == s_expectedString);
        REQUIRE(a_float == s_expectedFloat);
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    });
    TestDispatcher::Listener listener6 = dispatcher.Register([&testClass](const TestStruct& a_struct,
                                                                          const string& a_string,
                                                                          float a_float,
                                                                          bool a_bool)
    {
        testClass.TestClassFunctionMultiArg(a_struct, a_string, a_float, a_bool);
        REQUIRE(a_struct == s_expectedStruct);
        REQUIRE(a_string == s_expectedString);
        REQUIRE(a_float == s_expectedFloat);
        REQUIRE(a_bool == s_expectedBool);
        return Status::Continue;
    });

    TestStruct testStruct;
    testStruct.m_uint32_t = 9;
    testStruct.m_int32_t = -9;
    testStruct.m_float = 9.0f;
    testStruct.m_bool = false;

    s_expectedStruct = testStruct;
    s_expectedString = "Haggis";
    s_expectedFloat = -1.0f;
    s_expectedBool = true;
    dispatcher.Dispatch(testStruct, "Haggis", -1.0f, true);

    testStruct.m_bool = true;
    s_expectedStruct = testStruct;
    s_expectedString = "Another String";
    s_expectedFloat = 3.14f;
    s_expectedBool = false;
    dispatcher.Dispatch(s_expectedStruct, "Another String", 3.14f, false);
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher Consume", "[dispatcher][consume]")
{
    using TestDispatcher = Dispatcher<>;
    TestDispatcher dispatcher;
    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionVoidArg);
    TestDispatcher::Listener listener2 = dispatcher.Register([]()
    {
        REQUIRE(true);
        return Status::Consumed;
    });
    TestDispatcher::Listener listener3 = dispatcher.Register([]()
    {
        REQUIRE(false);
        return Status::Continue;
    });

    dispatcher.Dispatch();
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher Release", "[dispatcher][release]")
{
    using TestDispatcher = Dispatcher<>;
    TestDispatcher dispatcher;
    bool shouldBeInvoked = true;
    TestDispatcher::Listener listener1 = dispatcher.Register(TestFreeFunctionVoidArg);
    TestDispatcher::Listener listener2 = dispatcher.Register([&shouldBeInvoked]()
    {
        REQUIRE(shouldBeInvoked);
        return Status::Continue;
    });
    TestDispatcher::Listener listener3 = dispatcher.Register([]()
    {
        REQUIRE(true);
        return Status::Continue;
    });

    dispatcher.Dispatch();
    listener2 = nullptr;
    shouldBeInvoked = false;
    dispatcher.Dispatch();
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher Recursive", "[dispatcher][recursive]")
{
    using TestDispatcher = Dispatcher<>;
    TestDispatcher dispatcher;
    int invokedCount = 0;
    TestDispatcher::Listener listener1 = dispatcher.Register([&invokedCount]()
    {
        ++invokedCount;
        return Status::Continue;
    }, -1);
    TestDispatcher::Listener listener2 = dispatcher.Register([&listener1]()
    {
        listener1 = nullptr;
        return Status::Continue;
    });

    dispatcher.Dispatch();
    REQUIRE(invokedCount == 1);

    dispatcher.Dispatch();
    REQUIRE(invokedCount == 1);

    listener1 = dispatcher.Register([&invokedCount]()
    {
        ++invokedCount;
        return Status::Continue;
    });
    listener2 = dispatcher.Register([&listener1]()
    {
        listener1 = nullptr;
        return Status::Continue;
    }, -1);

    dispatcher.Dispatch();
    REQUIRE(invokedCount == 2);

    dispatcher.Dispatch();
    REQUIRE(invokedCount == 2);

    listener2 = dispatcher.Register([&listener1,
                                     &dispatcher,
                                     &invokedCount]()
    {
        if (!listener1)
        {
            listener1 = dispatcher.Register([&invokedCount]()
            {
                ++invokedCount;
                return Status::Continue;
            });
        }
        return Status::Continue;
    });

    dispatcher.Dispatch();
    REQUIRE(invokedCount == 2);

    dispatcher.Dispatch();
    REQUIRE(invokedCount == 3);
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher Priority", "[dispatcher][priority]")
{
    using TestDispatcher = Dispatcher<>;
    TestDispatcher dispatcher;
    uint32_t invokedCount = 0;
    TestDispatcher::Listener listener1 = dispatcher.Register([&invokedCount]()
    {
        REQUIRE(invokedCount == 2);
        ++invokedCount;
        return Status::Continue;
    }, INT_MAX);
    TestDispatcher::Listener listener2 = dispatcher.Register([&invokedCount]()
    {
        REQUIRE(invokedCount == 1);
        ++invokedCount;
        return Status::Continue;
    });
    TestDispatcher::Listener listener3 = dispatcher.Register([&invokedCount]()
    {
        REQUIRE(invokedCount == 0);
        ++invokedCount;
        return Status::Continue;
    }, INT_MIN);

    dispatcher.Dispatch();
}

//--------------------------------------------------------------
TEST_CASE("Test Dispatcher Thread", "[dispatcher][thread]")
{
    using TestDispatcher = Dispatcher<>;
    TestDispatcher dispatcher;
    const uint32_t numThreads = 64;
    atomic<uint32_t> invokedCount = { 0 };
    atomic<uint32_t> registeredCount = { 0 };

    vector<thread> threads;
    for (uint32_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&dispatcher,
                              &invokedCount,
                              &registeredCount]()
        {
            atomic_bool invoked = { false };
            TestDispatcher::Listener listener = dispatcher.Register([&invoked,
                                                                     &invokedCount]()
            {
                invoked = true;
                ++invokedCount;
                return Status::Continue;
            }, rand());

            ++registeredCount;
            while (!invoked);
        });
    }

    // Wait for all listeners to register.
    while (registeredCount < numThreads);
    REQUIRE(registeredCount == numThreads);

    // Dispatch the event.
    dispatcher.Dispatch();

    // Join all the threads.
    for (thread& testThread : threads)
    {
        testThread.join();
    }
    REQUIRE(invokedCount == numThreads);

    // Check that all listeners deregistered.
    dispatcher.Dispatch();
    REQUIRE(invokedCount == numThreads);
}

//--------------------------------------------------------------
Status TestListener(float a_float)
{
    // printf("TestListener function called with %f\n", a_float);
    (void)a_float;

    // Consume the event if passed a positive number.
    return a_float > 0.0f ? Status::Consumed : Status::Continue;
}

//--------------------------------------------------------------
// Make sure the example code from the readme.md file works.
//--------------------------------------------------------------
TEST_CASE("Test Dispatcher Example", "[dispatcher][example]")
{
    using TestDispatcher = Dispatcher<float>;
    TestDispatcher dispatcher;

    // Register two listeners.
    TestDispatcher::Listener listener1 = dispatcher.Register(TestListener);
    TestDispatcher::Listener listener2 = dispatcher.Register([](float a_float)
    {
        // printf("TestListener lambda called with %f\n", a_float);
        (void)a_float;
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
        // printf("TestListener higher priority called with %f\n", a_float);
        (void)a_float;

        return Status::Continue;
    }, -1);

    // Dispatch a positive number.
    dispatcher.Dispatch(1.0f);
    // Expected output:
    // "TestListener higher priority called with 1.0"
    // "TestListener function called with 1.0"

    // Dispatch a negative number.
    dispatcher.Dispatch(-9.0f);
    // Expected output:
    // "TestListener higher priority called with -9.0"
    // "TestListener function called with -9.0"
    // "TestListener lambda called with -9.0"

    // Release the higher priority listener.
    listener3.reset();

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
