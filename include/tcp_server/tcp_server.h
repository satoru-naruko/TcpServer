/**
 * @file tcp_server.h
 * @brief TCPサーバークラスの定義
 */

#ifndef TCP_SERVER_TCP_SERVER_H_
#define TCP_SERVER_TCP_SERVER_H_

#include <boost/asio.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

// 前方宣言
namespace tcp_server {
namespace internal {
class Connection;
}  // namespace internal

/**
 * @brief TCPサーバークラス
 *
 * 複数のクライアント接続を受け付け、メッセージを処理するTCPサーバー
 */
class TcpServer {
 public:
  using MessageHandler = std::function<std::string(const std::string&)>;

  /**
   * @brief TCPサーバーのコンストラクタ
   * @param port 待ち受けるポート番号
   * @param message_handler クライアントからのメッセージを処理するハンドラ
   * @param max_connections 最大接続数（デフォルト：8）
   * @throws std::runtime_error サーバーの初期化に失敗した場合
   */
  TcpServer(unsigned short port, MessageHandler message_handler,
           unsigned int max_connections = 8);

  /**
   * @brief デストラクタ
   */
  ~TcpServer();

  /**
   * @brief サーバーを起動する
   * @param thread_count 使用するスレッド数（0の場合はハードウェア並列数を使用）
   * @throws std::runtime_error サーバーの起動に失敗した場合
   */
  void Start(unsigned int thread_count = 0);

  /**
   * @brief サーバーを停止する
   */
  void Stop();

  /**
   * @brief サーバーが実行中かどうかを返す
   * @return サーバーが実行中ならtrue、そうでなければfalse
   */
  bool IsRunning() const;

 private:
  /**
   * @brief 新しい接続の受け入れを開始
   */
  void StartAccept();

  /**
   * @brief 接続受け入れ完了時のハンドラ
   * @param connection 確立した接続
   * @param error エラー情報
   */
  void HandleAccept(std::shared_ptr<internal::Connection> connection,
                   const boost::system::error_code& error);

  /**
   * @brief 接続を管理対象に追加
   * @param connection 追加する接続
   * @return 追加に成功した場合はtrue、失敗した場合はfalse
   */
  bool AddConnection(std::shared_ptr<internal::Connection> connection);

  /**
   * @brief 接続を管理対象から削除
   * @param connection 削除する接続
   */
  void RemoveConnection(std::shared_ptr<internal::Connection> connection);

  unsigned short port_;                 ///< 待ち受けポート
  unsigned int max_connections_;        ///< 最大接続数
  MessageHandler message_handler_;      ///< メッセージハンドラ
  std::unique_ptr<boost::asio::io_context> io_context_;  ///< I/Oコンテキスト
  std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;  ///< 接続受付オブジェクト
  std::vector<std::thread> threads_;   ///< ワーカースレッド
  std::unordered_set<std::shared_ptr<internal::Connection>> connections_;  ///< アクティブな接続
  std::mutex connections_mutex_;       ///< 接続リスト用ミューテックス
  volatile bool running_;              ///< サーバー実行中フラグ
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;  ///< I/Oコンテキスト終了防止ガード
};

}  // namespace tcp_server

#endif  // TCP_SERVER_TCP_SERVER_H_ 