#include "Server.hpp"
#include "Client.hpp"

int Server::isNickExist(std::string s) {
    for(std::vector<Client>::iterator it = this->connectedClients.begin() ; it != this->connectedClients.end(); ++it) {
        if(it->nick == s)
            return 1;
    }
    return 0;
}

int checkInput(std::string &nickName){
    for(std::string::iterator it = nickName.begin(); it != nickName.end(); ++it){
        if(!isalnum(*it))
            return 0;
    }
    return 1;
}

int Server::Nick(std::string &s, Client &c) {// NICK <nickname>
    if(!checkInput(s)){
        Utilities::transmitReply(c.client_fd, ERR_ERRONEUSNICKNAME(c.user, c.nick));
        return 0;
    }
    if(s.size() < 1){
        Utilities::transmitReply(c.client_fd, ERR_NONICKNAMEGIVEN(c.user));
        return 0;
    }
    if(!s.empty() && !isNickExist(s)){
        c.nick = s;
        c.chatMessages.push_back(RPL_NICK(c.nick, c.user, c.ip, s));
        FD_SET(c.client_fd, &this->write_fds);
    }
    else{
        if (s.empty())
            Utilities::transmitReply(c.client_fd, ERR_NICKNAMEEMPTY(c.nick));
        else if(isNickExist(s))
            Utilities::transmitReply(c.client_fd, ERR_NICKNAMEINUSE(c.nick));
    }
    return 1;
}
