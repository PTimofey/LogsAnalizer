#include"Client.hpp"



void client_interface::connect(std::string& host, const uint16_t port)
{
    try
    {
        boost::asio::ip::tcp::resolver resolver(m_context);
        boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

        m_connection = std::make_unique<Connection>(m_context, boost::asio::ip::tcp::socket(m_context), IncomingMessages, Connection::type::node);

        m_connection->connect_to_server(endpoints);
        thrContext=std::thread([this](){m_context.run();});

    }
    catch(const std::exception& e)
    {
        std::cerr<< "Client Exception " << e.what() << '\n';
    }
}


bool client_interface::IsConnected()
{
    if(m_connection)
        return m_connection->IsConnected();
    else
    {
        return false;
    }
}


void client_interface::Disconnect()
{
    if(IsConnected())
    {
        m_connection->Disconnect();
    }
    m_context.stop();

    if(thrContext.joinable())
    {
        thrContext.join();
    }
    m_connection.release();
}

void client_interface::Send(const Message& msg)
{
    if(IsConnected())
    {
        std::cout<<msg.head.size<<' '<<"client_interface::Send\n";
        m_connection->Send(msg);
    }
}