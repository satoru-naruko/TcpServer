#include "src/internal/connection.h"

#include <spdlog/spdlog.h>

namespace tcp_server {
namespace internal {

using tcp = boost::asio::ip::tcp;

std::shared_ptr<Connection> Connection::Create(
    boost::asio::io_context& io_context,
    MessageHandler message_handler) {
  return std::shared_ptr<Connection>(
      new Connection(io_context, std::move(message_handler)));
}

Connection::Connection(boost::asio::io_context& io_context,
                       MessageHandler message_handler)
    : socket_(io_context),
      message_handler_(std::move(message_handler)),
      read_buffer_(kBufferSize) {}

tcp::socket& Connection::GetSocket() {
  return socket_;
}

void Connection::Start() {
  spdlog::debug("Starting connection from {}:{}",
                socket_.remote_endpoint().address().to_string(),
                socket_.remote_endpoint().port());
  StartRead();
}

void Connection::Stop() {
  boost::system::error_code ec;
  socket_.close(ec);
  if (ec) {
    spdlog::error("Error closing socket: {}", ec.message());
  }
}

void Connection::StartRead() {
  auto self = shared_from_this();
  socket_.async_read_some(
      boost::asio::buffer(read_buffer_),
      [self](const boost::system::error_code& error, std::size_t bytes_transferred) {
        self->HandleRead(error, bytes_transferred);
      });
}

void Connection::HandleRead(const boost::system::error_code& error,
                            std::size_t bytes_transferred) {
  if (!error) {
    // Convert data to string
    std::string received_data(read_buffer_.data(), bytes_transferred);
    spdlog::debug("Received: {}", received_data);

    try {
      // Call message handler
      std::string response = message_handler_(received_data);
      // Send response
      StartWrite(response);
    } catch (const std::exception& ex) {
      spdlog::error("Error processing message: {}", ex.what());
      Stop();
      return;
    }

    // Start next read
    StartRead();
  } else if (error == boost::asio::error::eof ||
             error == boost::asio::error::connection_reset) {
    // Connection closed by peer
    spdlog::info("Connection closed by peer");
    Stop();
  } else {
    // Other error
    spdlog::error("Read error: {}", error.message());
    Stop();
  }
}

void Connection::StartWrite(const std::string& data) {
  auto self = shared_from_this();
  write_buffer_ = data;
  spdlog::debug("Sending: {}", write_buffer_);

  boost::asio::async_write(
      socket_,
      boost::asio::buffer(write_buffer_),
      [self](const boost::system::error_code& error, std::size_t bytes_transferred) {
        self->HandleWrite(error, bytes_transferred);
      });
}

void Connection::HandleWrite(const boost::system::error_code& error,
                             std::size_t /*bytes_transferred*/) {
  if (error) {
    spdlog::error("Write error: {}", error.message());
    Stop();
  }
}

}  // namespace internal
}  // namespace tcp_server 