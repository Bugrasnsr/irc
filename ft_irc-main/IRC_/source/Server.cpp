#include "Server.hpp"
#include "Client.hpp"

Server::Server(){}

Server::Server(size_t port_no, const std::string passwd) : port_num(port_no), password(passwd){
    std::cout << this->port_num << " " << this->password << std::endl;

    initializeServerSocket();
    initialize_command_handlers();
    bindServerAddress();
    startListening();
    run();
}

void Server::run(void){

    this->state = 0;

    FD_ZERO(&this->read_fds);
    FD_ZERO(&this->write_fds);
    FD_ZERO(&this->fd_read_tmp);
    FD_ZERO(&this->fd_write_tmp);

    FD_SET(this->server_file_descriptor, &this->read_fds);

    while (1)
    {
        while(this->state == 0)
        {
            this->fd_read_tmp = this->read_fds;
            this->fd_write_tmp = this->write_fds;
            state = select(this->get_max_fd() + 1, &this->fd_read_tmp, &this->fd_write_tmp, NULL, 0);
        }
        acc_client_connection();
        manageClientInput();
        processClientWrites();
    }
}

Client* Server::get_client_by_nick(std::string &user_nick)
{   
    if(user_nick.empty())
        return NULL;
    for(ClientIterator it = connectedClients.begin(); it != connectedClients.end() ; ++it)
    {
        if(it->nick == user_nick)
        {
            return &(*it);
        }
    }
    return NULL;
}

void Server::sendChannelMessage(Client& c, std::string& message, Channel &channel)
{
    for (std::vector<Client>::iterator it = channel.room_client.begin(); it != channel.room_client.end(); ++it)
    {
        if ((*it).user == c.user)
            continue;
        for(std::vector<Client>::iterator it2 = connectedClients.begin(); it2 != connectedClients.end(); ++it2){
            if((*it).user == (*it2).user){
                (*it2).chatMessages.push_back(message);
                FD_SET((*it2).client_fd, &this->write_fds);
            }
        }

    }
}

void Server::initialize_command_handlers(void)
{
    cmds["CAP"] = &Server::Cap;
    cmds["PASS"] = &Server::Pass;
    cmds["USER"] = &Server::User;
    cmds["NICK"] = &Server::Nick;
    cmds["PRIVMSG"] = &Server::Privmsg;
    cmds["JOIN"] = &Server::Join;
    cmds["MODE"] = &Server::Mode;
    cmds["INFO"] = &Server::Info;
    cmds["TOPIC"] = &Server::Topic;
    cmds["NOTICE"] = &Server::Notice;
    cmds["KICK"] = &Server::Kick;
    cmds["PART"] = &Server::Part;
    cmds["QUIT"] = &Server::Quit;
}

Server::~Server()
{
    close(this->server_file_descriptor);
}