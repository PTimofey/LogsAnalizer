#pragma once
#include"../Node.hpp"
#include<deque>
#include<iostream>
#include<unordered_map>
#include<string>
#include<utility>
#include <atomic>
#include <functional>
#include <memory>




class MessageHandler;
class AbstractAnalizer
{
public:
    friend class MessageHandler;
    AbstractAnalizer(){}
    ~AbstractAnalizer(){}

    virtual std::string Analize(const Message_and_IDconnection&& ProcessingMessage)=0;

    const std::deque<std::shared_ptr<Node>>& GetNodes() noexcept {return Connections;} 



private:    
    inline static std::deque<std::shared_ptr<Node>> Connections;
};



class SSH_ConnectionAnalizer : public AbstractAnalizer
{
private:
    
public:

    SSH_ConnectionAnalizer() {}
    
    ~SSH_ConnectionAnalizer(){}

    std::string Analize(const Message_and_IDconnection&& ProcessingMessage) override
    {
        std::string IP = ProcessingMessage.msg.JSON_message.at("Inforamtion").at("IP").as_string().data();
        if((IP.substr(0,7)=="192.168") || (IP.substr(0,6)=="172.16") || (IP.substr(0,1)=="10"))
        {
            //Public IP address
            ResulOfAnalysis.as_object()["type"]="public";
        }
        else
        {
            //Private IP address
            ResulOfAnalysis.as_object()["type"]="private";
        }
    }

private:
    boost::json::value ResulOfAnalysis;

};




class Test : public AbstractAnalizer
{
public: 
    Test() {}
    ~Test(){countOfAnalize=0;}


    std::string Analize(const Message_and_IDconnection&& ProcessingMessage)override
    {
        ++countOfAnalize;
        return std::to_string(countOfAnalize);
    }

private:
    std::atomic<long long> countOfAnalize;
};
class MessageHandler
{
public:
    MessageHandler(std::deque<std::shared_ptr<Node>>& _connections, std::deque<std::shared_ptr<Node>>& _LostedConnections) : Connections(_connections), LostedConnections(_LostedConnections)
    {
        AbstractAnalizer::Connections=_connections;
    }

    ~MessageHandler()
    {}

    void HandleMessage(Message_and_IDconnection& ProcessingMessage)
    {
        auto ProcessingObject = (TypesOfAnalizers.find(ProcessingMessage.msg.JSON_message.at("Deamon").as_string().data())->second)();
        ProcessingObject->Analize(std::move(ProcessingMessage));
        
        
    }

    
private:

    const std::unordered_map<std::string, std::function<std::shared_ptr<AbstractAnalizer>()>> TypesOfAnalizers={
        {"sshd",[](){return std::make_shared<SSH_ConnectionAnalizer>();}},
        {"Placeholder",[](){return std::make_shared<Test>();}}};

    std::deque<std::shared_ptr<Node>> Connections;
    std::deque<std::shared_ptr<Node>> LostedConnections;
    
};


