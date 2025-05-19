#include "Server.hpp"
#include "Utilities.hpp"
#include "Replies.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <algorithm>


int Server::Quit(std::string &input, Client& c)
{

	ClientIterator c_iter;
	for (c_iter = this->connectedClients.begin(); c_iter != this->connectedClients.end(); ++c_iter) {
		if (c_iter->nick == c.nick)
			break;
	}
	if (c_iter == this->connectedClients.end())
		return -1;

	for(ChannelIterator it = activeChannels.begin(); it != activeChannels.end(); ++it)
	{
		if((*it).is_client_in_room(c))
		{
			for (std::vector<Client>::iterator client_it = (*it).room_client.begin(); client_it != (*it).room_client.end(); )
			{
				if (client_it->nick == c.nick)
					client_it = (*it).room_client.erase(client_it);
				else
					++client_it;
			}
			updateClientGuiWithMembersBetter(c, *it);
		}
		Utilities::listChannelUsers(*it);
	}
	Utilities::transmitReply(c.client_fd, RPL_QUIT(c.nick, input.c_str()));
	removeClient(c_iter);
	return 0;
}
