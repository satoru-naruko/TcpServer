#include "tcp_server/tcp_server.h"

#include <spdlog/spdlog.h>
#include <thread>
#include <stdexcept>
#include <mutex>

#include "src/internal/connection.h"

namespace tcp_server {

TcpServer::TcpServer(unsigned short port, MessageHandler message_handler,
                   unsigned int max_connections)
    : port_(port),
      max_connections_(max_connections),
      message_handler_(std::move(message_handler)),
      io_context_(std::make_unique<boost::asio::io_context>()),
      running_(false),
      work_guard_(boost::asio::make_work_guard(*io_context_)) {
  try {
    using tcp = boost::asio::ip::tcp;
    
    // Initialize acceptor
    acceptor_ = std::make_unique<tcp::acceptor>(
        *io_context_, tcp::endpoint(tcp::v4(), port_));
        
    spdlog::info("TCP server initialized on port {}", port_);
  } catch (const std::exception& e) {
    spdlog::error("Failed to initialize TCP server: {}", e.what());
    throw std::runtime_error(std::string("Failed to initialize TCP server: ") + e.what());
  }
}

TcpServer::~TcpServer() {
  Stop();
}

void TcpServer::Start(unsigned int thread_count) {
  if (running_) {
    spdlog::warn("TCP server already running");
    return;
  }

  try {
    // Determine thread count
    if (thread_count == 0) {
      thread_count = std::thread::hardware_concurrency();
      if (thread_count == 0) {
        thread_count = 1;  // Use single thread if hardware info is not available
      }
    }
    
    // Start accepting new connections
    StartAccept();
    
    // Launch worker threads
    threads_.clear();
    for (unsigned int i = 0; i < thread_count; ++i) {
      threads_.emplace_back([this] {
        try {
          io_context_->run();
        } catch (const std::exception& e) {
          spdlog::error("Error in worker thread: {}", e.what());
        }
      });
    }
    
    running_ = true;
    spdlog::info("TCP server started with {} worker threads", thread_count);
  } catch (const std::exception& e) {
    spdlog::error("Failed to start TCP server: {}", e.what());
    // Cleanup if partially started
    Stop();
    throw std::runtime_error(std::string("Failed to start TCP server: ") + e.what());
  }
}

void TcpServer::Stop() {
  if (!running_) {
    return;
  }

  spdlog::info("Stopping TCP server...");
  
  // Destroy work guard to allow io_context to stop
  work_guard_.reset();
  
  // Stop io_context
  io_context_->stop();
  
  // Wait for all worker threads to finish
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  threads_.clear();
  
  // Close all connections
  {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    for (auto& conn : connections_) {
      conn->Stop();
    }
    connections_.clear();
  }
  
  running_ = false;
  spdlog::info("TCP server stopped");
}

bool TcpServer::IsRunning() const {
  return running_;
}

void TcpServer::StartAccept() {
  if (!acceptor_ || !io_context_) {
    spdlog::error("Acceptor or io_context is not initialized");
    return;
  }

  auto connection = internal::Connection::Create(*io_context_, message_handler_);
  
  acceptor_->async_accept(
      connection->GetSocket(),
      [this, connection](const boost::system::error_code& error) {
        HandleAccept(connection, error);
      });
}

void TcpServer::HandleAccept(std::shared_ptr<internal::Connection> connection,
                           const boost::system::error_code& error) {
  if (!error) {
    spdlog::info("New connection from {}:{}",
                connection->GetSocket().remote_endpoint().address().to_string(),
                connection->GetSocket().remote_endpoint().port());
    
    // Add connection and start processing
    if (AddConnection(connection)) {
      connection->Start();
    } else {
      // Reject connection if maximum connections reached
      spdlog::warn("Maximum connections reached, rejecting new connection");
      connection->Stop();
    }
  } else {
    spdlog::error("Accept error: {}", error.message());
  }
  
  // Accept next connection
  StartAccept();
}

bool TcpServer::AddConnection(std::shared_ptr<internal::Connection> connection) {
  std::lock_guard<std::mutex> lock(connections_mutex_);
  
  // Check maximum connections
  if (connections_.size() >= max_connections_) {
    return false;
  }
  
  // Store connection
  connections_.insert(connection);
  return true;
}

void TcpServer::RemoveConnection(std::shared_ptr<internal::Connection> connection) {
  std::lock_guard<std::mutex> lock(connections_mutex_);
  connections_.erase(connection);
}

}  // namespace tcp_server 