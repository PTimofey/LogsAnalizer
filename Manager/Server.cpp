#include"Server.hpp"

void server_interface::stop()
{
    context.stop();
    if(contextThread.joinable())
    {
        contextThread.join();
    }
    std::cout<<"[SERVER]: stopped\n";
}

void server_interface::WaitForConnection()
    {
        contextAcceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket){
            if(!ec)
            {
                
                std::cout<<"[SERVER]: new connection "<<socket.remote_endpoint()<<'\n';
                std::shared_ptr<Connection> new_connection=std::make_shared<Connection>(context,std::move(socket), IncomingMessages, Connection::type::server);
                ++countID;
                new_connection->ID=countID;
                new_connection->IPv4=socket.remote_endpoint().data()->sa_data;
                deqConnections.push_back(std::make_shared<Node>(new_connection, StateOfNode::CONNECTING));
                deqConnections.back()->ConnectionToRemoteNode->connect_to_client();
            }
            else
            {
                std::cout<<"[SERVER]: connection denied\n";
            }
            WaitForConnection();
        });
    }


bool server_interface::Start()
{
    unsigned short WorkingHandlers=1;
    try
    {
        WaitForConnection();
        contextThread=std::thread([this](){context.run();});
    }
    catch(const std::exception& e)
    {
        std::cerr<<"[SERVER]: ERROR!!! " << e.what() << '\n';
        return false;
    }
    return true;
    
}


void server_interface::MessageNode(std::shared_ptr<Node>& node, const Message& message)
{
    if(node->ConnectionToRemoteNode->IsConnected())
    {
        node->ConnectionToRemoteNode->Send(message);
    }
    else
    {
        node.reset();
        deqConnections.erase(std::remove(deqConnections.begin(), deqConnections.end(), node), deqConnections.end());
    }
}


void server_interface::ForwardMessageToRestNodes(uint16_t senderID, const Message& message)
{
    for(auto& node : deqConnections)
    {
        if(node->ConnectionToRemoteNode->IsConnected())
        {
            if(node->ID!=senderID)
                {node->ConnectionToRemoteNode->Send(message);}
        }
        else
        {
            node.reset();
            deqConnections.erase(std::remove(deqConnections.begin(), deqConnections.end(), node), deqConnections.end());
        }
    }
} 


void server_interface::BroadcastMessage(const Message& message)
{   
    for(auto& node : deqConnections)
    {
        if(node->ConnectionToRemoteNode->IsConnected())
        {
            node->ConnectionToRemoteNode->Send(message);
        }
        else
        {
            node.reset();
            deqConnections.erase(std::remove(deqConnections.begin(), deqConnections.end(), node), deqConnections.end());
        }       
    }
}

void server_interface::Update(size_t MaxMsgs, bool bWaiting)
{
    if(bWaiting)
    {
        IncomingMessages.wait();
    }

    
    size_t msgCount=0;
    while (msgCount!=MaxMsgs && !IncomingMessages.empty())
    {
        size_t PrevQueueSize = IncomingMessages.size();
        OnMessage();
        ++msgCount;
    }
    
}