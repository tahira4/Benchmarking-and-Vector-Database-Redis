#!/usr/bin/env python3
import socket
import time
import hmac
import hashlib

# Server details
HOST = '127.0.0.1'  # Server IP address
PORT = 5001         # Server port

# Settings
MESSAGE_SIZE = 1024 * 1024  # 1 MB
NUM_MESSAGES = 100  # Number of messages
SECRET_KEY = b"supersecretkey"  # HMAC Key


def compute_hmac(data):
    """ Compute HMAC-SHA256 for given data """
    return hmac.new(SECRET_KEY, data, hashlib.sha256).digest()


def run_client():
    """ Client function to send data and HMAC to server """
    total_bytes = MESSAGE_SIZE * NUM_MESSAGES
    data = b'a' * MESSAGE_SIZE  # 1MB of dummy data

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((HOST, PORT))
        print(f"Connected to server at {HOST}:{PORT}")

        # Start benchmark timer
        start_time = time.time()

        for i in range(NUM_MESSAGES):
            # Send 1MB data
            client_socket.sendall(data)

            # Compute and send HMAC
            hmac_value = compute_hmac(data)
            client_socket.sendall(hmac_value)

            print(f"Sent message {i + 1}/{NUM_MESSAGES} with HMAC")

        # Notify server that transmission is complete
        client_socket.shutdown(socket.SHUT_WR)

        # Receive acknowledgment
        ack = client_socket.recv(1024)
        print("Received acknowledgment:", ack.decode())

        # End benchmark timer
        elapsed = time.time() - start_time
        throughput = total_bytes / elapsed if elapsed > 0 else 0

        print(f"Sent {total_bytes} bytes in {elapsed:.2f} seconds.")
        print(f"Throughput: {throughput / 1024:.2f} KB/s")


if __name__ == "__main__":
    run_client()
