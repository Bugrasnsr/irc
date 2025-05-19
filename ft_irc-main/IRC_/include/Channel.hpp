#pragma once
#include "Server.hpp"
#include "Client.hpp"
#include "Utilities.hpp"
#include "Replies.hpp"

class Client;
class Server;

class Channel
{
	public:
		std::string name_of_room;
		std::vector<Client> room_client;
		
		bool mode;
		std::string topic;
		bool topic_settable; //Channel Mode Topic
		unsigned int room_limit;
		std::string room_key;

		Channel(const std::string& roomName);
		Channel();
		~Channel();
		bool is_client_in_room(Client& c);
		std::vector<int>    get_fd() const;


};
