#include"Client/Client.hpp"

#define PORT 1989



int main(int argc, char* argv[])
{
    std::cout<<argv[1];
    LogSander client("Bob",argv[1], PORT);

    client.MessageStream();
}