#include "Server.hpp"
#include <algorithm>

int Server::isClientInChannel(Client &c, std::string roomName){
    for(ChannelIterator it = this->activeChannels.begin(); it != this->activeChannels.end(); ++it){
        if(it->name_of_room == roomName){
            for(ClientIterator cliIt = it->room_client.begin(); cliIt != it->room_client.end(); ++cliIt){
                if(cliIt->client_fd == c.client_fd)
                    return 1;
            }
            break;
        }
    }
    return 0;
}

int Server::Notice(std::string &arg, Client& c){
    std::vector<std::string> param = Utilities::tokenizeCommand(arg);
    if (param.size() < 2)
        return 0;

    std::string target = param[0];
    std::string message = param[1];
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    // Aynı kullanıcıya birden fazla mesaj gitmesini engellemek için flag
    std::vector<int> notified_fds;

    for(ClientIterator it = connectedClients.begin(); it != connectedClients.end(); ++it){
        bool shouldSend = false;
        if(it->nick == target || it->user == target) {
            shouldSend = true;
        } else if(target[0] == '#' && isClientInChannel(*it, target) && c.nick != it->nick) {
            shouldSend = true;
        }
        if(shouldSend && std::find(notified_fds.begin(), notified_fds.end(), it->client_fd) == notified_fds.end()) {
            (*it).chatMessages.push_back(RPL_NOTICE(c.buildPrefixString(), target, message));
            FD_SET((*it).client_fd, &this->write_fds);
            notified_fds.push_back(it->client_fd);
        }
    }
    return 0;
}