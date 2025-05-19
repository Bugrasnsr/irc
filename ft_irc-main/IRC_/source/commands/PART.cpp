#include "Server.hpp"
#include "Utilities.hpp"
#include "Replies.hpp"
#include "Client.hpp"
#include "Channel.hpp"


int Server::Part(std::string &input, Client& c)
{
    std::string roomName;
    for (int i = 0; input[i]; i++) {
        if (input[i] == ' ')
            break;
        roomName += input[i];
    }
    input = roomName;
    Channel *ch = fetchChannel(input);
    if (input.size() <= 1 || input[0] != '#') {
        Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, input));
        return 0;
    }
    else if (ch)
    {        
        for (ClientIterator it = ch->room_client.begin(); it != ch->room_client.end(); ++it) {
            if (c.user == (*it).user) {
                Utilities::transmitReply(c.client_fd, RPL_PART(c.nick, input));
                ch->room_client.erase(it);
                updateClientGuiWithMembersBetter(c, *ch);
                if (ch->room_client.empty()) {
                    return 0;
                }
                break;
            }
        }
    }
    else  
        Utilities::transmitReply(c.client_fd, ERR_NOSUCHCHANNEL(c.nick, input.c_str()));
    return 0;
}
