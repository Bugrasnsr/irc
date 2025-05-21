#include "Exception.hpp"
#include "Utilities.hpp"
#include "Server.hpp"
#include <iostream>

void printHelp() {
    std::cout << "Follow this instructions -> ./ircserv <port> <password>" << std::endl;
}

void validateArgs(int ac, char** av) {
    if (ac == 2 && std::string(av[1]) == "-h") {
        printHelp();
        exit(0);
    }
    if (ac != 3) {
        throw Exception("Follow this instructions -> ./ircserv <port> <password>");
    }
    if (!Utilities::portNumberInRange(av[1])) {
        throw Exception("Port number must be integer");
    }
}

void startServer(char** av) {
    Server server(atoi(av[1]), std::string(av[2]));
}

int main(int ac, char** av) {
    try {
        validateArgs(ac, av);
        startServer(av);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}