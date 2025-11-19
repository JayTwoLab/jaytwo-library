import socket
import sys
from datetime import datetime

def run_tcp_server(ip, port):
    # Create a TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Allow quick restart
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind to IP and port
    server_socket.bind((ip, port))

    # Start listening
    server_socket.listen(5)

    # Timeout for Ctrl+C to work properly
    server_socket.settimeout(1.0)

    print(f"TCP server started at {ip}:{port}")

    try:
        while True:
            try:
                client_socket, client_addr = server_socket.accept()
            except socket.timeout:
                continue

            print(f"Client connected: {client_addr}")

            try:
                while True:
                    data = client_socket.recv(1024)
                    if not data:
                        print(f"Client disconnected: {client_addr}")
                        break

                    print(f"Received from {client_addr}: {data.decode('utf-8', errors='replace')}")

                    # Build timestamped response
                    current_time = datetime.now().strftime("%H:%M:%S")
                    response = f"Message received at {current_time}."
                    client_socket.sendall(response.encode('utf-8'))

            except Exception as e:
                print(f"Error while communicating with {client_addr}: {e}")

            finally:
                client_socket.close()

    except KeyboardInterrupt:
        print("Server shutdown requested by user (Ctrl+C).")

    finally:
        server_socket.close()
        print("TCP server stopped.")


def main():
    if len(sys.argv) != 3:
        print("Usage: python tcp_server.py <IP> <PORT>")
        sys.exit(1)

    ip = sys.argv[1]
    port = int(sys.argv[2])

    run_tcp_server(ip, port)


if __name__ == "__main__":
    main()
