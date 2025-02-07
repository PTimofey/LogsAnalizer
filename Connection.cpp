#include "Connection.hpp"



void Connection::Send(const Message& msg)
{
    std::cout<<"Connection::Send\n";
   context.post([this,msg](){

        OutgoingMessages.push_back(msg);
        if(!OutgoingMessages.empty())
        {
            Write_Head();
        }
   });
}

bool Connection::IsConnected()
{
    return socket.is_open();
}

void Connection::Disconnect()
{
    if(IsConnected())
    {
        boost::asio::post(context,[this](){socket.close();});
    }
};

void Connection::connect_to_client()
{
    if(ObjType==Connection::type::server && socket.is_open())
    {
        Read_Head();
    }   
}

void Connection::connect_to_server(boost::asio::ip::tcp::resolver::results_type& endpoint)
{
    boost::asio::async_connect(socket,endpoint,[this](std::error_code ec, boost::asio::ip::tcp::endpoint endP){
        if(!ec)
        {
            Read_Head();
        }
        else
        {
            std::cout<<" faild to connect\n";
        }
    });
}