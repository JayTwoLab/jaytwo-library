#pragma once

// Windows network header setup
#include "j2_library/network/win_net_shim.hpp" 

// Common network utilities and type definitions
#include "j2_library/network/ethernet.hpp"

// Include boost/asio or standalone asio headers and define namespace/type aliases
// #if defined(ASIO_STANDALONE)
//     #include <asio.hpp>
//     namespace net = asio;
//     using asio_error_code_t = std::error_code;
// #else
//     #include <boost/asio.hpp>
//     #include <boost/system/error_code.hpp>
//     namespace net = boost::asio;
//     using asio_error_code_t = boost::system::error_code;
// #endif

// TCP-related utilities and type definitions
#include "j2_library/network/tcp/tcp_server.hpp"
#include "j2_library/network/tcp/tcp_client.hpp"

// UDP-related utilities and type definitions
#include "j2_library/network/udp/udp_receiver.hpp"
#include "j2_library/network/udp/udp_sender.hpp"

// Rest API (using curl)
// #include "j2_library/network/rest/curl_get_client.hpp"
// #include "j2_library/network/rest/curl_post_client.hpp"

// FTP/SFTP client utilities and type definitions (using curl)
// #include "j2_library/network/ftp/ftp_client.hpp"

// File downloader utilities and type definitions
// #include "j2_library/network/downloader/downloader.hpp"





