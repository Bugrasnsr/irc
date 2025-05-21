#include "Client.hpp"
#include "Server.hpp"
#include "Utilities.hpp"

int Server::Pass(std::string &input, Client& c) {
    if (input.empty()) {
        Utilities::transmitReply(c.client_fd, ERR_NEEDMOREPARAMS(c.nick, "PASS"));
        return 0;
    }
    if (!c.pass.empty()) {
        Utilities::transmitReply(c.client_fd, ERR_ALREADYREGISTRED(c.user));
        return 0;
    }
    if (this->password == input) {
        c.passCheck = 1;
        c.pass = input;
        std::cout << "Password is correct!" << std::endl;
        Utilities::transmitReply(c.client_fd, "Registration is Successful!\n");
        return 1;
    } else {
        Utilities::transmitReply(c.client_fd, ERR_PASSWDMISMATCH(c.user));
        return 0;
    }
}