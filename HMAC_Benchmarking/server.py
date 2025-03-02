#!/usr/bin/env python3
import socket
import time
import hmac
import hashlib

# Server details
HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 5001       # Port to listen on

# Settings
BUFFER_SIZE = 4096  # Receive buffer size
MESSAGE_SIZE = 1024 * 1024  # 1MB per message
NUM_MESSAGES = 100  # Expecting 100 messages
HMAC_SIZE = 32  # SHA256 HMAC size
SECRET_KEY = b"supersecretkey"  # HMAC Key


def compute_hmac(data):
    """ Compute HMAC-SHA256 for given data """
    return hmac.new(SECRET_KEY, data, hashlib.sha256).digest()


def start_server():
    """ Server function to receive data and verify HMAC """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen()
        print(f"Server listening on {HOST}:{PORT}")

        conn, addr = server_socket.accept()
        with conn:
            print('Connected by', addr)
            total_received = 0
            start_time = time.time()

            for i in range(NUM_MESSAGES):
                # Receive the full 1MB message
                received_data = b''
                while len(received_data) < MESSAGE_SIZE:
                    chunk = conn.recv(min(BUFFER_SIZE, MESSAGE_SIZE - len(received_data)))
                    if not chunk:
                        break
                    received_data += chunk

                # Receive the 32-byte HMAC
                received_hmac = conn.recv(HMAC_SIZE)

                # Compute HMAC for the received data
                computed_hmac = compute_hmac(received_data)

                # Compare HMACs
                if hmac.compare_digest(received_hmac, computed_hmac):
                    print(f"Message {i + 1} HMAC verified ✅")
                else:
                    print(f"Message {i + 1} HMAC MISMATCH ❌")

                total_received += len(received_data)

            end_time = time.time()
            elapsed = end_time - start_time
            throughput = total_received / elapsed if elapsed > 0 else 0

            print(f"Received {total_received} bytes in {elapsed:.2f} seconds.")
            print(f"Throughput: {throughput / 1024:.2f} KB/s")

            # Send acknowledgment
            conn.sendall(b"ACK")


if __name__ == "__main__":
    start_server()
