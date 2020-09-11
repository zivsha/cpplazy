#include "catch.hpp"
#include <cpplazy\cpplazy.hpp>
#include <thread>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <type_traits>

using namespace std::literals;

//TEST_CASE("Compilation Check")
//{
//    SECTION("Declarations")
//    {
//        lazy<int>                   l0{ []() { return 42;                               } };
//        lazy<double>                l1{ []() { return 3.14;                             } };
//        lazy<char>                  l2{ []() { return '\n';                             } };
//        lazy<std::string>           l3{ []() { return "lazy"s;                          } };
//        lazy<std::array<int, 2>>    l4{ []() { return std::array<int, 2 >{ 1,2 };       } };
//        lazy<std::unique_ptr<int>>  l5{ []() { return std::make_unique<int>(12);        } };
//    }
//
//    SECTION("noncopyable")
//    {
//        static_assert(std::is_copy_constructible<lazy<int>>::value == false, "Lazy should be non copyable");
//    }
//
//    SECTION("moveable")
//    {
//        lazy<int> l{ []() { return 42; } };
//        lazy<int> l2 = std::move(l);
//    }
//
//    SECTION("Lambda")
//    {
//
//    }
//
//    SECTION("Functor")
//    {
//
//    }
//
//    SECTION("Function Pointer")
//    {
//
//    }
//
//    SECTION("Static Functions")
//    {
//
//    }
//
//    SECTION("Free Functions")
//    {
//
//    }
//
//    SECTION("Member Functions")
//    {
//
//    }
//
//    SECTION("std::bind")
//    {
//
//    }
//
//    SECTION("Dereference")
//    {
//
//    }
//
//    SECTION("Arrow Operator")
//    {
//
//    }
//}
//
//TEST_CASE("Lazy Initialization")
//{
//
//}
//
//TEST_CASE("Thread safe initialization")
//{
//
//}
//TEST_CASE("Thread safe move")
//{
//
//}
//
//TEST_CASE("const access")
//{
//    struct throw_when_non_const_accessed
//    {
//        throw_when_non_const_accessed(std::string s) : s(std::move(s)) {}
//        std::string get() { throw std::exception{}; }
//        std::string get() const { return s; }
//        std::string s;
//    };
//
//    const throw_when_non_const_accessed value{ "initialized"s };
//    std::string s1;
//    int init_count = 0;
//    auto dereference_op = [&](const lazy<throw_when_non_const_accessed>& s) { s1 = (*s).get(); };
//    auto arrow_op = [&](const lazy<throw_when_non_const_accessed>& s) { s1 = s->value().get(); };
//
//    lazy<throw_when_non_const_accessed> lazy_s{ [&]() { ++init_count;  return value; } };
//    REQUIRE(init_count == 0);
//
//    REQUIRE(s1 != value.s);
//    dereference_op(lazy_s);
//    REQUIRE(s1 == value.s);
//    REQUIRE(init_count == 1);
//    s1 = "";
//    arrow_op(lazy_s);
//    REQUIRE(s1 == value.s);
//    REQUIRE(init_count == 1);
//
//    auto will_throw = [&](lazy<throw_when_non_const_accessed>& s) { s1 = s->value().get(); };
//    auto will_throw2 = [&](lazy<throw_when_non_const_accessed>& s) { s1 = (*s).get(); };
//    REQUIRE_THROWS(will_throw(lazy_s));
//    REQUIRE_THROWS(will_throw2(lazy_s));
//}
//
//TEST_CASE("Move an non initialized lazy")
//{
//    const int Value = 42;
//    int init_count = 0;
//    lazy<int> l{ [&]() { ++init_count;  return Value; } };
//    REQUIRE(init_count == 0);
//    lazy<int> l2 = std::move(l);
//    int value = *l2;
//    REQUIRE(value == Value);
//    REQUIRE(init_count == 1);
//}
//TEST_CASE("Move an already initialized lazy")
//{
//    const int Value = 42;
//    int init_count = 0;
//    lazy<int> l{ [&]() { ++init_count;  return Value; } };
//    int init_value = *l;
//    REQUIRE(init_value == Value);
//    REQUIRE(init_count == 1);
//
//    lazy<int> l2 = std::move(l);
//    int init_value2 = *l2;
//    REQUIRE(init_value2 == Value);
//    REQUIRE(init_count == 1);
//
//}


TEST_CASE("README Simple API")
{
    cpplazy::lazy<std::string> lazy_string{ []() { return "very expensive initialization here...."s; } };
    //Same API as `std::optional"
    std::string data = *lazy_string;
    std::string data2 = lazy_string->value();
}

TEST_CASE("README Generic lazy initialized type")
{
    cpplazy::lazy<std::array<int, 6>> lazy_fib_seq{ []() { return std::array<int, 6>{ 0, 1, 1, 2, 3, 5 }; } };
    //First 6 numbers of fibbonaci have not been created 
    std::array<int, 6> fib_seq = *lazy_fib_seq; //lazy object is initialized at (and only at) first usage
    std::array<int, 6> fib_seq2 = lazy_fib_seq->value(); //Returning the value, without re-initializing
}
#include <iostream>

TEST_CASE("README Thread safe access")
{
    cpplazy::lazy<int> the_answer_to_life_the_universeand_everything{ []() { std::cout << "Computing answer...Finished"  << std::endl; return 42; } };

    auto ask_question = [&]() { std::cout << the_answer_to_life_the_universeand_everything->value() << std::endl; };
    std::thread hitchhiker_1(ask_question);
    std::thread hitchhiker_2(ask_question);

    hitchhiker_1.join();
    hitchhiker_2.join();

    //(Possible) Output:
    //Computing answer...Finished
    //42
    //42
}

TEST_CASE("README Failed initialization handling")
{
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
}
