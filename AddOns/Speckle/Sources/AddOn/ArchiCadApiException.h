#include <iostream>
#include <exception>
#include <string>

class ArchiCadApiException : public std::exception 
{
private:
    int errorCode;

public:
    ArchiCadApiException(int code) : errorCode(code) {}

    virtual const char* what() const noexcept override 
	{
        std::string message = "ArchiCad API Exception: Error code " + std::to_string(errorCode);
        return message.c_str();
    }

    int getErrorCode() const noexcept 
	{
        return errorCode;
    }
};
