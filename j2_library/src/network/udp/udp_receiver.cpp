
#include "j2_library/network/udp/udp_receiver.hpp"

#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <mswsock.h>
#endif

namespace j2::network::udp {

udp_receiver::udp_receiver(IpVersion version)
    : is_running(false), server_socket(-1),
      receiver_ip(""), receiver_port(0), udpType(UdpType::None),
      any_address(version == IpVersion::IPv4 ? "0.0.0.0" : "::"),
      enable_reuse_port(true), ip_version(version) {
}

void udp_receiver::setIpVersion(IpVersion version) {
    ip_version = version;
    any_address = (version == IpVersion::IPv4) ? "0.0.0.0" : "::";
}

udp_receiver::~udp_receiver() {
    quit();
}

void udp_receiver::setEnableReusePort(bool enable) {
    enable_reuse_port = enable;
}

void udp_receiver::setAnyAddress(const std::string anyAddr) {
    any_address = anyAddr;
}

bool udp_receiver::startUnicast(const std::string& ip, const unsigned short port) {
    if (!start(ip, port)) {
        udpType = UdpType::None;
        receiver_ip.clear();
        receiver_port = 0;
        return false;
    }

    udpType = UdpType::Unicast;
    receiver_ip = ip;
    receiver_port = port;
    return true;
}

bool udp_receiver::startMulticast(const std::string multicast_group, const unsigned short port) {
    if (!start(any_address, port, true, multicast_group)) {
        udpType = UdpType::None;
        receiver_ip.clear();
        receiver_port = 0;
        return false;
    }

    udpType = UdpType::Multicast;
    receiver_ip = multicast_group;
    receiver_port = port;
    return true;
}

bool udp_receiver::startBroadcast(const unsigned short port) {
    if (!start(any_address, port, false, "", true)) {
        udpType = UdpType::None;
        receiver_ip.clear();
        receiver_port = 0;
        return false;
    }

    udpType = UdpType::Broadcast;
    receiver_ip = any_address;
    receiver_port = port;
    return true;
}

bool udp_receiver::start(const std::string& ip, const unsigned short port,
                        const bool enable_multicast,
                        const std::string& multicast_group,
                        const bool enable_broadcast) {
    int family = (ip_version == IpVersion::IPv4) ? AF_INET : AF_INET6;
    server_socket = static_cast<int>(socket(family, SOCK_DGRAM, 0));
    if (server_socket < 0) {
        perror("Socket creation failed");
        return false;
    }

    if (enable_reuse_port) {
        int reuse = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
            perror("Failed to set SO_REUSEADDR");
            return false;
        }
    }

    if (enable_broadcast && ip_version == IpVersion::IPv4) {
        int broadcast = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast)) < 0) {
            perror("Failed to enable broadcast");
            return false;
        }
    }

    if (enable_multicast) {
#ifdef _WIN32
        // Windows: IP_PKTINFO Replacement Feature Settings (IPv4 only)
        if (ip_version == IpVersion::IPv4) {
            GUID wsaRecvMsgGuid = WSAID_WSARECVMSG;
            LPFN_WSARECVMSG lpfnWSARecvMsg = nullptr;
            DWORD bytesReturned = 0;
            if (WSAIoctl(server_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &wsaRecvMsgGuid, sizeof(wsaRecvMsgGuid),
                &lpfnWSARecvMsg, sizeof(lpfnWSARecvMsg), &bytesReturned, nullptr, nullptr) == SOCKET_ERROR) {
                std::cerr << "WSAIoctl failed: " << WSAGetLastError() << std::endl;
                return false;
            }
        }
#else
        // Linux: activate IP_PKTINFO
        if (ip_version == IpVersion::IPv4) {
            int opt = 1;
            if (setsockopt(server_socket, IPPROTO_IP, IP_PKTINFO, &opt, sizeof(opt)) < 0) {
                std::cerr << "Failed to enable IP_PKTINFO" << std::endl;
                return false;
            }
        }
        // For IPv6, you may want to enable IPV6_RECVPKTINFO if needed
#endif
    }

    if (ip_version == IpVersion::IPv4) {
        sockaddr_in server_addr {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Bind failed");
#ifdef _WIN32
            closesocket(server_socket);
#else
            close(server_socket);
#endif
            return false;
        }

        if (enable_multicast && !multicast_group.empty()) {
            struct ip_mreq mreq {};
            inet_pton(AF_INET, multicast_group.c_str(), &mreq.imr_multiaddr);
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);

            if (setsockopt(server_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0) {
                perror("Failed to join multicast group");
#ifdef _WIN32
                closesocket(server_socket);
#else
                close(server_socket);
#endif
                return false;
            }
        }
    } else { // IPv6
        sockaddr_in6 server_addr6 {};
        server_addr6.sin6_family = AF_INET6;
        server_addr6.sin6_port = htons(port);
        inet_pton(AF_INET6, ip.c_str(), &server_addr6.sin6_addr);

        if (bind(server_socket, (struct sockaddr*)&server_addr6, sizeof(server_addr6)) < 0) {
            perror("Bind failed");
#ifdef _WIN32
            closesocket(server_socket);
#else
            close(server_socket);
#endif
            return false;
        }

        if (enable_multicast && !multicast_group.empty()) {
            struct ipv6_mreq mreq6 {};
            inet_pton(AF_INET6, multicast_group.c_str(), &mreq6.ipv6mr_multiaddr);
            mreq6.ipv6mr_interface = 0; // 0 = default interface

            if (setsockopt(server_socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (const char*)&mreq6, sizeof(mreq6)) < 0) {
                perror("Failed to join IPv6 multicast group");
#ifdef _WIN32
                closesocket(server_socket);
#else
                close(server_socket);
#endif
                return false;
            }
        }
    }

    is_running = true;
    receiver_thread = std::thread(&udp_receiver::receiveLoop, this);

    return true;
}

void udp_receiver::setOnReceiveCallback(Callback cb) {
    on_receive = std::move(cb);
}

void udp_receiver::quit() {
    if (is_running) {
        is_running = false;
        if (receiver_thread.joinable()) {
            receiver_thread.join();
        }

        if (server_socket >= 0) {
#ifdef _WIN32
            closesocket(server_socket);
#else
            close(server_socket);
#endif
            server_socket = -1;
        }
    }
}

void udp_receiver::receiveLoop() {
    char buffer[BUFFER_SIZE];

    if (ip_version == IpVersion::IPv4) {
        sockaddr_in client_addr {};
        socklen_t client_len = sizeof(client_addr);

        while (is_running) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recvfrom(server_socket,
                                          buffer, sizeof(buffer) - 1,
                                          0,
                                          (struct sockaddr*)&client_addr,
                                          &client_len);

            if (bytes_received < 0) {
                if (is_running) {
                    perror("Receive failed");
                }
                continue;
            }

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            uint16_t client_port = ntohs(client_addr.sin_port);

            if (on_receive) {
                on_receive(std::string(buffer, bytes_received), std::string(client_ip), client_port);
            }
        }
    } else { // IPv6
        sockaddr_in6 client_addr6 {};
        socklen_t client_len6 = sizeof(client_addr6);

        while (is_running) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recvfrom(server_socket,
                                          buffer, sizeof(buffer) - 1,
                                          0,
                                          (struct sockaddr*)&client_addr6,
                                          &client_len6);

            if (bytes_received < 0) {
                if (is_running) {
                    perror("Receive failed");
                }
                continue;
            }

            char client_ip[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &client_addr6.sin6_addr, client_ip, INET6_ADDRSTRLEN);
            uint16_t client_port = ntohs(client_addr6.sin6_port);

            if (on_receive) {
                on_receive(std::string(buffer, bytes_received), std::string(client_ip), client_port);
            }
        }
    }
}

}  // namespace j2::network::udp

