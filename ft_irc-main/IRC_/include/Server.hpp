#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Utilities.hpp"
#include "Exception.hpp"
#include "Replies.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <cerrno>
#include <cstring>

#define USERLEN 12

class Server;
class Client;
class Channel;

typedef std::vector<Client>::iterator ClientIterator;
typedef std::vector<Channel>::iterator ChannelIterator;
typedef int (Server::*command)(std::string& , Client&);

class Server
{
    private:
        size_t                  port_num;
        const std::string       password;
        size_t                  server_file_descriptor;
        sockaddr_in             address;
        std::vector<Client>     connectedClients;
        std::vector<Channel>    activeChannels;
        char                    buffer[1024];
        int                     state;


        fd_set read_fds;
        fd_set write_fds;
        fd_set fd_read_tmp;
        fd_set fd_write_tmp;

    private:

        void initializeServerSocket(void);
        void bindServerAddress(void);
        void startListening(void) const;
        void run(void);
        int get_max_fd(void) const;
        void acc_client_connection();
        void manageClientInput();
        void processClientWrites();

    public:
        Server();
        Server(size_t port_num, const std::string password);
        ~Server();

        void initialize_command_handlers(void);

        std::map<std::string, command> cmds;
        int Nick(std::string &input, Client& c);
        int Cap(std::string &input, Client& c);
        int Privmsg(std::string &input, Client& c);//operator
        int Join(std::string &input, Client& c);
        int Pass(std::string &input, Client& c);
        int User(std::string &input, Client& c);
        int Info(std::string &input, Client& c);
        int Notice(std::string &input, Client& c);

        int Mode(std::string &input, Client& c);
        int Topic(std::string &input, Client& c);//operator
        int Part(std::string &input, Client& c);
        int Quit(std::string &input, Client& c);
        int Kick(std::string &input, Client& c);

        void updateClientGuiWithMembers(Client &client, Channel &channel);//graphical user interface
        void updateClientGuiWithMembersBetter(Client &client, Channel &channel);
        Channel* fetchChannel(const std::string &name);
        void sendChannelMessage(Client& c, std::string& message, Channel& channel);
        Client* get_client_by_nick(std::string &nick);
        std::string getTopic(const std::vector<std::string>& params);
        int isNickExist(std::string s);
        void removeClient(ClientIterator &);
        int doesChannelExist(std::string const&);
        int isClientInChannel(Client &c, std::string roomName);//notice

        int isUserExist(std::string s);



};