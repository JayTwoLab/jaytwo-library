import socket

def main(server_ip, server_port):
    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        try:
            sock.connect((server_ip, server_port))
            print(f"Connected to {server_ip}:{server_port}")

            # Send a message to the server
            message = "Hello, TCP Server!"
            sock.sendall(message.encode())
            print(f"Sent: {message}")

            # Receive a response from the server (optional)
            response = sock.recv(1024)
            print(f"Received: {response.decode()}")

        except Exception as e:
            print(f"Error: {e}")

if __name__ == "__main__":
    # Example usage with custom IP and port
    main("127.0.0.1", 122345)

