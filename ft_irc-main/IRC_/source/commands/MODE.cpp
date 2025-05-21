#include "Server.hpp"
#include "Client.hpp"
#include "Utilities.hpp"
#include "Replies.hpp"

int Server::Mode(std::string &input, Client& c)
{
    std::vector<std::string> cmd;
    std::string tmp;
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == ' ') {
            if (!tmp.empty()) {
                cmd.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += input[i];
        }
    }
    if (!tmp.empty())
        cmd.push_back(tmp);

    if (cmd.size() < 2) {
        Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "MODE"));
        return 0;
    }

    std::string channel = cmd[0];
    std::string mode = cmd[1];
    Channel *ch = fetchChannel(channel);

    if (!ch) {
        Utilities::transmitReply(c.client_fd, ERR_NOSUCHCHANNEL(c.nick, channel));
        return 0;
    }

    // Sadece kanal operatorü işlem yapabilir
    if (ch->room_client.empty() || ch->room_client[0].user != c.user) {
        Utilities::transmitReply(c.client_fd, ERR_CHANOPRIVSNEEDED(c.nick, channel));
        return 0;
    }

    if (mode.size() < 2)
        return 0;

    // +o/-o: Operator atama/çıkarma
    if (mode[1] == 'o') {
        if (mode[0] == '+') {
            if (cmd.size() < 3) {
                Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "MODE"));
                return 0;
            }
            std::string user_nick = cmd[2];
            Client *newOprt = get_client_by_nick(user_nick);
            if (!newOprt) {
                Utilities::transmitReply(c.client_fd, ERR_NOSUCHNICK(c.nick));
                return 0;
            }
            if (!ch->is_client_in_room(*newOprt)) {
                Utilities::transmitReply(c.client_fd, ERR_USERNOTINCHANNEL(c.nick, user_nick, channel));
                return 0;
            }
            // Operatorü başa al
            for (ClientIterator it = ch->room_client.begin(); it != ch->room_client.end(); ++it) {
                if (it->user == newOprt->user) {
                    Client found = *it;
                    if (it != ch->room_client.begin()) {
                        ch->room_client.erase(it);
                        ch->room_client.insert(ch->room_client.begin(), found);
                    }
                    break;
                }
            }
            std::cout << YELLOW << "+o NEW OPERATOR: " << newOprt->user << RESET << std::endl;
            Utilities::transmitReply(c.client_fd, RPL_MODE(c.nick, channel, "+o " + user_nick, "O"));
            updateClientGuiWithMembers(*newOprt, *ch);
        } else if (mode[0] == '-') {
            if (ch->room_client.size() > 1) {
                ClientIterator it = ch->room_client.begin();
                Client found = *it;
                ch->room_client.erase(it);
                ch->room_client.push_back(found);
                std::cout << YELLOW << "+o NEW OPERATOR: " << ch->room_client[0].nick << RESET << std::endl;
                updateClientGuiWithMembers(c, *ch);
            } else {
                Utilities::transmitReply(c.client_fd, "You are the only one in that channel!");
            }
        }
    }
    // +t/-t: Topic değiştirme izni
    else if (mode[1] == 't') {
        if (mode[0] == '+') {
            ch->topic_settable = true;
            Utilities::transmitReply(c.client_fd, RPL_TOPICSETTABLE(c.nick, ch->name_of_room));
        } else if (mode[0] == '-') {
            ch->topic_settable = false;
            Utilities::transmitReply(c.client_fd, RPL_TOPICNOTSETTABLE(c.nick, ch->name_of_room));
        }
    }
    // +l/-l: Kanal limiti
    else if (mode[1] == 'l') {
        if (mode[0] == '+') {
            if (cmd.size() < 3) {
                Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "MODE"));
                return 0;
            }
            std::string lmt = cmd[2];
            ch->room_limit = atoi(lmt.c_str());
            Utilities::transmitReply(c.client_fd, RPL_MODE(c.nick, channel, "+l " + lmt, "+L"));
        } else if (mode[0] == '-') {
            ch->room_limit = 0;
            Utilities::transmitReply(c.client_fd, RPL_MODE(c.nick, channel, "-l ", "-L"));
        }
    }
    // +k/-k: Kanal anahtarı
    else if (mode[1] == 'k') {
        if (mode[0] == '+') {
            if (cmd.size() < 3) {
                Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "MODE"));
                return 0;
            }
            std::string key = cmd[2];
            if (!ch->room_key.empty()) {
                Utilities::transmitReply(c.client_fd, ERR_KEYSET(c.nick, ch->name_of_room));
                return 0;
            }
            ch->room_key = key;
            std::cout << "|THIS IS THE KEY!: " << ch->room_key << "|" << std::endl;
            Utilities::transmitReply(c.client_fd, RPL_MODE(c.nick, channel, "+k " + key, "K"));
        } else if (mode[0] == '-') {
            ch->room_key = "";
            std::cout << " THIS IS THE KEY!: " << ch->room_key << std::endl;
            Utilities::transmitReply(c.client_fd, RPL_MODE(c.nick, channel, "-k ","-K"));
        }
    }
    return 0;
}