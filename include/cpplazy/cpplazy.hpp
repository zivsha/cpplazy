// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// Copyright (c) 2020 Ziv Shahaf <ziv.shahaf@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <mutex>
#include <functional>
#include <optional>
#include <utility>


namespace cpplazy
{
    // Provides support lazy initialization.
    template<typename T>
    class lazy
    {
        mutable std::once_flag m_init_flag;
        const std::function<T()> m_init_func;
        mutable std::optional<T> m_value;

    public:

        explicit lazy(std::function<T()> initFunc) : 
            m_init_func(std::move(initFunc)) 
        {
        }
        
        lazy(const lazy&) = delete; // It would make your code awkward if copying was allowed (how would you enforce the init function can be called twice?)

        lazy(lazy&& other) noexcept :
            m_init_func(std::move(other.m_init_func))
        {
            // The other object might be initialized already. 
            // In this case the move c'tor needs to make sure this->m_value is set to the same value, and not re-initialize.
            // Using call_once to determine it.

            const bool was_other_initialized = [&other]()
            {
                bool other_once_flag_used = true;
                std::call_once(other.m_init_flag, [&other_once_flag_used]() { other_once_flag_used = false; });
                return other_once_flag_used;
            }();
            
            if (was_other_initialized)
            {
                std::call_once(m_init_flag, [this, &other]() { m_value.swap(other.m_value); });
            }
        }

        std::optional<T>* operator->()
        {
            return get_or_init();
        }

        const std::optional<T>* const operator->() const
        {
            return get_or_init();
        }

        T& operator*()
        {
            return get_or_init()->value();
        }

        const T& operator*() const
        {
            return get_or_init()->value();
        }

    private:

        std::optional<T>* get_or_init() const
        {
            try
            {
                std::call_once(m_init_flag, [this]() { m_value = m_init_func(); });
            }
            catch (...)
            {

            }

            return &m_value;
        }
    };

    //TODO: Add deduction guids to allow cpplazy::lazy lazy_string{[](){return ""s;}; ?
}