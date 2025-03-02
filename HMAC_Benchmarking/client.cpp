#include <iostream>
#include <chrono>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5001
#define MESSAGE_SIZE (1024 * 1024)  // 1MB per message
#define NUM_MESSAGES 100            // Send 100 messages

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error: Winsock initialization failed!" << std::endl;
        return 1;
    }

    // Create socket
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket" << std::endl;
        return 1;
    }

    // Server address setup
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Connection to server failed" << std::endl;
        return 1;
    }
    std::cout << "Connected to server at " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    size_t total_bytes_sent = 0;
    char data[MESSAGE_SIZE];
    memset(data, 'a', MESSAGE_SIZE);  // Fill with dummy data

    for (int i = 0; i < NUM_MESSAGES; ++i) {
        send(client_socket, data, MESSAGE_SIZE, 0);
        total_bytes_sent += MESSAGE_SIZE;
        std::cout << "Sent message " << i + 1 << "/" << NUM_MESSAGES << std::endl;
    }

    // Indicate that data transmission is complete
    shutdown(client_socket, SD_SEND);

    // Receive acknowledgment
    char buffer[1024] = {0};
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    std::cout << "Received acknowledgment: " << buffer << std::endl;

    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end_time - start_time).count();
    double throughput = total_bytes_sent / elapsed / 1024;  // Convert to KB/s

    std::cout << "Sent " << total_bytes_sent << " bytes in " << elapsed << " seconds." << std::endl;
    std::cout << "Throughput: " << throughput << " KB/s" << std::endl;

    // Close socket
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
