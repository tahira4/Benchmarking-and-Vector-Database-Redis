#include <iostream>
#include <chrono>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define SERVER_PORT 5001
#define BUFFER_SIZE 4096
#define MESSAGE_SIZE (1024 * 1024)  // 1MB per message
#define NUM_MESSAGES 100             // Expecting 100 messages

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error: Winsock initialization failed!" << std::endl;
        return 1;
    }

    // Create server socket
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Enable address reuse to avoid "Address already in use" errors
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // Bind and listen
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Error: Bind failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {  // Allow multiple pending connections
        std::cerr << "Error: Listen failed" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on 0.0.0.0:" << SERVER_PORT << std::endl;

    while (true) {  // Keep accepting multiple clients
        SOCKET client_socket;
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        
        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Error: Accept failed, retrying..." << std::endl;
            continue;  // Keep the server running
        }

        std::cout << "Connected by ('" << inet_ntoa(client_addr.sin_addr) << "', " << ntohs(client_addr.sin_port) << ")" << std::endl;

        // Start timing
        auto start_time = std::chrono::high_resolution_clock::now();
        size_t total_received = 0;
        char buffer[BUFFER_SIZE];

        for (int i = 0; i < NUM_MESSAGES; ++i) {
            size_t message_received = 0;
            while (message_received < MESSAGE_SIZE) {
                ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
                if (bytes_received <= 0) break;
                message_received += bytes_received;
                total_received += bytes_received;
            }
            std::cout << "Received message " << i + 1 << "/" << NUM_MESSAGES << std::endl;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end_time - start_time).count();
        double throughput = total_received / elapsed / 1024;  // Convert to KB/s

        std::cout << "Received " << total_received << " bytes in " << elapsed << " seconds." << std::endl;
        std::cout << "Throughput: " << throughput << " KB/s" << std::endl;

        // Send acknowledgment
        send(client_socket, "ACK", 3, 0);
        std::cout << "Sent acknowledgment: ACK" << std::endl;

        // Close client socket
        closesocket(client_socket);
    }

    // Close server socket when done
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
