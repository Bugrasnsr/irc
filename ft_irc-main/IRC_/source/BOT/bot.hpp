#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <ctime>

class Bot
{
	private:
		int Port; //unsidgned short??
		std::string Password;
		std::string chatbotUser;
		std::string botNickname;
		int bot_file_descriptor;
	public:
		Bot(int port, std::string pass);
		~Bot();
		// int getSocketFd() const;
		std::vector<std::string> splitByFirstSpace(const std::string& input);
		void createBotSocket(void);
		void runBot(void);
		void MessageAnalyzer(const std::string &message);
};