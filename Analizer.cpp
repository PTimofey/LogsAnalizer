#include<iostream>
#include<string>
#include"Connection.hpp"
#include"Node.hpp"
#include"Manager/Server.hpp"



#define PORT 1989


int main()
{
    
    NodesManipulator Serv(PORT);
    
    Serv.Work();
    return 0;
}