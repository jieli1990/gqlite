#pragma once
#include <cstdarg>
#include <string>
#include <exception>

#define MAX_BUFFER_SIZE 256

class GRuntimeException : public std::exception {
public:
  GRuntimeException(const char* fmt, ...){
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    vsnprintf(_msg, MAX_BUFFER_SIZE, fmt, arg_ptr);
    va_end(arg_ptr);
  }

#ifdef __linux__
    virtual const char* what() const 
#if defined(__ANDROID__)
    _NOEXCEPT
#elif defined(__linux__)
    _GLIBCXX_NOTHROW
#endif
    {
      return _msg;
    }
#endif

private:
  char _msg[MAX_BUFFER_SIZE];
};

#undef MAX_BUFFER_SIZE
