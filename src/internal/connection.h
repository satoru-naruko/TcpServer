/**
 * @file connection.h
 * @brief Class for managing TCP connections
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
 * @brief Class for managing TCP connections
 *
 * Manages a single client connection and handles data transmission
 */
class Connection : public std::enable_shared_from_this<Connection> {
 public:
  using tcp = boost::asio::ip::tcp;
  using MessageHandler = std::function<std::string(const std::string&)>;

  /**
   * @brief Create a connection object
   * @param io_context Boost.Asio io_context
   * @param message_handler Handler for processing received messages
   * @return Shared pointer to the connection object
   */
  static std::shared_ptr<Connection> Create(
      boost::asio::io_context& io_context,
      MessageHandler message_handler);

  /**
   * @brief Get reference to TCP socket
   * @return Reference to TCP socket
   */
  tcp::socket& GetSocket();

  /**
   * @brief Initialize connection and start reading
   */
  void Start();

  /**
   * @brief Close the connection
   */
  void Stop();

 private:
  /**
   * @brief Constructor
   * @param io_context Boost.Asio io_context
   * @param message_handler Handler for processing received messages
   */
  Connection(boost::asio::io_context& io_context, MessageHandler message_handler);

  /**
   * @brief Start asynchronous read
   */
  void StartRead();

  /**
   * @brief Handler for read completion
   * @param error Error information
   * @param bytes_transferred Number of bytes transferred
   */
  void HandleRead(const boost::system::error_code& error, std::size_t bytes_transferred);

  /**
   * @brief Start asynchronous write
   * @param data Data to send
   */
  void StartWrite(const std::string& data);

  /**
   * @brief Handler for write completion
   * @param error Error information
   * @param bytes_transferred Number of bytes transferred
   */
  void HandleWrite(const boost::system::error_code& error, std::size_t bytes_transferred);

  tcp::socket socket_;                  ///< TCP socket
  MessageHandler message_handler_;      ///< Message handler
  std::vector<char> read_buffer_;       ///< Read buffer
  std::string write_buffer_;            ///< Write buffer
  static constexpr size_t kBufferSize = 1024;  ///< Buffer size
};

}  // namespace internal
}  // namespace tcp_server

#endif  // TCP_SERVER_INTERNAL_CONNECTION_H_ 