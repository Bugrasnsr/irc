#include "Server.hpp"
#include "Utilities.hpp"
#include "Exception.hpp"

void Server::initializeServerSocket(void)
{
    this->server_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if ((int)(this->server_file_descriptor) < 0)
    {
        throw std::runtime_error("Socket is not created!");

    }
    else
    {
        std::cout << GREEN << "Socket is created." << RESET << std::endl;
    }

    if (fcntl(this->server_file_descriptor, F_SETFL, O_NONBLOCK) < 0) {
        close(this->server_file_descriptor);
        throw std::runtime_error("Failed to set socket to non-blocking mode");
    }
    int opt = 1;

    if (setsockopt(this->server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        throw std::runtime_error("Socket is not optimized!");

    }
    else
    {
        std::cout << GREEN << "Socket is optimized." << RESET << std::endl;
    }
}

void Server::bindServerAddress(void) {

    memset(&this->address, 0, sizeof(sockaddr_in));
    this->address.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(this->port_num); // port number

    if (bind(this->server_file_descriptor, (sockaddr *)&this->address, sizeof(this->address)) < 0)
    {
        throw std::runtime_error("Socket is not bound!");

    }
    else
    {
        std::cout << GREEN << "Socket is bound." << RESET << std::endl;
    }
}

/*
    how many sockets can you listen at the same time ->128
*/
void Server::startListening(void) const{

    if (listen(this->server_file_descriptor, 128) < 0)
    {
        throw std::runtime_error("Server socket cannot hear you!");

    }
    else
    {
        std::cout << GREEN << "Server socket is listening you now." << RESET << std::endl;
    }
}


