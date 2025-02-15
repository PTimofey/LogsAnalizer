#pragma once
#include "../Node.hpp"
#include "MessageHandler.hpp"
#include<thread>





class server_interface
{
public:
    server_interface(const uint16_t& port) : contextAcceptor(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {std::cout<<"server_interface\n";}

    ~server_interface()
    {
        stop();
    }
    void stop();

    void WaitForConnection();

    bool Start();
    

    void MessageNode(std::shared_ptr<Node>& node, const Message& message);



    void ForwardMessageToRestNodes(uint16_t senderID, const Message& message);


    void BroadcastMessage(const Message& message);


    void Update(size_t MaxMsgs=-1, bool bWaiting=false);

protected:

    virtual void OnMessage()=0;


    uint16_t countID=0;
    boost::asio::io_context context;
    boost::asio::ip::tcp::acceptor contextAcceptor;
    
    std::thread contextThread;
    std::deque<std::shared_ptr<Node>> deqConnections;
    std::deque<std::shared_ptr<Node>> deqLostedConnections;

    AsyncQueue<Message_and_IDconnection> IncomingMessages;
};


class NodesManipulator : public server_interface
{
public:
    NodesManipulator(const uint16_t& port) : server_interface(port), LogHandler(deqConnections,deqConnections)
    {

    }
    

    void Work()
    {
        Start();
    
        while (true)
        {
            Update(-1, true);
        }
    }
    ~NodesManipulator(){}

    void OnMessage() override
    {
        //LogHandler.HandleMessage();   
    }


private:


    MessageHandler LogHandler;

};

