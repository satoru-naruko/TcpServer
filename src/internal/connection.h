/**
 * @file connection.h
 * @brief TCP接続を管理するクラス
 */

#ifndef TCP_SERVER_INTERNAL_CONNECTION_H_
#define TCP_SERVER_INTERNAL_CONNECTION_H_

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace tcp_server {
namespace internal {

/**
 * @brief TCP接続を管理するクラス
 *
 * 単一のクライアント接続を管理し、データの送受信を行う
 */
class Connection : public std::enable_shared_from_this<Connection> {
 public:
  using tcp = boost::asio::ip::tcp;
  using MessageHandler = std::function<std::string(const std::string&)>;

  /**
   * @brief 接続オブジェクトを作成する
   * @param io_context Boost.Asioのio_context
   * @param message_handler 受信メッセージを処理するハンドラ
   * @return 接続オブジェクトの共有ポインタ
   */
  static std::shared_ptr<Connection> Create(
      boost::asio::io_context& io_context,
      MessageHandler message_handler);

  /**
   * @brief TCPソケットの参照を取得
   * @return TCPソケットの参照
   */
  tcp::socket& GetSocket();

  /**
   * @brief 接続の初期化と読み込み開始
   */
  void Start();

  /**
   * @brief 接続を閉じる
   */
  void Stop();

 private:
  /**
   * @brief コンストラクタ
   * @param io_context Boost.Asioのio_context
   * @param message_handler 受信メッセージを処理するハンドラ
   */
  Connection(boost::asio::io_context& io_context, MessageHandler message_handler);

  /**
   * @brief 非同期読み込みを開始
   */
  void StartRead();

  /**
   * @brief 読み込み完了時のハンドラ
   * @param error エラー情報
   * @param bytes_transferred 転送されたバイト数
   */
  void HandleRead(const boost::system::error_code& error, std::size_t bytes_transferred);

  /**
   * @brief 非同期書き込みを開始
   * @param data 送信するデータ
   */
  void StartWrite(const std::string& data);

  /**
   * @brief 書き込み完了時のハンドラ
   * @param error エラー情報
   * @param bytes_transferred 転送されたバイト数
   */
  void HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred);

  tcp::socket socket_;                  ///< TCPソケット
  MessageHandler message_handler_;      ///< メッセージハンドラ
  std::vector<char> read_buffer_;       ///< 読み込みバッファ
  std::string write_buffer_;            ///< 書き込みバッファ
  static constexpr size_t kBufferSize = 1024;  ///< バッファサイズ
};

}  // namespace internal
}  // namespace tcp_server

#endif  // TCP_SERVER_INTERNAL_CONNECTION_H_ 