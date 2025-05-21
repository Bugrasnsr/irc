#include "Server.hpp"
#include "Client.hpp"
#include "Utilities.hpp"
#include "Replies.hpp"
#include <iostream>

Channel::Channel(const std::string& roomName) : name_of_room(roomName), topic_settable(false), room_limit(0) {}
Channel::Channel() : topic("") {}
Channel::~Channel() {}

std::vector<int> Channel::get_fd() const {
    std::vector<int> ret;
    for (size_t i = 0; i < room_client.size(); ++i) {
        ret.push_back(room_client[i].client_fd);
    }
    return ret;
}

Channel* Server::fetchChannel(const std::string &name) {
    for (size_t i = 0; i < activeChannels.size(); ++i) {
        if (activeChannels[i].name_of_room == name) {
            return &activeChannels[i];
        }
    }
    return NULL;
}

bool Channel::is_client_in_room(Client& c) {
    for (size_t i = 0; i < room_client.size(); ++i) {
        if (room_client[i].user == c.user) {
            return true;
        }
    }
    return false;
}