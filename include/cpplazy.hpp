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

template<typename T>

class Lazy
{
    std::once_flag m_initFlag;
    std::function<T()> m_initFunc;
    std::optional<T> m_value;

public:

    Lazy(std::function<T()> initFunc) : m_initFunc(std::move(initFunc)) {}
    std::optional<T>* operator->() { return get_or_init(); }
    T operator*() { return get_or_init()->value(); }

private:

    std::optional<T>* get_or_init()
    {
        try
        {
            std::call_once(m_initFlag, [this]() { std::cout << "Call once!" << std::endl; m_value = m_initFunc(); });
        }
        catch (std::exception& ex)
        {
            //std::cerr << "Error calling once: " << ex.what() << std::endl;
        }
        catch (...)
        {
            //std::cerr << "Error calling once" << std::endl;
        }

        return &m_value;
    }
};
