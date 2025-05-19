#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <sstream>
#include <string>

int Server::Kick(std::string &input, Client& c)
{
    std::string roomName;
    std::string userToKick;
    std::string kickReason;
    std::istringstream stream(input);
    stream >> roomName >> userToKick;
    
    // Get the rest of the input as the kick reason
    std::getline(stream, kickReason);
    if (!kickReason.empty() && kickReason[0] == ' ')
        kickReason = kickReason.substr(1);  // Remove leading space if present

    if (roomName.empty() || userToKick.empty())
    {
        Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "KICK"));
        return 0;
    }

    if (roomName.size() <= 1 || roomName[0] != '#') {
        Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "KICK"));
        return 0;
    }

    Channel *ch = fetchChannel(roomName);
    if (ch == NULL)
    {
        Utilities::transmitReply(c.client_fd, ERR_NOSUCHCHANNEL(c.nick, roomName));
        return 0;
    }

    if (ch->room_client.empty() || c.user != ch->room_client[0].user)
    {
        Utilities::transmitReply(c.client_fd, ERR_CHANOPRIVSNEEDED(c.nick, roomName));
        return 0;
    }

    bool userFound = false;

    if(userToKick == c.nick){
        Utilities::transmitReply(c.client_fd, "You cannot kick yourself!\n");
        return 0;
    }

    for (ClientIterator it2 = ch->room_client.begin(); it2 != ch->room_client.end(); ++it2)
    {

        if (it2->nick == userToKick)
        {
            userFound = true;
            // Use the provided kick reason, or "No reason given" if empty
            if(kickReason.empty()) {
                kickReason = "No reason given";
            }
            else {
                kickReason = kickReason.substr(1);
            }
            // Notify the kicked user
            Utilities::transmitReply(it2->client_fd, RPL_KICK(c.nick, roomName, userToKick, kickReason));
            
            // Notify other channel members
            for (ClientIterator it3 = ch->room_client.begin(); it3 != ch->room_client.end(); ++it3)
            {
                if (it3->nick != userToKick) // Don't send to the kicked user again
                {
                    Utilities::transmitReply(it3->client_fd, RPL_KICK(c.nick, roomName, userToKick, kickReason));
                }
            }
            
            Utilities::transmitReply(it2->client_fd, RPL_ENDOFNAMES(it2->nick, roomName));
            ch->room_client.erase(it2);
            updateClientGuiWithMembers(c, *ch);
            break;
        }
    }

    if (!userFound)
    {
        Utilities::transmitReply(c.client_fd, ERR_USERNOTINCHANNEL(c.nick, userToKick, roomName));
    }

    return 0;
}