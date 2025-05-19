#include "Server.hpp"

int Server::Info(std::string &s, Client& c){
    (void)s;

    Utilities::transmitReply(c.client_fd, RPL_INFO(c.nick, Utilities::getProjectInfoMessage()));
    return 1;
}