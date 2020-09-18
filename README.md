# cpplazy
A hedaer-only, C#-like, generic class for lazy initialization in C++17.

### Simple API
```cpp
    lazy<std::string> lazy_string{ [] { return "very expensive initialization here...."s; } };
    //Same API as `std::optional"
    std::string data = *lazy_string;
    std::string data2 = lazy_string->value();
```

### Generic lazy initialized type 
```cpp
    lazy<std::array<int, 6>> lazy_fib_seq{ [] { return std::array<int, 6>{ 0, 1, 1, 2, 3, 5 }; } };
    //First 6 numbers of fibbonaci have not been created 
    std::array<int, 6> fib_seq = *lazy_fib_seq; //lazy object is initialized at (and only at) first usage
    std::array<int, 6> fib_seq2 = lazy_fib_seq->value(); //Returning the value, without re-initializing
```



### Thread safe access
```cpp
    lazy<int> the_answer_to_life_the_universeand_everything{ [] { std::cout << "Computing answer...Finished"  << std::endl; return 42; } };

    auto ask_question = [&] { std::cout << the_answer_to_life_the_universeand_everything->value() << std::endl; };
    std::thread hitchhiker_1(ask_question);
    std::thread hitchhiker_2(ask_question);

    hitchhiker_1.join();
    hitchhiker_2.join();

    //(Possible) Output:
    //Computing answer...Finished
    //42
    //42
```

### Failed initialization handling
```cpp
    using namespace std;

    cpplazy::lazy<string> lazy_config_value{ []() ->string { throw invalid_argument("can't open config file"); } };

    string the_value = lazy_config_value->value_or("oops"); //value will be fallback to: "oops"

    try
    {
        the_value = lazy_config_value->value();   //Will throw
        the_value = *lazy_config_value;           //Will throw
    }
    catch (const bad_optional_access& ex) 
    {
        std::cout << "'invalid_argument' was thrown when initializing, but"
                     " `bad_optional_access` is thrown when taking the value" << std::endl;
    }
```

## Installation

 Simply copy [`cpplazy.hpp`](include/cpplazy/cpplazy.hpp) to your project.
 See [demo project](demo/) and [tests](tests/tests.cpp) for examples.
