#include "Server.hpp"
#include "Channel.hpp"
#include "Utilities.hpp"

int Server::Privmsg(std::string &input, Client& c)
{
    // Hedef ve mesajı ayır
    std::string target, message;
    std::string::size_type pos = input.find(' ');
    if (pos != std::string::npos) {
        target = input.substr(0, pos);
        message = input.substr(pos + 1);
    } else {
        target = input;
        message = "";
    }

    // Hedef veya mesaj yoksa hata döndür
    if (target.empty()) {
        Utilities::transmitReply(c.client_fd, ERR_NORECIPIENT(c.user));
        return 0;
    }
    if (message.empty()) {
        Utilities::transmitReply(c.client_fd, ERR_NOTEXTTOSEND(c.user));
        return 0;
    }

    // Mesaj başında ':' varsa temizle
    if (!message.empty() && message[0] == ':')
        message = message.substr(1);

    // Kanal mesajı mı?
    if (target[0] == '#') {
        Channel* ch = fetchChannel(target);
        if (!ch) {
            Utilities::transmitReply(c.client_fd, ERR_NOSUCHCHANNEL(c.user, target));
            return 0;
        }
        if (!ch->is_client_in_room(c)) {
            Utilities::transmitReply(c.client_fd, ERR_CANNOTSENDTOCHAN(c.user));
            return 0;
        }
        std::string mes = RPL_PRIVMSG(c.user, target, message);
        sendChannelMessage(c, mes, *ch);
    } else {
        // Kişiye özel mesaj
        std::string mes = RPL_PRIVMSG(c.nick, target, message);
        bool found = false;
        for (std::vector<Client>::iterator it = connectedClients.begin(); it != connectedClients.end(); ++it) {
            if ((*it).nick == target) {
                (*it).chatMessages.push_back(mes);
                FD_SET((*it).client_fd, &this->write_fds);
                found = true;
                break;
            }
        }
		if (!found) {
   			 Utilities::transmitReply(c.client_fd, ERR_NOSUCHNICK(c.user));
		}
    }
    return 0;
}