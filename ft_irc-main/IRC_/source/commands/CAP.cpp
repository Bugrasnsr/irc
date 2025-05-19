#include "Server.hpp"
#include "Client.hpp"

int Server::Cap(std::string &input, Client &c)
{
    if (input == "LS\r")
        c.hex_or_numeric = HEX;
    else
        c.hex_or_numeric = NC;
    return 1;
}
