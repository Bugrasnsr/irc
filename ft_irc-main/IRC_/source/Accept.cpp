#include "Server.hpp"
#include "Client.hpp"

void Server::acc_client_connection() {
    if (!FD_ISSET(this->server_file_descriptor, &this->fd_read_tmp))
        return ;
    sockaddr_in clientAddr;
    socklen_t client_size;
    Client temp;

    client_size = sizeof(sockaddr_in);
    temp.client_fd = accept(this->server_file_descriptor, (sockaddr *)&clientAddr, &client_size);
    if (temp.client_fd < 0) { 
        throw std::runtime_error("Accept failed!");
    }
    fcntl(temp.client_fd, F_SETFL, O_NONBLOCK);
    temp.client_port = ntohs(clientAddr.sin_port);
    inet_ntop(AF_INET, &(clientAddr.sin_addr), temp.ip_adress, INET_ADDRSTRLEN);
    this->connectedClients.push_back(temp);
    FD_SET(temp.client_fd, &this->read_fds);
    std::cout << GREEN << "CS: "<< this->connectedClients.size() << ", New Client Connected!" << RESET << std::endl;
    Utilities::transmitReply(temp.client_fd, "Password please!\n");
    this->state = 0;
}