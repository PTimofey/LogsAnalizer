#pragma once
#include<boost/asio.hpp>
#include<queue>
#include<string>
#include"Message.hpp"
#include"Conteiners.hpp"
#include<thread>
#include<mutex>
#include<list>
#include<algorithm>





class Connection
{
public:


    enum class type
    {
        server,
        node
    };
    Connection(boost::asio::io_context& io, boost::asio::ip::tcp::socket _socket, AsyncQueue<Message_and_IDconnection>& InMessages, type T) : 
    context(io), socket(std::move(_socket)), IncomingMessages(InMessages), ObjType(T) {}

    bool IsConnected();

    void Disconnect();

    void Send(const Message& msg);

    void connect_to_client();

    void connect_to_server(boost::asio::ip::tcp::resolver::results_type& endpoint);



    uint16_t ID;

protected:

    void Write_Head()
    {
        
        boost::asio::async_write(socket,boost::asio::buffer(&OutgoingMessages.front().head,sizeof(OutgoingMessages.front().head)),[this](std::error_code ec, std::size_t length){
            if(!ec)
            {
                if(OutgoingMessages.front().head.size>0)
                {

                    std::string tmpString=boost::json::serialize(OutgoingMessages.front().JSON_message);
                    std::vector<char> MessagBuffer(tmpString.begin(), tmpString.end());

                    Write_Message(MessagBuffer);
                }
                else
                {
                    OutgoingMessages.pop_front();
                    Write_Head();
                }
            }
            else
            {
                std::cerr<<"Write Size of Message fail\n";
                socket.close();
            }
        });
    }
    void Write_Message(const std::vector<char>& SendBuffer)
    {
        std::cout<<"Send Message: \n";
        boost::asio::async_write(socket, boost::asio::buffer(SendBuffer.data(), OutgoingMessages.front().head.size), 
        [this](std::error_code ec, std::size_t length){
        
            OutgoingMessages.pop_front();
            if(!ec)
            {
                if(!OutgoingMessages.empty())
                {
                    Write_Head();
                }
            }
            else
            {
                std::cerr<<"Write Message fail\n";
                socket.close();
            }
            
        });
    }

    void Read_Head()
    {
        boost::asio::async_read(socket,boost::asio::buffer(&tempMessage.head, sizeof(tempMessage.head)),
        [&](std::error_code ec, std::size_t length){
            if(!ec)
            {
                if(tempMessage.head.size>0)
                {
                    std::cout<<tempMessage.head.size<<'\n';
                    tempMessage.JSON_buffer.resize(tempMessage.head.size);
                    Read_Body();
                }
                else
                {
                    AddToIncoming(tempMessage);
                }
            }
            else
            {
                //std::cerr<<"Read head fail\n";
                //socket.close();
                std::string ErrorMessage = '{'+"<\"Read head fail\">"+'}';
                tempMessage<<ErrorMessage;
                AddToIncoming(tempMessage);
            }
        });
    }

    void AddToIncoming(Message& tmp)
    {
        if(ObjType==type::server)
        {
            IncomingMessages.push_back({tmp,ID});
            Read_Head();
        }
        else
        {
            IncomingMessages.push_back({tmp,0});
            Read_Head();
        }
    }

    void Read_Body()
    {
        
        boost::asio::async_read(socket,boost::asio::buffer(tempMessage.JSON_buffer.data(), tempMessage.JSON_buffer.size()),[this](std::error_code ec, std::size_t length){
            if(!ec)
            {
                std::cout<<std::string(tempMessage.JSON_buffer.begin(), tempMessage.JSON_buffer.end());
                AddToIncoming(tempMessage);
                
            }
            else
            {
                std::cerr<<"Read Body fail\n";
                socket.close();
            }
        });
    }
    

    Message tempMessage;
    type ObjType;
    boost::asio::io_context& context;  
    boost::asio::ip::tcp::socket socket;
    AsyncQueue<Message_and_IDconnection>& IncomingMessages;
    AsyncQueue<Message> OutgoingMessages;
};


