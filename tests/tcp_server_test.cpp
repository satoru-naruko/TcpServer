#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <thread>
#include <string>
#include <future>
#include <chrono>

#include "tcp_server/tcp_server.h"

using namespace tcp_server;
using boost::asio::ip::tcp;

class TcpServerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create test server
    server_ = std::make_unique<TcpServer>(test_port_, [](const std::string& message) {
      if (message == "ping") {
        return "pong";
      } else if (message == "hello") {
        return "world";
      } else {
        return "unknown command";
      }
    });
  }

  void TearDown() override {
    // Stop server after test
    if (server_ && server_->IsRunning()) {
      server_->Stop();
    }
  }

  // Client function for testing
  std::string SendMessage(const std::string& message) {
    try {
      boost::asio::io_context io_context;
      tcp::socket socket(io_context);
      
      // Connect to server
      socket.connect(tcp::endpoint(
          boost::asio::ip::make_address("127.0.0.1"), test_port_));
      
      // Send message
      boost::asio::write(socket, boost::asio::buffer(message));
      
      // Receive response
      std::vector<char> reply(1024);
      size_t reply_length = socket.read_some(boost::asio::buffer(reply));
      
      return std::string(reply.data(), reply_length);
    } catch (std::exception& e) {
      return std::string("ERROR: ") + e.what();
    }
  }

  const unsigned short test_port_ = 12345;
  std::unique_ptr<TcpServer> server_;
};

// Test server start and stop
TEST_F(TcpServerTest, StartStopTest) {
  ASSERT_FALSE(server_->IsRunning());
  
  // Start server
  server_->Start(1);
  ASSERT_TRUE(server_->IsRunning());
  
  // Stop server
  server_->Stop();
  ASSERT_FALSE(server_->IsRunning());
}

// Test basic message communication
TEST_F(TcpServerTest, BasicCommunication) {
  // Start server
  server_->Start(1);
  
  // Wait a bit for the server to start
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Send ping message
  std::string response = SendMessage("ping");
  EXPECT_EQ("pong", response);
  
  // Send hello message
  response = SendMessage("hello");
  EXPECT_EQ("world", response);
  
  // Send unknown message
  response = SendMessage("unknown");
  EXPECT_EQ("unknown command", response);
}

// Test multiple connections
TEST_F(TcpServerTest, MultipleConnections) {
  // Start server (2 threads)
  server_->Start(2);
  
  // Wait a bit for the server to start
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // Run multiple clients simultaneously
  constexpr int client_count = 5;
  std::vector<std::future<std::string>> futures;
  
  for (int i = 0; i < client_count; ++i) {
    futures.push_back(std::async(std::launch::async, [this]() {
      return SendMessage("ping");
    }));
  }
  
  // Check all responses
  for (auto& f : futures) {
    EXPECT_EQ("pong", f.get());
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
} 