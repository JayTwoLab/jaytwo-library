cmake_minimum_required(VERSION 3.26)

# tcp 
add_subdirectory(tcp_server)
add_subdirectory(tcp_client)

# udp 
add_subdirectory(udp_receiver)
add_subdirectory(udp_sender)

# rest api
add_subdirectory(rest_get)



