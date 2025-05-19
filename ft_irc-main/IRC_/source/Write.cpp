#include "Server.hpp"
#include "Client.hpp"

void Server::processClientWrites()
{
    for(std::vector<Client>::iterator it = this->connectedClients.begin(); it != this->connectedClients.end() && state; ++it){
        if(FD_ISSET((*it).client_fd, &this->fd_write_tmp)){
            int readed = write((*it).client_fd, (char *)(*it).chatMessages[0].c_str(), (*it).chatMessages[0].length());
            (*it).chatMessages.erase((*it).chatMessages.begin());

            if((*it).chatMessages.empty())
                FD_CLR((*it).client_fd, &this->write_fds);
            if(readed <= 0){
                FD_CLR((*it).client_fd, &this->read_fds);
                FD_CLR((*it).client_fd, &this->write_fds);
                close((*it).client_fd);
                this->connectedClients.erase(it);
                std::cout << RED << "CS: "<< this->connectedClients.size() << ", A client disconnected!" << RESET << std::endl;
            }
            state = 0;
            break;
        }
    }
}