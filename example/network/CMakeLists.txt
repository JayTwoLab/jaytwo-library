cmake_minimum_required(VERSION 3.26)

# tcp 
add_subdirectory(tcp_server)
add_subdirectory(tcp_client)

# udp 
add_subdirectory(udp_receiver)
add_subdirectory(udp_sender)

# rest api (disable for now)
## add_subdirectory(rest_get)
## add_subdirectory(rest_post)

# ftp (disable for now)
## add_subdirectory(ftp_client)

# downloader (disable for now))
## add_subdirectory(downloader)


