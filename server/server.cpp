#include "server.h"
#include "router.h"
#include "http_connection.h"
#define PORT 11451

void registerRouters(Router& router) {
    router.get("/echo/:content",[](Request& req, Response& res) {
        std::string content = req.params.at("content");
        std::cout << content << std::endl;
        res.headers["Content-Type"] = "text/plain";
        res.body = content;
        res.status = 200;
    });

    router.get("/", [](Request& req, Response& res) {
        res.status = 200;
        res.headers["Content-Type"] = "text/html";
        res.headers["Server"] = "Toy Tcp Server/0.1";
        res.body = "<html> \
             <head><title>Toy Cpp Server</title></head> \
             <body> \
            <center><h1>Toy Cpp Server</h1></center> \
             <hr><center>BoranStars</center> \
             </body> \
             </html>";
    });

    router.get("/user-agent",[](Request& req, Response& res) {
        res.status = 200;
        res.headers["Content-Type"] = "text/plain";
        res.body = req.headers["User-Agent"];
    });

    router.get("/ys",[](Request& req, Response& res) {
        res.status = 302;
        res.headers["Location"] = "https://www.yuanshen.com/";

    });

    router.get("/foo/bar",[](Request& req, Response& res) {
        res.status = 200;
        res.headers["Content-Type"] = "text/plain";
        res.body = "foo/bar!";
    });

    router.get("/file/:*filepath",[](Request& req, Response& res) {
        res.status = 200;
        res.headers["Content-Type"] = "text/plain";
        res.body = req.params.at("filepath");
    });

    router.get("/static/*",[](Request& req, Response& res) {
        res.status = 200;
        res.headers["Content-Type"] = "text/plain";
        res.body = req.params.at("wildcard");
    });
}

int main() {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);



    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "bind failed at\n";
        return 1;
    }

    std::cout << std::endl << "Listening on port " << PORT << std::endl;
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        return 1;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    Router router;
    registerRouters(router);


    std::cout << "Waiting for a client to connect...\n";

    while (true) {
        // main loop
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            std::cerr << "accept failed\n";
            continue;
        }
        std::cout << "Client connected\n";
        std::cout << std::endl << "Client : " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

        // HttpConnection connection(client_fd,router);
        // connection.process();
        std::thread([client_fd, &router]() {
            HttpConnection connection(client_fd, router);
            connection.process();
        }).detach();


        // Buffer recv_buffer;
        // std::vector<char> temp_buffer(BUFSIZE);
        // Request request;
        //
        // ssize_t recv_size = recv(client_fd, temp_buffer.data(), BUFSIZE, 0);
        // if (recv_size > 0) {
        //     temp_buffer.resize(recv_size);
        //     recv_buffer.append(temp_buffer);
        //     HttpParser parser;
        //     if (parser.parse(recv_buffer)) {
        //         request = parser.getRequest();
        //         parser.reset();
        //         std::cout << "Method: " << request.method << "\n"
        //     << "URI: " << request.uri << "\n"
        //     << "Body: " << request.body << std::endl;
        //         HandlerReturn result = router.getHandler(request.method, request.uri);
        //         if (result.success) {
        //             Response response;
        //             if (!result.params.empty()) {
        //                 request.params = result.params;
        //             }
        //             result.handler(request, response);
        //             std::string response_string = response.generateResponse();
        //             std::cout << response_string << std::endl;
        //             send(client_fd, response_string.c_str(), response_string.size(), 0);
        //
        //         } else {
        //             Response response;
        //             response.status = 404;
        //             response.headers["Content-Type"] = "text/html";
        //             response.headers["Server"] = "Toy Tcp Server/0.1";
        //             response.body = "<html> \
        //                                <h1>404 Not Found</h1> \
        //                                 <hr> \
        //                                 <center>Toy Cpp Server</center> ";
        //             std::string response_string = response.generateResponse();
        //             std::cout << response_string << std::endl;
        //             send(client_fd, response_string.c_str(), response_string.size(), 0);
        //         }
        //     }
        //
        // } else {
        //     close(client_fd);
        //     std::cerr << "Client disconnected\n";
        //     continue;
        // }
        // // Response response;
        // // response.status = 200;
        // // response.headers["Content-Type"] = "text/html";
        // // response.headers["Server"] = "Toy Tcp Server/0.1";
        // // response.body = "<html> \
        // //      <head><title>Toy Cpp Server</title></head> \
        // //      <body> \
        // //     <center><h1>Toy Cpp Server</h1></center> \
        // //      <hr><center>BoranStars</center> \
        // //      </body> \
        // //      </html>";
        //
        //
        //
        // close(client_fd);
        // std::cout << "Client disconnected\n";

    }



    close(server_fd);
    return 0;
}

void mainHandler(const Request &req, Response &res) {
    // uri 为"/"

}