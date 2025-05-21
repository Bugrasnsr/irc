#include "Utilities.hpp"
#include "Server.hpp"
#include "Client.hpp"

Utilities::Utilities(){}
Utilities::~Utilities(){}

int Utilities::portNumberInRange(char* port_num){ //max port number

    int pN = atoi(port_num);

    for(int i = 0;port_num[i];i++){
        if(!isdigit(port_num[i]))
            return 0;
    }

    if (pN < 0 || pN > 65535)
        return 0;
    return 1;
}

int Server::get_max_fd() const{
    int maxFd = this->server_file_descriptor;
    for(std::vector<Client>::const_iterator begin = this->connectedClients.begin(); begin != this->connectedClients.end(); begin++)
    {
        if (maxFd < (*begin).client_fd)
            maxFd = (*begin).client_fd;
    }
    return (maxFd);
}

// filepath: [Utilities.cpp](http://_vscodecontentref_/3)
std::vector<std::string> Utilities::divideAtFirstSpace(const std::string& input) {
    std::vector<std::string> result;
    std::string temp = input;
    if (!input.empty() && input[input.size()-1] == '\n') {
        temp = input.substr(0, input.size() - 1);
    }
    size_t spacePos = temp.find(' ');
    if (spacePos != std::string::npos) {
        result.push_back(temp.substr(0, spacePos));
        result.push_back(temp.substr(spacePos + 1));
    } else {
        result.push_back(temp);
        result.push_back("");
    }
    return result;
}

std::vector<std::string> Utilities::split_string(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type end = input.find(delimiter);

    while (end != std::string::npos) {
        if (end != start) {
            result.push_back(input.substr(start, end - start));
        }
        start = end + 1;
        end = input.find(delimiter, start);
    }

    if (start < input.length()) {
        result.push_back(input.substr(start));
    }

    return result;
}

void Utilities::transmitReply(int fd, std::string message){
    if (write(fd, message.c_str(), message.length()) < 0){
        std::cout << "Message cannot be sent!" << std::endl;
    }
}

void Utilities::sendMessageToAll(std::vector<int> const& fd, std::string const& message)
{
    for (std::vector<int>::const_iterator it = fd.begin(); it != fd.end(); ++it) {
        Utilities::transmitReply((*it), message);
    }
}

void Utilities::notifyAllClients(std::vector<int> fd, std::string message){
    for(std::vector<int>::iterator it = fd.begin(); it != fd.end(); ++it){
        Utilities::transmitReply((*it), message);
    }
}

std::vector<std::string> Utilities::divideStringByNewline(const std::string& str) {
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type end;
    while ((end = str.find('\n', start)) != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    // Son satırı da ekle (eğer varsa)
    if (start < str.length())
        result.push_back(str.substr(start));
    return result;
}

void Server::updateClientGuiWithMembers(Client &client, Channel &channel) {
    
    if (!channel.is_client_in_room(client)) {
        std::cerr << "Error: Client is not a member of the channel." << std::endl;
        return;
    }
    if(client.client_fd < 0 || channel.room_client.empty())
        return;

    std::string msg;
    Channel* tmp = fetchChannel(channel.name_of_room);


    if (!tmp)
        return;
    for (std::vector<Client>::iterator it = tmp->room_client.begin(); it != tmp->room_client.end(); ++it) {
        if(it == tmp->room_client.begin())
            msg += "@" + it->nick + " ";
        else
            msg += ' ' + it->nick + " ";
    }    
    Utilities::notifyAllClients(tmp->get_fd(), RPL_NAMREPLY(client.nick, tmp->name_of_room, msg));
    Utilities::notifyAllClients(tmp->get_fd(), RPL_ENDOFNAMES(client.nick, tmp->name_of_room));

}

void Server::updateClientGuiWithMembersBetter(Client &client, Channel &channel) {
    

    if(client.client_fd < 0 || channel.room_client.empty())
        return;

    std::string msg;
    Channel* tmp = fetchChannel(channel.name_of_room);


    if (!tmp)
        return;
    for (std::vector<Client>::iterator it = tmp->room_client.begin(); it != tmp->room_client.end(); ++it) {
        if(it == tmp->room_client.begin())
            msg += "@" + it->nick + " ";
        else
            msg += ' ' + it->nick + " ";
    }    
    Utilities::notifyAllClients(tmp->get_fd(), RPL_NAMREPLY(client.nick, tmp->name_of_room, msg));
    Utilities::notifyAllClients(tmp->get_fd(), RPL_ENDOFNAMES(client.nick, tmp->name_of_room));

}

std::vector<std::string> Utilities::tokenizeCommand(std::string& cmd) {
    std::vector<std::string> result;
    std::string::size_type start = 0, end;
    while ((end = cmd.find(' ', start)) != std::string::npos) {
        result.push_back(cmd.substr(start, end - start));
        start = end + 1;
    }
    if (start < cmd.length())
        result.push_back(cmd.substr(start));
    return result;
}

void Server::removeClient(ClientIterator& it)
{
    std::cout << RED << "Client " << it->client_fd - 3  << " disconnected!" << RESET << std::endl;
    FD_CLR(it->client_fd, &read_fds);
    FD_CLR(it->client_fd, &write_fds);
    close(it->client_fd);
    connectedClients.erase(it);
}

int Server::doesChannelExist(std::string const& roomName)
{
    for (ChannelIterator it = activeChannels.begin(); it != activeChannels.end(); ++it) {
        if (it->name_of_room == roomName)
            return 1;
    }
    return 0;
}

bool Utilities::isValidChannelName(std::string& s) {
    if(s.size() < 2 || s.size() > 50)
        return false;
    for(int i = 0; s[i]; i++) {
        if(s[i] == ' ' || s[i] == 7 || s[i] == ',')
            return false;
    }
    return true;
}

void Utilities::listChannelUsers(Channel& channel)
{
	std::cout << YELLOW << "< CHANNEL INFO >" << RESET << std::endl;
	for(std::vector<Client>::const_iterator it = channel.room_client.begin(); it != channel.room_client.end(); ++it) {
		const Client& client = *it;
		std::cout << CYAN << "Channel :  " << channel.name_of_room << std::endl;
		std::cout << "User : " << client.user << std::endl;
		std::cout << "Nickname : " << client.nick << RESET <<std::endl;
        std::cout << std::endl;
        std::cout << YELLOW << "OPERATOR : " << channel.room_client[0].user << RESET << std::endl;
	}
}

std::string Utilities::getProjectInfoMessage(void){
    std::string msg;

    msg += "========================================\n";
    msg += "      🚀 42 Ecole IRC Project 🚀\n";
    msg += "========================================\n";
    msg += "  Developed by:\n";
    msg += "    • Mustafa Buğra Sansar\n";
    msg += "    • Berat Öntürk\n";
    msg += "    • Burak Öntürk\n";
    msg += "----------------------------------------\n";
    msg += "  Project Information:\n";
    msg += "    Start Date : 02/04/2025\n";
    msg += "    End Date   : ../05/2025\n";
    msg += "----------------------------------------\n";
    msg += "  Welcome to our custom IRC server!\n";
    msg += "  Enjoy real-time chat, channels, and\n";
    msg += "  a true 42 spirit of collaboration.\n";
    msg += "----------------------------------------\n";
    msg += "  Thank you for using our project!\n";
    msg += "========================================\n";
    msg += "         End of /INFO list\n";
    msg += "========================================\n";

    return msg;
}
