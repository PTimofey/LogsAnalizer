#pragma once
#include<string>
#include<boost/asio.hpp>
#include<boost/json.hpp>
#include<iostream>



class HeadMessage
{
public:
    uint32_t size;
};

class Message
{
public:

    Message()=default;
    Message(std::string msg)
    {
        head.size = msg.size();
        JSON_message=boost::json::parse(msg);
        JSON_buffer.assign(msg.begin(), msg.end());
    }

    friend Message& operator << (Message& msg, const std::string& str) 
    {
        msg.JSON_message=boost::json::parse(str);
        return msg;
    }

    friend std::ostream& operator << (std::ostream& os, const Message& msg)
    {
        os<<boost::json::serialize(msg.JSON_message);
        return os;
    }

    HeadMessage head;
    boost::json::value JSON_message;
    std::vector<char> JSON_buffer;
};

struct Message_and_IDconnection
{
    Message msg;
    const uint16_t ID;
};