#include "Client.hpp"

Client::Client() : passCheck(0){}

Client::~Client(){}


std::string Client::buildPrefixString() const {
    std::string tmp = this->nick;
    if(this->user.empty()){
        tmp += "";
    }
    else{
        tmp += "!~" + this->user;
    }
    if(this->ip.empty()){
        tmp += "";
    }
    else{
        tmp += "@" + this->ip;
    }
    return (tmp);
}

Client& Client::operator=(Client const &copy)
{
    this->chatMessages = copy.chatMessages;
    this->client_fd = copy.client_fd;
    this->client_port = copy.client_port;
    this->ip = copy.ip;
    this->user = copy.user;
    this->nick = copy.nick;
    this->pass = copy.pass;
    return *this;
}
