#pragma once
#include <iostream>
#include <string>
#include "Server.hpp"
#include <vector>

#define HEX 1
#define NC 2

class Client
{
    public:
        std::vector<std::string> chatMessages;
        int client_fd;
        int client_port;
        char ip_adress[INET_ADDRSTRLEN];
        int passCheck;
        int hex_or_numeric;

        std::string buffer;
        std::string ip;
        std::string user;
        std::string nick;
        std::string pass;

        Client();
        ~Client();
        Client& operator=(Client const &copy);

        std::string buildPrefixString() const;
        
};