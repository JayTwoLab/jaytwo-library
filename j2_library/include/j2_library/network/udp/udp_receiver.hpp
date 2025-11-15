#pragma once

#include "j2_library/export.hpp"
#include "j2_library/network/ethernet.hpp"

namespace j2::network::udp {

    class J2LIB_API udp_receiver {
    public:
        using Callback = std::function<void(const std::string&, const std::string&, uint16_t)>;

        enum class IpVersion { IPv4, IPv6 };

    protected:
        int server_socket;
        std::atomic<bool> is_running;
        std::thread receiver_thread;

        Callback on_receive;

        static constexpr int BUFFER_SIZE = 1024;

        enum class UdpType { None, Unicast, Multicast, Broadcast };
        UdpType udpType;

        std::string receiver_ip;
        unsigned short receiver_port;

        std::string any_address;

        bool enable_reuse_port;
        IpVersion ip_version; // IPv4 or IPv6

    public:
        udp_receiver(IpVersion version = IpVersion::IPv4);
        ~udp_receiver();

        void setEnableReusePort(bool enable);
        void setAnyAddress(const std::string anyAddr);

        // New: Set IP version (IPv4 or IPv6)
        void setIpVersion(IpVersion version);

        bool startUnicast(const std::string& ip, const unsigned short port);
        bool startMulticast(const std::string multicast_group, const unsigned short port);
        bool startBroadcast(const unsigned short port);

        void setOnReceiveCallback(Callback cb);
        void quit();

    protected:
        bool start(const std::string& ip, const unsigned short port,
                   const bool enable_multicast = false,
                   const std::string& multicast_group = "",
                   const bool enable_broadcast = false);

        void receiveLoop();
    };

}  // namespace j2::network::udp

