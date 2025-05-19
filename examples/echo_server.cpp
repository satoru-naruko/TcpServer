#include <iostream>
#include <string>
#include <csignal>
#include <atomic>

#include "tcp_server/tcp_server.h"
#include <spdlog/spdlog.h>

std::atomic<bool> running(true);

// シグナルハンドラ
void signal_handler(int signal) {
  spdlog::info("Signal {} received, shutting down...", signal);
  running = false;
}

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
    
    // シグナルハンドラを設定（Ctrl+Cなどで適切に終了するため）
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
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
    
    return 0;
  } catch (const std::exception& e) {
    spdlog::error("Error: {}", e.what());
    return 1;
  }
} 