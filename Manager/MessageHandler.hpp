#pragma once
#include"../Message.hpp"
#include<thread>
#include<iostream>
#include<unordered_map>

class AbstractAnalizer;

class MessageHandler
{
public:
    MessageHandler(AsyncQueue<Message_and_IDconnection>& Queue)
    {}

    ~MessageHandler()
    {}

    void HandleMessage(const uint16_t ID, const Message& message)
    {
        //auto  ProcessingObject = TypesOfAnalizers.find(); 
    }

    
private:

    const std::unordered_map<std::string, std::function<AbstractAnalizer*()>> TypesOfAnalizers={
        {"AnalizerOfUsersOnHost",[](){return new AnalizerOfUsersOnHost();}}
    };

    AsyncQueue<Message_and_IDconnection>& MessagesToProcess;
    
};

class AbstractAnalizer
{
public:
    AbstractAnalizer(){}
    ~AbstractAnalizer(){}

    virtual void Analize(){}

};

class AnalizerOfUsersOnHost : public AbstractAnalizer
{
public:
    AnalizerOfUsersOnHost(){}

    ~AnalizerOfUsersOnHost(){}

    void Analize()
    {}

};