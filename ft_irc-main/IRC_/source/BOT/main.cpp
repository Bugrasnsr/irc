#include "bot.hpp"
#include <exception>

void printHelp() {
    std::cout << "Follow this instructions -> ./ircbot <port> <password>" << std::endl;
}

int portNumberInRange(char* port_num) {
    int pN = atoi(port_num);
    for (int i = 0; port_num[i]; i++) {
        if (!isdigit(port_num[i]))
            return 0;
    }
    if (pN < 0 || pN > 65535)
        return 0;
    return 1;
}

void validateArgs(int ac, char** av) {
    if (ac == 2 && std::string(av[1]) == "-h") {
        printHelp();
        exit(0);
    }
    if (ac != 3) {
        throw std::runtime_error("Follow this instructions -> ./ircbot <port> <password>");
    }
    if (!portNumberInRange(av[1])) {
        throw std::runtime_error("Port number must be integer");
    }
}

void startBot(char** av) {
    Bot bot(atoi(av[1]), std::string(av[2]));
}

int main(int ac, char **av) {
    try {
        validateArgs(ac, av);
        startBot(av);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}