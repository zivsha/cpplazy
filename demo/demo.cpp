#include "cpplazy/cpplazy.hpp"
#include <iostream>
#include <vector>
#include <mutex>
#include <sstream>

namespace demo_helpers
{
    class large_object
    {
    public:
        large_object(std::thread::id initialized_by) :
            m_init_by(initialized_by)
        {
            std::cout << "large_object was created on thread id " << m_init_by << std::endl;
        }
        std::vector<long> data{ 1'000'000 }; //Init to one million longs
        std::thread::id m_init_by;
    };

    large_object create_large_object()
    {
        return large_object(std::this_thread::get_id());
    }

    struct large_object_creator
    {
        large_object operator()()
        {
            return large_object(std::this_thread::get_id());
        }
    };

    long thread_id_to_long(std::thread::id id)
    {
        std::ostringstream oss;
        oss << id;
        return std::stoll(oss.str());
    }
}

using namespace demo_helpers;

// The below demo is a similar example as given by .NET's Lazy<T> documentation (https://docs.microsoft.com/en-us/dotnet/api/system.lazy-1)
int main()
{
    //Create the lazy object with a lambda function that returns the type of the lazy object
    cpplazy::lazy<large_object> lazy_large_object{ []() { return large_object(std::this_thread::get_id()); } };

    //The following lines show additional way to initialize the lazy object (to get the same result as above):
    cpplazy::lazy<large_object> lazy_large_object2{ &create_large_object };
    large_object_creator lrg_obj_crtr;
    cpplazy::lazy<large_object> lazy_large_object3{ lrg_obj_crtr };

    std::cout << "large_object is not created until the first time you \n" 
                 " use its '->' (arrow) operator or its '*' (dereference) operator\n" << std::endl;
    
    std::vector<std::thread> threads;
    std::mutex lock;
    const auto num_threads = std::thread::hardware_concurrency() - 1;

    std::cout << "Creating, and starting " << num_threads << " threads that will access the same large_object now\n" << std::endl;
        for (size_t i = 0; i < num_threads; i++)
    {
        threads.emplace_back([&]() {
            //This is the thread code

            large_object& obj = *lazy_large_object;

            // IMPORTANT: lazy initialization is thread-safe, but it doesn't protect the
            //            object after creation. You must lock the object before accessing it,
            //            unless the type is thread safe. (large_object is not thread safe.)
            {
                std::lock_guard lg(lock); //C++17 type deduction
                auto thread_id = std::this_thread::get_id();
                obj.data[0] = thread_id_to_long(thread_id);
                std::cout << "Initialized by thread " << obj.m_init_by << "; last used by thread " << obj.data[0] << "." << std::endl;
            }
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "\nPress any key to end program" << std::endl;
    getchar();
}