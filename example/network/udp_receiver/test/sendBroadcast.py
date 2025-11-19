
import socket
import datetime

def send_broadcast(broadcast_ip: str, port: int, message: str):
    # Create UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Enable broadcast mode
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    # Add timestamp to message (optional)
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    send_data = f"[{now}] {message}".encode("utf-8")

    # Send broadcast packet
    sock.sendto(send_data, (broadcast_ip, port))
    print(f"Broadcast sent to {broadcast_ip}:{port} -> {send_data}")

    # Close socket
    sock.close()


if __name__ == "__main__":
    # Typical broadcast address: 255.255.255.255
    BROADCAST_IP = "255.255.255.255"
    PORT = 12349

    send_broadcast(BROADCAST_IP, PORT, "Hello Broadcast!")

