/*
 * Copyright (C) 2013-2016 Trent Houliston <trent@houliston.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pure_evil.h"

#if !defined(NDEBUG) && !defined(__APPLE__)

#include <nuclear>
#include <backtrace.h>
#include <dlfcn.h>

#include <iostream>

namespace utility {
    namespace support {
        namespace evil {
            thread_local std::vector<StackFrame> stack;
            thread_local std::string exception_name;
        }
    }
}

extern "C" {

    // Don't know what to do with this
    void error_callback(void * /*data*/, const char* /*msg*/, int /*errnum*/) {
    }

    // Initialise our state for the backtrace (I think we can do this once per binary)
    backtrace_state* state = backtrace_create_state(nullptr, true, error_callback, nullptr);

    typedef void (*cxa_throw_func_type)(void*,void*,void(*)(void*));

    /**
     * This function looks inside the libstdc++.so.6 library to manually load the __cxa_throw function
     *
     * It's a monstrosity!
     */
    cxa_throw_func_type get_real_throw_func() {

        // Load libc++
        void* handle = dlopen("libstdc++.so.6", RTLD_LAZY);

        // Get the __cxa_throw function
        return (cxa_throw_func_type) dlsym(handle, "__cxa_throw");
    }

    // Find the real __cxa_throw and call it to actually throw the exception
    static void (*const rethrow)(void*,void*,void(*)(void*)) __attribute__ ((noreturn)) = get_real_throw_func();

    /**
     * Add the stack frame to our list we pass in
     *
     * @param data the vector to pass in
     * @param pc the program counter
     * @param filename the filename
     * @param lineno the line number
     * @param function the function we are using
     *
     * @return 0 if we should continue (always returns 0)
     */
    int add_backtrace_info_to_vector(void* data, uintptr_t pc, const char* filename, int lineno, const char* function) {

        // Get our stack
        std::vector<utility::support::evil::StackFrame>* stack = static_cast<std::vector<utility::support::evil::StackFrame>*>(data);

        if(filename && function) {
            // Add a new one
            stack->emplace_back(pc, filename, lineno, NUClear::util::demangle(function));
        }

        return 0;
    }

    /**
     * This symbol is here to intercept all exception throwing and extract the
     * stack trace when it happens.
     *
     * @param ex the exception object
     * @param info the typeinfo of the exception
     * @param dest where we are going to execute when we are done here
     */
    void __cxa_throw(void *ex, void *info, void (*dest)(void *)) {

        // Demangle our exception name
        utility::support::evil::exception_name = NUClear::util::demangle(reinterpret_cast<const std::type_info*>(info)->name());

        // Perform our full stack backtrace
        utility::support::evil::stack.clear();
        backtrace_full(state, 1, add_backtrace_info_to_vector, error_callback, &utility::support::evil::stack);

        rethrow(ex, info, dest);
    }
}

#endif  // NDEBUG or APPLE
