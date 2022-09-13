#include <stdexcept>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <iostream>

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

int main(void)
{
    try
    {
        throw compiler_error("%s, %s", "Sup", "Isak");
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}