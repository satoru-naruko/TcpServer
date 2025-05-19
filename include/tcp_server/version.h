/**
 * @file version.h
 * @brief TcpServerのバージョン情報を定義
 */

#ifndef TCP_SERVER_VERSION_H_
#define TCP_SERVER_VERSION_H_

namespace tcp_server {

/**
 * @brief バージョン情報
 */
struct Version {
  static constexpr int kMajor = 0;  ///< メジャーバージョン
  static constexpr int kMinor = 1;  ///< マイナーバージョン
  static constexpr int kPatch = 0;  ///< パッチバージョン
};

}  // namespace tcp_server

#endif  // TCP_SERVER_VERSION_H_ 