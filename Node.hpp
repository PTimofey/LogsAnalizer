#pragma once
#include<set>
#include<map>
#include<memory>
#include"Connection.hpp"
#include"Message.hpp"
#include<map>


enum class StateOfNode
{
    ACTIVE,
    CONNECTING,
    SLEEPING,
    STOPPED
};

enum class TypeOfNode
{
    CLIENT,
    SERVER
};

class Node
{
public:
    Node(std::shared_ptr<Connection> connection, StateOfNode st) : ConnectionToRemoteNode(connection), state(st)
    {}


    
    TypeOfNode type;
    StateOfNode state;

    unsigned int ID;
    unsigned int ReceivedMessages;
    unsigned int SentMessages;

    std::shared_ptr<Connection> ConnectionToRemoteNode;

    

};