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
#include <coroutine>



class MessageHandler;
class AbstractAnalizer
{
public:
    friend class MessageHandler;
    AbstractAnalizer(){}
    ~AbstractAnalizer(){}

    virtual std::string Analize(const Message_and_IDconnection&& ProcessingMessage)=0;

    const std::deque<std::shared_ptr<Node>>& GetNodes() noexcept {return Connections;} 
    const std::deque<std::shared_ptr<Node>>& GetNotActiveNodes() noexcept {return LostConnections;}
    void SetCurrentConnection(std::shared_ptr<Node>& CurCon) noexcept {CurrentConnection=CurCon;}
    const std::shared_ptr<Node> GetCurrentConnection() noexcept {return CurrentConnection;}


private:
    std::shared_ptr<Node> CurrentConnection;

    inline static std::deque<std::shared_ptr<Node>> Connections;
    inline static std::deque<std::shared_ptr<Node>> LostConnections;
};



class SSH_ConnectionAnalizer : public AbstractAnalizer
{
private:
    
public:

    SSH_ConnectionAnalizer() {}
    
    ~SSH_ConnectionAnalizer(){}

    std::string Analize(const Message_and_IDconnection&& ProcessingMessage) override
    {
        const std::deque<std::shared_ptr<Node>> *ActiveUsers=&GetNodes();
        const std::deque<std::shared_ptr<Node>> *NotActiveUsers=&GetNotActiveNodes();

        std::string IP = ProcessingMessage.msg.JSON_message.at("Inforamtion").at("IP").as_string().data();

        ResultOfAnalysis.as_object()["IP"]=IP;
        ResultOfAnalysis.as_object()["port"]=ProcessingMessage.msg.JSON_message.at("Inforamtion").at("port").as_string().data();

        if((IP.substr(0,7)=="192.168") || (IP.substr(0,6)=="172.16") || (IP.substr(0,1)=="10"))
        {
            //Public IP address
            ResultOfAnalysis.as_object()["IPtype"]="public";
        }
        else
        {
            //Private IP address
            ResultOfAnalysis.as_object()["IPtype"]="private";
        }

        for(const auto& it : *ActiveUsers)
        {
            if(it->ConnectionToRemoteNode->IPv4==IP)
            {
                ResultOfAnalysis.as_object()["user"]="Active";
                goto Next;
            }
        }
        for(const auto& it : *NotActiveUsers)
        {
            if(it->ConnectionToRemoteNode->IPv4==IP)
            {
                ResultOfAnalysis.as_object()["user"]="NotActive";
                break;
            }
        }
        Next:
        auto Action = ActionsScenarious.find(ProcessingMessage.msg.JSON_message.as_object().at("Text").as_string().data());
    }

private:

    const std::unordered_map<std::string, std::function<void()>> ActionsScenarious={{"Successful connection", [&](){std::cout<<"[Successful connection] from "<<ResultOfAnalysis.as_object()["IP"]<<"to "<<GetCurrentConnection()->ConnectionToRemoteNode->IPv4;}}};
    

    boost::json::value ResultOfAnalysis;

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
    MessageHandler(std::deque<std::shared_ptr<Node>>& _connections, std::deque<std::shared_ptr<Node>>& _LostConnections) : Connections(_connections), LostConnections(_LostConnections)
    {
        AbstractAnalizer::Connections=_connections;
        AbstractAnalizer::LostConnections=_LostConnections;
    }

    ~MessageHandler()
    {}

    void HandleMessage(Message_and_IDconnection& ProcessingMessage)
    {
        auto ProcessingObject = (TypesOfAnalizers.find(ProcessingMessage.msg.JSON_message.at("Deamon").as_string().data())->second)();
        for(auto& it : Connections)
        {
            if(ProcessingMessage.ID==it->ID)
            {
                ProcessingObject->SetCurrentConnection(it);
            }
        }
        ProcessingObject->Analize(std::move(ProcessingMessage));
    }

    
private:

    const std::unordered_map<std::string, std::function<std::shared_ptr<AbstractAnalizer>()>> TypesOfAnalizers={
        {"sshd",[](){return std::make_shared<SSH_ConnectionAnalizer>();}},
        {"Placeholder",[](){return std::make_shared<Test>();}}};

    std::deque<std::shared_ptr<Node>> Connections;
    std::deque<std::shared_ptr<Node>> LostConnections;
    
};


