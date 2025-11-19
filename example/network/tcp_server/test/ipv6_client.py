import socket

def main(server_ip, server_port):
    # Create an IPv6 TCP socket
    with socket.socket(socket.AF_INET6, socket.SOCK_STREAM) as sock:
        try:
            sock.connect((server_ip, server_port))
            print(f"Connected to [{server_ip}]:{server_port}")

            # Send a message to the server
            message = "Hello, TCP IPv6 Server!"
            sock.sendall(message.encode())
            print(f"Sent: {message}")

            # Receive a response from the server (optional)
            response = sock.recv(1024)
            print(f"Received: {response.decode()}")

        except Exception as e:
            print(f"Error: {e}")

if __name__ == "__main__":
    # Example usage with IPv6 loopback address and port 12346
    main("::1", 12346)

