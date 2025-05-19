#pragma once
#include <iostream>
#include <string>
#include "Server.hpp"
#include <stdlib.h>
#include <Channel.hpp>
#include <sstream>

# define BLACK		"\033[0;30m"
# define RED		"\033[0;31m"
# define GREEN		"\033[0;32m"
# define YELLOW		"\033[0;33m"
# define BLUE		"\033[0;34m"
# define PURPLE		"\033[0;35m"
# define CYAN		"\033[0;36m"
# define WHITE		"\033[0;37m"
# define END		"\033[m"
# define RESET		"\033[0m"


# define BUG 1


class Utilities{

    private:
        Utilities();
        ~Utilities();

    public:
        static void transmitReply(int fd, std::string message);
        static void notifyAllClients(std::vector<int> fd, std::string message);
        static void sendMessageToAll(std::vector<int> const&, std::string const&);
        static int portNumberInRange(char* port_num);
        static std::vector<std::string> divideAtFirstSpace(const std::string& input);
        static std::vector<std::string> divideStringByNewline(const std::string& str);
        static std::vector<std::string> split_string(const std::string& input, char delimiter);
        static std::vector<std::string> tokenizeCommand(std::string& cmd);
        static std::string getProjectInfoMessage(void);
        static void listChannelUsers(Channel& channel);
        static bool isValidChannelName(std::string& s);

    template<typename Iterator>
    static std::string add_strings(Iterator begin, Iterator end, const std::string& delimiter)
    {
        std::ostringstream result;
        if (begin != end)
            result << *begin++;
        while (begin != end)
            result << delimiter << *begin++;
        return result.str();
    }
};
