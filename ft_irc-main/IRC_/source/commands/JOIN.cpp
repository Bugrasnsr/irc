#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

bool channelExists(std::string &name, std::vector<Channel> channel){
	for(ChannelIterator it = channel.begin(); it != channel.end(); ++it){
		if(name == it->name_of_room)
			return true;
	}
	return false;
}

int Server::Join(std::string &cmd, Client& c)
{
	std::vector<std::string> splitResult = Utilities::divideAtFirstSpace(cmd);
	std::string ch_name, ch_key;

	if (splitResult.size() == 2) {
		ch_name = splitResult[0];
		ch_key = splitResult[1];
	}
	
	if(c.user.empty() || c.nick.empty()){
		Utilities::transmitReply(c.client_fd, "ERROR: You must enter a username and a nickname first!\n");
		return 0;
	}

	if(!Utilities::isValidChannelName(ch_name)) {
		Utilities::transmitReply(c.client_fd, ERR_NOSUCHCHANNEL(c.buildPrefixString(), ch_name));
		return 0;
	}

	if(channelExists(ch_name, this->activeChannels))
	{
		for(ChannelIterator it = this->activeChannels.begin(); it != this->activeChannels.end(); ++it)
		{
		
			if(ch_name == it->name_of_room)
			{
				if(it->is_client_in_room(c)){
					Utilities::transmitReply(c.client_fd, ERR_ALREADYREGISTRED(c.user));
					return 0;
				}
				if(it->room_limit != 0 && it->room_client.size() >= it->room_limit) {
					Utilities::transmitReply(c.client_fd, ERR_CHANNELISFULL(c.nick, ch_name));
					return 0;
				}
				if(!it->room_key.empty())
				{
					{
						if(ch_key.empty())
						{
							Utilities::transmitReply(c.client_fd, ERR_BADCHANNELKEY(c.nick, ch_name));
							return 0;
						}
						if(ch_key != it->room_key)
						{
							Utilities::transmitReply(c.client_fd, ERR_PASSWDMISMATCH(c.nick));
							return 0;
						}
					}
				}
				it->room_client.push_back(c);
				std::string topicName = it->name_of_room + " :" + it->topic;
				Utilities::transmitReply(c.client_fd, RPL_JOIN(c.nick, c.ip_adress, ch_name));
				this->updateClientGuiWithMembers(c, *it);
				std::cout << "TOPIC :  " << it->topic << std::endl;
				std::vector<int> fds = it->get_fd();
				if (!fds.empty() && !it->topic.empty()) {
					Utilities::sendMessageToAll(fds, RPL_TOPIC(c.nick, c.ip_adress, it->name_of_room, it->topic));
				}
				else
					Utilities::transmitReply(c.client_fd, RPL_NOTOPIC(c.nick, it->name_of_room));
			}
			Utilities::listChannelUsers(*it);
		}
	}
	else {
		Channel a(ch_name);
		a.room_client.push_back(c);
		this->activeChannels.push_back(a);
		Utilities::transmitReply(c.client_fd, RPL_JOIN(c.nick, c.ip_adress, ch_name));
		this->updateClientGuiWithMembers(c, this->activeChannels.back());
	}
	return 0;
}
