#include "bot.hpp"

Bot::Bot(int port, std::string pass) : Port(port), Password(pass), chatbotUser("Bot"), botNickname("Bot")
{
    std::cout << "Bot created with nick: -" << this->botNickname << "-, user: -" << this->chatbotUser << "-, password: -" << this->Password << "-, port: -" << this->Port << "-" << std::endl;
    this->createBotSocket();
    this->runBot();
}

std::vector<std::string> Bot::splitByFirstSpace(const std::string& input)
{
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

void Bot::createBotSocket(void)
{
    this->bot_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (this->bot_file_descriptor < 0)
        throw std::runtime_error("Bot Socket Couldnt Be Created!");

    std::cout << "Bot Socket is Created :)" << std::endl;

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP Address!" << std::endl;
        close(this->bot_file_descriptor);
        throw std::runtime_error("Invalid IP Address!");
    }

    if (connect(this->bot_file_descriptor, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bot Connection To The Server Failed! Error: " << strerror(errno) << std::endl;
        close(this->bot_file_descriptor);
        throw std::runtime_error("Bot Connection To The Server Failed!");
    }

    std::cout << "Bot Connection To The Server Successfull :)" << std::endl;
}

void Bot::runBot(void)
{
    int flag = 0;
    std::cout << "Bot Executing!" << std::endl;
    while (1)
    {
        std::string msgTmp;
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int readed = recv(this->bot_file_descriptor, buffer, sizeof(buffer) - 1, 0);
        if(readed <= 0)
        {
            std::cerr << "Bot Disconnected!" << std::endl;
            break;
        }
        // Satır sonu karakterlerini temizle
        if (readed > 0 && (buffer[readed - 1] == '\n' || buffer[readed - 1] == '\r'))
            buffer[readed - 1] = '\0';
        else
            buffer[readed] = '\0';

        msgTmp = buffer;
        std::cout << "Bot Received: " << msgTmp << std::endl;

        if(!flag)
        {
            std::string login = "PASS " + this->Password + "\nUSER " + this->chatbotUser + " 0 * :Bot\nNICK " + this->botNickname + "\n";
            size_t ret = write(this->bot_file_descriptor, login.c_str(), login.size());
            if((int)ret < 0)
            {
                std::cerr << "Bot Login Failed!" << std::endl;
                break;
            }
            flag = 1;
        }

        if(msgTmp.empty())
            continue;
        MessageAnalyzer(msgTmp);
    }
}

void Bot::MessageAnalyzer(const std::string &message)
{
    std::vector<std::string> parts = splitByFirstSpace(message);
    std::string target = parts[0];
    if (!target.empty() && target[0] == ':')
        target = target.substr(1);

    std::cout << "Bot Target: " << target << std::endl;
    std::cout << "INCOMING INPUT: " << message << std::endl;

    if(message.find("PING") != std::string::npos)
    {
        std::string msg = "PRIVMSG " + target + " PONG\r\n";
        send(this->bot_file_descriptor, msg.c_str(), msg.size(), 0);
    }
    else if(message.find("PRIVMSG") != std::string::npos)
    {
        if(message.find("Hello") != std::string::npos)
        {
            std::string msg = "PRIVMSG " + target + " Wassup Mate!\r\n";
            send(this->bot_file_descriptor, msg.c_str(), msg.size(), 0);
        }
        if(message.find("Sassy") != std::string::npos)
        {
            std::string msg = "PRIVMSG " + target + " Wadiyatalkinabeet!\r\n";
            send(this->bot_file_descriptor, msg.c_str(), msg.size(), 0);
        }
        if(message.find("Date") != std::string::npos)
        {
            std::time_t currentTime = std::time(0);
            char* dt = std::ctime(&currentTime);
            std::string msg = "PRIVMSG " + target + " " + (std::string)dt + "\r\n";
            send(this->bot_file_descriptor, msg.c_str(), msg.size(), 0);
        }
        if(message.find("Bye") != std::string::npos)
        {
            std::string msg = "PRIVMSG " + target + " See ya later!\r\n";
            send(this->bot_file_descriptor, msg.c_str(), msg.size(), 0);
        }
    }
}

Bot::~Bot()
{
    close(this->bot_file_descriptor);
    std::cout << "X Bot Destroyed X" << std::endl;
}