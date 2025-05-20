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
    // ログレベルを設定
    spdlog::set_level(spdlog::level::info);
    
    // デフォルトポート
    unsigned short port = 9876;
    
    // 引数からポートを取得（指定されている場合）
    if (argc > 1) {
      port = static_cast<unsigned short>(std::stoi(argv[1]));
    }
    
    // Boost.Asioのio_contextを作成
    boost::asio::io_context io_context;
    
    // シグナルハンドラを設定（クロスプラットフォーム対応）
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code& error, int signal_number) {
      spdlog::info("Signal {} received, shutting down...", signal_number);
      running = false;
    });
    
    // 別スレッドでio_contextを実行
    std::thread io_thread([&io_context]() {
      io_context.run();
    });
    
    // メッセージハンドラ関数（受信したメッセージをそのまま返す）
    auto message_handler = [](const std::string& message) -> std::string {
      spdlog::debug("Echoing message: {}", message);
      return message;
    };
    
    // TCPサーバーを作成して起動
    spdlog::info("Starting echo server on port {}", port);
    tcp_server::TcpServer server(port, message_handler);
    server.Start();
    
    // サーバーが実行中かつシグナルを受信していない間はループ
    spdlog::info("Echo server running. Press Ctrl+C to stop.");
    while (running && server.IsRunning()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // サーバーを停止
    spdlog::info("Stopping echo server...");
    server.Stop();
    spdlog::info("Echo server stopped.");
    
    // io_contextを停止し、io_threadが終了するのを待つ
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