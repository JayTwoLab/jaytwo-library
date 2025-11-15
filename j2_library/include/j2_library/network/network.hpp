#pragma once

#include "j2_library/network/win_net_shim.hpp" // 윈도우 네트워크 헤더 셋업

// boost/asio 또는 standalone asio 헤더 포함 및 네임스페이스 별칭/타입 정의
#if defined(ASIO_STANDALONE)
    #include <asio.hpp>
    namespace net = asio;
    using asio_error_code_t = std::error_code;
#else
    #include <boost/asio.hpp>
    #include <boost/system/error_code.hpp>
    namespace net = boost::asio;
    using asio_error_code_t = boost::system::error_code;
#endif

  
#include "j2_library/network/ethernet.hpp"

#include "j2_library/network/tcp/tcp_server.hpp"
#include "j2_library/network/tcp/tcp_client.hpp"

#include "j2_library/network/udp/udp_receiver.hpp"
#include "j2_library/network/udp/udp_sender.hpp"

