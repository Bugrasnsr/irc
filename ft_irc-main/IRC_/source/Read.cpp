#include "Server.hpp"
#include "Client.hpp"

void Server::manageClientInput() {
    for (std::vector<Client>::iterator it = connectedClients.begin(); it != connectedClients.end() && state;) {
        if (FD_ISSET(it->client_fd, &fd_read_tmp)) {
            int readed = read(it->client_fd, buffer, 1024);
            if (readed <= 0) {
                FD_CLR(it->client_fd, &read_fds);
                FD_CLR(it->client_fd, &write_fds);
                close(it->client_fd);
                std::cout << RED << "CS: " << connectedClients.size() - 1 << ", " << it->nick << " client disconnected!" << RESET << std::endl;
                it = connectedClients.erase(it);
                state = 0;
                continue;
            } else {
                buffer[readed] = '\0';
                std::string k = buffer;
                if (k == "\n") {
                    state = 0;
                    break;
                }
                if (!k.empty() && k[k.size() - 1] != '\n') {
                    it->buffer += k;
                    state = 0;
                    break;
                }
                std::vector<std::string> lines = Utilities::divideStringByNewline(it->buffer + buffer);
                it->buffer.clear();
                for (size_t i = 0; i < lines.size(); i++) {
                    if (lines[i].empty())
                        continue;
                    if (!lines[i].empty() && lines[i][lines[i].size() - 1] == '\r')
                        lines[i].erase(lines[i].size() - 1);
                    std::cout << BLUE << "[ CMD ] " << RESET << PURPLE << "[ " << lines[i] << " ]" << RESET << std::endl;
                    std::vector<std::string> all = Utilities::divideAtFirstSpace(lines[i]);
                    if (!all.empty() && cmds.find(all[0]) != cmds.end()) {
                        std::string arg = (all.size() > 1) ? all[1] : "";
                        (this->*cmds[all[0]])(arg, *it);
                    } else {
                        std::cout << "Command Not Found!" << std::endl;
                    }
                }
                // lines boşsa şifre kontrolü yapma
                if (!lines.empty()) {
                    std::vector<std::string> firstCmd = Utilities::tokenizeCommand(lines[0]);
                    if (!(firstCmd.size() > 0 && firstCmd[0] == "CAP" && lines.size() == 1)) {
                        if (!it->passCheck) {
                            FD_CLR(it->client_fd, &read_fds);
                            FD_CLR(it->client_fd, &write_fds);
                            std::cout << RED << it->client_fd - 3 << " YOU MUST ENTER THE PASSWORD!" << RESET << std::endl;
                            std::cout << RED << "CS: " << connectedClients.size() - 1 << ", client disconnected!" << RESET << std::endl;
                            close(it->client_fd);
                            it = connectedClients.erase(it);
                            state = 0;
                            continue;
                        }
                    }
                }
            }
            state = 0;
            break;
        } else {
            ++it;
        }
    }
}