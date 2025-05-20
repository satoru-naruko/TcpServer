#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <functional>
#include <unordered_map>
#include <algorithm>

#include <boost/asio.hpp>
#include <tcp_server/tcp_server.h>
#include <spdlog/spdlog.h>

// Function to remove whitespace from both ends of a string
inline std::string trim(const std::string& str) {
    const auto begin = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    const auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();
    
    if (begin >= end) {
        return "";
    }
    return std::string(begin, end);
}

// Class to manage response patterns
class ResponseProvider {
public:
    // Initialize with default responses
    ResponseProvider() {
        // Set default response patterns
        responsePatterns["hello"] = "Hello!";
        responsePatterns["help"] = "Available commands: hello, help, info, exit, time, date, weather";
        responsePatterns["info"] = "Application Server using TcpServer library";
        responsePatterns["exit"] = "Connection will be closed. Goodbye!";
    }

    // Add a new response pattern
    void addPattern(const std::string& request, const std::string& response) {
        responsePatterns[trim(request)] = response;
    }

    // Get response for a request
    std::string getResponse(const std::string& request) {
        // Remove whitespace from request
        std::string trimmed_request = trim(request);
        spdlog::debug("Trimmed request: '{}'", trimmed_request);
        
        auto it = responsePatterns.find(trimmed_request);
        if (it != responsePatterns.end()) {
            // Pattern found
            return it->second;
        } else {
            // Pattern not found
            return "Command not recognized. Type 'help' to see available commands.";
        }
    }

private:
    std::unordered_map<std::string, std::string> responsePatterns;
};

std::atomic<bool> running(true);

int main(int argc, char* argv[]) {
    try {
        // Set log level
        spdlog::set_level(spdlog::level::info);
        
        // Default port
        unsigned short port = 8765;
        
        // Get port from command line if specified
        if (argc > 1) {
            port = static_cast<unsigned short>(std::stoi(argv[1]));
        }
        
        // Create Boost.Asio io_context
        boost::asio::io_context io_context;
        
        // Set up signal handler (cross-platform)
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](const boost::system::error_code& error, int signal_number) {
            spdlog::info("Signal {} received, shutting down...", signal_number);
            running = false;
        });
        
        // Run io_context in a separate thread
        std::thread io_thread([&io_context]() {
            io_context.run();
        });
        
        // Create response provider
        ResponseProvider responseProvider;
        
        // Add custom responses
        responseProvider.addPattern("time", "Current time information");
        responseProvider.addPattern("date", "Today's date information");
        responseProvider.addPattern("weather", "Weather is sunny");
        
        // Message handler function
        auto message_handler = [&responseProvider](const std::string& message) -> std::string {
            spdlog::info("Request received: {}", message);
            std::string response = responseProvider.getResponse(message);
            spdlog::info("Sending response: {}", response);
            return response;
        };
        
        // Create and start TCP server
        spdlog::info("Starting application server on port {}", port);
        tcp_server::TcpServer server(port, message_handler);
        server.Start();
        
        spdlog::info("Application server running. Press Ctrl+C to stop.");
        spdlog::info("Available commands: hello, help, info, exit, time, date, weather");
        
        // Main loop: run while server is active and no shutdown signal received
        while (running && server.IsRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Stop server
        spdlog::info("Stopping application server...");
        server.Stop();
        spdlog::info("Application server stopped.");
        
        // Stop io_context and wait for io_thread to finish
        io_context.stop();
        if (io_thread.joinable()) {
            io_thread.join();
        }
        
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
} 