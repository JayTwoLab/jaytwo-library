import socket
import struct
import datetime

def send_multicast(multicast_ip: str, multicast_port: int, message: str, ttl: int = 1):
    # Determine address family (IPv4 or IPv6)
    try:
        socket.inet_pton(socket.AF_INET, multicast_ip)
        family = socket.AF_INET
        proto = socket.IPPROTO_UDP
    except OSError:
        family = socket.AF_INET6
        proto = socket.IPPROTO_UDP

    sock = socket.socket(family, socket.SOCK_DGRAM, proto)

    # Set TTL for multicast packets
    if family == socket.AF_INET:
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)
    else:
        # For IPv6, set hop limit (same as TTL)
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, ttl)

    # Add timestamp to message (optional)
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    send_data = f"[{now}] {message}".encode("utf-8")

    # For IPv6, you may want to specify the interface (0 = default)
    if family == socket.AF_INET6:
        # (address, port, flowinfo, scope_id)
        # scope_id=0 means default interface; for link-local, set proper scope_id
        dest = (multicast_ip, multicast_port, 0, 0)
    else:
        dest = (multicast_ip, multicast_port)

    sock.sendto(send_data, dest)
    print(f"Multicast sent to {multicast_ip}:{multicast_port} -> {send_data}")

    sock.close()


if __name__ == "__main__":
    # IPv4 example
    MULTICAST_IP = "239.255.255.250"
    MULTICAST_PORT = 12347
    send_multicast(MULTICAST_IP, MULTICAST_PORT, "Hello Multicast!")

    # IPv6 example (loopback, port 12348)
    MULTICAST_IP_V6 = "ff3e::1234"   
    MULTICAST_PORT_V6 = 12348
    # For true loopback, you can use "::1" but that's not a multicast address.
    # ff02::1 is link-local all-nodes, will reach all on the local link.
    send_multicast(MULTICAST_IP_V6, MULTICAST_PORT_V6, "Hello Multicast (IPv6)!")

