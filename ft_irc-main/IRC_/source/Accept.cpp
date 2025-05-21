#include "Server.hpp"
#include "Client.hpp"

Client Server::createClient(int client_fd, const sockaddr_in& clientAddr) {
    Client client;
    client.client_fd = client_fd;
    client.client_port = ntohs(clientAddr.sin_port);
    inet_ntop(AF_INET, &(clientAddr.sin_addr), client.ip_adress, INET_ADDRSTRLEN);
    return client;
}

void Server::acc_client_connection() {
    if (!FD_ISSET(this->server_file_descriptor, &this->fd_read_tmp))
        return;

    sockaddr_in clientAddr;
    socklen_t client_size = sizeof(sockaddr_in);

    int client_fd = accept(this->server_file_descriptor, (sockaddr *)&clientAddr, &client_size);
    if (client_fd < 0) {
        throw std::runtime_error("Accept failed!");
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
        close(client_fd);
        throw std::runtime_error("Failed to set non-blocking mode!");
    }

    Client newClient = createClient(client_fd, clientAddr);
    this->connectedClients.push_back(newClient);
    FD_SET(client_fd, &this->read_fds);

    std::cout << GREEN << "CS: " << this->connectedClients.size() << ", New Client Connected!" << RESET << std::endl;
    Utilities::transmitReply(client_fd, "Password please!\n");
    this->state = 0;
}