import socket
import datetime

def send_udp_unicast(target_ip: str, target_port: int, message: str):
    # Determine address family (IPv4 or IPv6)
    try:
        socket.inet_pton(socket.AF_INET, target_ip)
        family = socket.AF_INET
    except OSError:
        family = socket.AF_INET6

    sock = socket.socket(family, socket.SOCK_DGRAM)

    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    send_data = f"[{now}] {message}".encode("utf-8")

    sock.sendto(send_data, (target_ip, target_port))
    print(f"Sent to {target_ip}:{target_port} -> {send_data}")

    sock.close()


if __name__ == "__main__":
    # IPv4 example
    TARGET_IP = "127.0.0.1"
    TARGET_PORT = 12345
    send_udp_unicast(TARGET_IP, TARGET_PORT, "Hello UDP Unicast! (IPv4)")

    # IPv6 example (loopback, port 12346)
    TARGET_IP_V6 = "::1"
    TARGET_PORT_V6 = 12346
    send_udp_unicast(TARGET_IP_V6, TARGET_PORT_V6, "Hello UDP Unicast! (IPv6)")

