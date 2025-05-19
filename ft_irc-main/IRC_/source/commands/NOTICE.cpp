#include "Server.hpp"

int Server::isClientInChannel(Client &c, std::string roomName){
    int ret = 1;
    for(ChannelIterator it = this->activeChannels.begin(); it != this->activeChannels.end(); ++it, ++ret){
        if((*it).name_of_room == roomName){
            for(ClientIterator cliIt = (*it).room_client.begin(); cliIt != (*it).room_client.end(); ++cliIt){
                if((*cliIt).client_fd == c.client_fd)
                    return ret;
            }
            break;
        }
    }
    return 0;
}

int Server::Notice(std::string &arg, Client& c){
    std::vector<std::string> param = Utilities::tokenizeCommand(arg);
    
    for(ClientIterator it = connectedClients.begin(); it != connectedClients.end(); ++it){
        if((*it).nick == param[0] || (*it).user == param[0]){ // param[0] -> eyms
            (*it).chatMessages.push_back(RPL_NOTICE(c.buildPrefixString(), param[0], (param[1][0] == ':' ? param[1].substr(1, param[1].length()) : param[1])));
            FD_SET((*it).client_fd, &this->write_fds);
        }
        else if(isClientInChannel((*it), param[0]) && c.nick != it->nick) {
            (*it).chatMessages.push_back(RPL_NOTICE(c.buildPrefixString(), param[0], (param[1][0] == ':' ? param[1].substr(1, param[1].length()) : param[1])));
            FD_SET((*it).client_fd, &this->write_fds);
        }
    }
    return 0;

}