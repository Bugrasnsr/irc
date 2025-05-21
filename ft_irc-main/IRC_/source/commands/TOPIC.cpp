#include "Server.hpp"
#include "Client.hpp"
#include "Utilities.hpp"
#include "Replies.hpp"
#include "Channel.hpp"

int Server::Topic(std::string &input, Client& c)
{
    std::vector<std::string> params = Utilities::split_string(input, ' ');

    if (params.size() < 1) {
        Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "TOPIC"));
        return -1;
    }
    std::string roomName = params[0];
    std::string topicContent;
    if (params.size() > 1) {
        topicContent = Utilities::add_strings(params.begin() + 1, params.end(), " ");
        if (!topicContent.empty() && topicContent[0] == ':') {
            topicContent.erase(0, 1);
        }
    }

    for (std::vector<Channel>::iterator it = activeChannels.begin(); it != activeChannels.end(); ++it)
    {
        if (it->name_of_room == roomName)
        {
            // Kullanıcı kanalda mı?
            bool isInChannel = false;
            for (size_t i = 0; i < it->room_client.size(); ++i) {
                if (it->room_client[i].user == c.user) {
                    isInChannel = true;
                    break;
                }
            }
            if (!isInChannel) {
                Utilities::transmitReply(c.client_fd, ERR_NOTONCHANNEL(c.nick, roomName));
                return 0;
            }

            // Sadece operator değiştirebilir mi?
            if (!it->topic_settable) {
                if (it->room_client[0].user == c.user) {
                    it->topic = topicContent;
                    std::cout << "TOPIC : " << it->topic << std::endl;
                    std::vector<int> fds = it->get_fd();
                    if (!fds.empty()) {
                        Utilities::sendMessageToAll(fds, RPL_TOPIC(c.nick, c.ip_adress, roomName, topicContent));
                    }
                } else {
                    Utilities::transmitReply(c.client_fd, ERR_TOPICNOTSETTABLE(c.nick, roomName));
                }
                return 0;
            }
            // Herkes değiştirebilir
            it->topic = topicContent;
            std::cout << "TOPIC : " << it->topic << std::endl;
            std::vector<int> fds = it->get_fd();
            if (!fds.empty()) {
                Utilities::sendMessageToAll(fds, RPL_TOPIC(c.nick, c.ip_adress, roomName, topicContent));
            }
            return 0;
        }
    }
    // Kanal bulunamadıysa
    Utilities::transmitReply(c.client_fd, ERR_NOSUCHCHANNEL(c.nick, roomName));
    return -1;
}