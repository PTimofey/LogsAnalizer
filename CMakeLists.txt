cmake_minimum_required(VERSION 3.10)
project(MyBoostJsonAsioProject)


find_package(Boost COMPONENTS json REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

set(HEADERS
Message.hpp
Connection.hpp
Node.hpp
Manager/Server.cpp
Manager/Server.hpp
)

set(SRC
Analizer.cpp
Connection.cpp
)

add_executable(Server ${SRC} ${HEADERS})
target_link_libraries(Server Boost::json)


add_executable(Client ClientNode.cpp Client/Client.hpp Client/Client.cpp Connection.hpp Connection.cpp Message.hpp Node.hpp)
target_link_libraries(Client Boost::json)