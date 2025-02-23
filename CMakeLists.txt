cmake_minimum_required(VERSION 3.10)
project(MyBoostJsonAsioProject)


find_package(Boost COMPONENTS json REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

set(HEADERS
Message.hpp
Connection.hpp
Node.hpp
Manager/Server.hpp
Manager/MessageHandler.hpp
)

set(SRC
Manager/Server.cpp
Analizer.cpp
Connection.cpp
)

add_executable(Server ${SRC} ${HEADERS})

target_link_libraries(Server Boost::json)

set_target_properties(
    Server PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)


add_executable(Client ClientNode.cpp Client/Client.hpp Client/Client.cpp Connection.hpp Connection.cpp Message.hpp Node.hpp)

set_target_properties(
    Client PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(Client Boost::json)