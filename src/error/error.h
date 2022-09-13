#include <stdexcept>
#include <string>
#include <cstring>

#include "lexer/token.h"

#include <stdexcept>
#include <stdio.h>
#include <stdarg.h>

// Error class
// Takes in a format string and a number of arguments 

class compiler_error : public std::exception
{
private:
    char* retval;
public:
    // Writes to retval the formatted string
    compiler_error(const std::string& fstr, ...)
    {
        va_list aptr;

        va_start(aptr, fstr);
        vasprintf(&retval, fstr.c_str(), aptr);
        va_end(aptr);
    }

    const char* what() const noexcept override
    {
        return retval;
    }
};

