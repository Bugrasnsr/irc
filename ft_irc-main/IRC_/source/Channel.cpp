#include "Server.hpp"
#include "Client.hpp"
#include "Utilities.hpp"
#include "Replies.hpp"
#include <iostream>

Channel::Channel(const std::string& roomName) : name_of_room(roomName), topic_settable(false) , room_limit(0){}
Channel::Channel() : topic(""){}

Channel::~Channel() {}

std::vector<int>    Channel::get_fd() const {
	std::vector<int> ret;
	for(int i = 0; i < (int) this->room_client.size(); i++) {
		ret.push_back(this->room_client[i].client_fd);
	}
	return(ret);
}

Channel* Server::fetchChannel(const std::string &name) {
	for (std::vector<Channel>::iterator it = this->activeChannels.begin(); it != this->activeChannels.end(); ++it) {

		if (name == it->name_of_room) {
			return &(*it);
		}
	}
	return NULL;
}

bool Channel::is_client_in_room(Client& c)
{
	for (std::vector<Client>::iterator it = room_client.begin(); it != room_client.end(); ++it)
	{
		if (c.user == it->user)
		{
			return true;
		}
	}
	return false;
}
