#include <iostream>
#include <exception>
#include <string>

class NotImplementedException : public std::exception
{
public:
    virtual char const * what() const { return "Function not yet implemented."; }
};