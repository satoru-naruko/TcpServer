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
    // テスト用サーバーを作成
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
    // テスト後にサーバーを停止
    if (server_ && server_->IsRunning()) {
      server_->Stop();
    }
  }

  // テスト用のクライアント関数
  std::string SendMessage(const std::string& message) {
    try {
      boost::asio::io_context io_context;
      tcp::socket socket(io_context);
      
      // サーバーに接続
      socket.connect(tcp::endpoint(
          boost::asio::ip::address::from_string("127.0.0.1"), test_port_));
      
      // メッセージを送信
      boost::asio::write(socket, boost::asio::buffer(message));
      
      // レスポンスを受信
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

// サーバーの起動と停止をテスト
TEST_F(TcpServerTest, StartStopTest) {
  ASSERT_FALSE(server_->IsRunning());
  
  // サーバーを起動
  server_->Start(1);
  ASSERT_TRUE(server_->IsRunning());
  
  // サーバーを停止
  server_->Stop();
  ASSERT_FALSE(server_->IsRunning());
}

// 基本的なメッセージの送受信をテスト
TEST_F(TcpServerTest, BasicCommunication) {
  // サーバーを起動
  server_->Start(1);
  
  // 少し待ってサーバーが起動するのを待つ
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // pingメッセージを送信
  std::string response = SendMessage("ping");
  EXPECT_EQ("pong", response);
  
  // helloメッセージを送信
  response = SendMessage("hello");
  EXPECT_EQ("world", response);
  
  // 未知のメッセージを送信
  response = SendMessage("unknown");
  EXPECT_EQ("unknown command", response);
}

// 複数接続のテスト
TEST_F(TcpServerTest, MultipleConnections) {
  // サーバーを起動（2スレッド）
  server_->Start(2);
  
  // 少し待ってサーバーが起動するのを待つ
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  // 複数のクライアントを同時に実行
  constexpr int client_count = 5;
  std::vector<std::future<std::string>> futures;
  
  for (int i = 0; i < client_count; ++i) {
    futures.push_back(std::async(std::launch::async, [this]() {
      return SendMessage("ping");
    }));
  }
  
  // すべてのレスポンスを確認
  for (auto& f : futures) {
    EXPECT_EQ("pong", f.get());
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
} 