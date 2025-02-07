#include"../Connection.hpp"



class client_interface
{
public:
    ~client_interface()
    {
        Disconnect();
    }

    void connect(std::string& host, const uint16_t port);
    
    bool IsConnected();

    void Disconnect();

    void Send(const Message& msg);

    const AsyncQueue<Message_and_IDconnection>& GetMessages()
    {
        return IncomingMessages;
    }
    
protected:
    AsyncQueue<Message_and_IDconnection> IncomingMessages;
    boost::asio::io_context m_context;
    std::thread thrContext;
    std::unique_ptr<Connection> m_connection;
};

class LogSander : public client_interface
{
public:
    LogSander(std::string name, std::string host, const uint16_t port) : Name(name)
    {
        connect(host, port);
    } 

    void MessageStream()
    {
        std::string msg;
        while(true)
        { 
            std::cin>>msg;
            boost::json::value vl{{"Name",Name}, {"Message", msg}};
            Send(Message(boost::json::serialize(vl)));
            msg.clear();
        }
    }

private:
    std::string Name;

};