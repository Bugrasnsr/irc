#include "Server.hpp"
#include "Client.hpp"

void Server::manageClientInput() {
    Client cl;
    for(std::vector<Client>::iterator begin = this->connectedClients.begin(); begin != this->connectedClients.end() && this->state; ++begin){
        if(FD_ISSET((*begin).client_fd, &this->fd_read_tmp)){
            int readed = read((*begin).client_fd, this->buffer, 1024);
            if(readed <= 0) {
                FD_CLR((*begin).client_fd, &this->read_fds);
                FD_CLR((*begin).client_fd, &this->write_fds);
                close((*begin).client_fd);
                this->connectedClients.erase(begin);
                std::cout << RED << "CS: "<< this->connectedClients.size() << ", " << (*begin).nick << " client disconnected!" <<RESET << std::endl;
            }
            else {
                this->buffer[readed] = '\0';
                std::string k = this->buffer;
                if (k == "\n") {
                    state = 0;
                    break;
                }
                // ^D
               if(k.length() > 0) {
                    if (k[k.length() - 1] != '\n') {
                        (*begin).buffer += k;
                        state = 0;
                        break;
                    }
                }
                std::vector<std::string> lines = Utilities::divideStringByNewline((*begin).buffer + buffer);
                for(size_t i = 0; i < lines.size(); i++){
                    if(lines[i].empty())
                        continue;
                    if(lines[i][lines[i].size() - 1] == '\r')
                        lines[i] = lines[i].substr(0, lines[i].size() - 1);
                    std::cout << BLUE << "[ CMD ] " <<  RESET << PURPLE << "[ "<< lines[i] << " ]" << RESET << std::endl; 
                    std::vector<std::string> all = Utilities::divideAtFirstSpace(lines[i]);
                    if (cmds.find(all[0]) != cmds.end())
                    {
                        ((this->*cmds[all[0]])(all[1], (*begin)));
                    }
                    else{
                        std::cout << "Command Not Found!" << std::endl;
                    }
                }
                if(!(Utilities::tokenizeCommand(lines[0])[0] == "CAP" && lines.size() == 1)){
                    if (!(*begin).passCheck) {
                        FD_CLR((*begin).client_fd, &this->read_fds);
                        FD_CLR((*begin).client_fd, &this->write_fds);
                        std::cout << RED << (*begin).client_fd - 3 << " YOU MUST ENTER THE PASSWORD!" << RESET << std::endl;
                        std::cout << RED << "CS: "<< this->connectedClients.size() << ", " << " client disconnected!" <<RESET << std::endl;
                        close((*begin).client_fd);
                        this->connectedClients.erase(begin);
                    }
                }
            }

            this->state = 0;
            break;
        }
    }
}
