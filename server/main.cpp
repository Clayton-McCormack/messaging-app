#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

struct Message {
    std::string username;
    std::string content;
};

std::vector<Message> messages;

std::string build_json_messages() {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < messages.size(); ++i) {
        if (i) ss << ",";
        ss << "{\"username\":\"" << messages[i].username << "\",";
        ss << "\"content\":\"" << messages[i].content << "\"}";
    }
    ss << "]";
    return ss.str();
}

std::string http_response(const std::string& body, const std::string& status="200 OK", const std::string& content_type="application/json") {
    std::stringstream ss;
    ss << "HTTP/1.1 " << status << "\r\n";
    ss << "Content-Type: " << content_type << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "\r\n";
    ss << body;
    return ss.str();
}

void handle_connection(int client_fd) {
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    ssize_t read_bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (read_bytes <= 0) return;
    buffer[read_bytes] = '\0';
    std::string request(buffer);

    std::string method, path;
    std::stringstream ss(request);
    ss >> method >> path;

    if (method == "GET" && path == "/messages") {
        std::string body = build_json_messages();
        std::string resp = http_response(body);
        send(client_fd, resp.c_str(), resp.size(), 0);
    } else if (method == "POST" && path == "/messages") {
        size_t pos = request.find("\r\n\r\n");
        if (pos != std::string::npos) {
            std::string body = request.substr(pos + 4);
            // naive json parsing
            auto uname_pos = body.find("\"username\"");
            auto content_pos = body.find("\"content\"");
            if (uname_pos != std::string::npos && content_pos != std::string::npos) {
                auto start = body.find('"', uname_pos + 11);
                auto end = body.find('"', start + 1);
                std::string username = body.substr(start + 1, end - start - 1);

                start = body.find('"', content_pos + 10);
                end = body.find('"', start + 1);
                std::string content = body.substr(start + 1, end - start - 1);

                messages.push_back({username, content});
                std::string resp = http_response("{}", "200 OK");
                send(client_fd, resp.c_str(), resp.size(), 0);
            } else {
                std::string resp = http_response("{}", "400 Bad Request");
                send(client_fd, resp.c_str(), resp.size(), 0);
            }
        }
    } else {
        std::string resp = http_response("Not Found", "404 Not Found", "text/plain");
        send(client_fd, resp.c_str(), resp.size(), 0);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Server started on port 8080" << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }
        handle_connection(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
