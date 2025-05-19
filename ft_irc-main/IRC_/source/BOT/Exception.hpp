#pragma once
#include <iostream>
#include <string>
#include <exception>

# define RESET		"\033[0m"
# define RED		"\033[0;31m"



class Exception : public std::exception {
    private:
        const char* messages;

    public:
        Exception(const char* msg) : messages(msg) {}

        virtual const char* what() const throw(){
            static std::string msg = std::string(RED) + "Error: " + this->messages + RESET + '\n';
            return(msg.c_str());
        }
};
