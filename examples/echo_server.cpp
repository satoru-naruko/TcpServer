#include <iostream>
#include <string>
#include <atomic>
#include <thread>

#include <boost/asio.hpp>
#include "tcp_server/tcp_server.h"
#include <spdlog/spdlog.h>

std::atomic<bool> running(true);

int main(int argc, char* argv[]) {
  try {
    // Set log level
    spdlog::set_level(spdlog::level::info);
    
    // Default port
    unsigned short port = 9876;
    
    // Get port from command line if specified
    if (argc > 1) {
      port = static_cast<unsigned short>(std::stoi(argv[1]));
    }
    
    // Create Boost.Asio io_context
    boost::asio::io_context io_context;
    
    // Set up signal handler (cross-platform)
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code& error, int signal_number) {
      (void)error;
      spdlog::info("Signal {} received, shutting down...", signal_number);
      running = false;
    });
    
    // Run io_context in a separate thread
    std::thread io_thread([&io_context]() {
      io_context.run();
    });
    
    // Message handler function (echo back received message)
    auto message_handler = [](const std::string& message) -> std::string {
      spdlog::debug("Echoing message: {}", message);
      return message;
    };
    
    // Create and start TCP server
    spdlog::info("Starting echo server on port {}", port);
    tcp_server::TcpServer server(port, message_handler);
    server.Start();
    
    // Main loop: run while server is active and no shutdown signal received
    spdlog::info("Echo server running. Press Ctrl+C to stop.");
    while (running && server.IsRunning()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Stop server
    spdlog::info("Stopping echo server...");
    server.Stop();
    spdlog::info("Echo server stopped.");
    
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