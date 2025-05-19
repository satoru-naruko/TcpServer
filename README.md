# TCP Server Library

C++17で実装された多重化TCPサーバーライブラリです。

## 機能

- 複数のクライアント接続を同時に処理（最大8接続）
- 非同期I/O処理（Boost.Asio使用）
- マルチスレッドワーカープール
- カスタムメッセージハンドラによる柔軟なレスポンス処理
- クロスプラットフォーム対応（Windows/Linux）

## 必要条件

- C++17対応コンパイラ
- CMake 3.20以上
- Boost 1.71.0以上
- spdlog
- GoogleTest（テスト実行時のみ）

## ビルド方法

### Visual Studio 2022でのビルド

1. 事前準備
   ```powershell
   # vcpkgをインストール（まだの場合）
   cd C:\
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install

   # 必要なパッケージをインストール
   .\vcpkg install boost:x64-windows
   .\vcpkg install spdlog:x64-windows
   .\vcpkg install gtest:x64-windows

   # 環境変数の設定
   # システムの環境変数に以下を追加（コントロールパネルから設定）
   # VCPKG_ROOT = C:\vcpkg
   ```

2. コマンドラインでのビルド
   ```powershell
   # プロジェクトのディレクトリに移動
   cd C:\work\repos\TcpServer

   # ビルドディレクトリを作成
   mkdir build
   cd build

   # CMakeでプロジェクトを構成（Debug版）
   cmake .. -G "Visual Studio 17 2022" -A x64 ^
     -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
     -DCMAKE_BUILD_TYPE=Debug

   # ビルドを実行
   cmake --build . --config Debug

   # テストを実行
   ctest -C Debug --output-on-failure
   ```

3. Visual Studio IDEでのビルド
   - Visual Studio 2022を起動
   - メニューから「ファイル」→「開く」→「CMake」を選択
   - プロジェクトのルートディレクトリにある`CMakeLists.txt`を選択
   - CMakeの設定が完了するまで待機（数分かかる場合があります）
   - 以下の手順でビルドとテストを実行：
     1. ツールバーの構成を`x64-Debug`に設定
     2. 「ビルド」→「すべてビルド」を選択（または F7 キー）
     3. 「テスト」→「テストエクスプローラー」を開く
     4. テストエクスプローラーの「すべてのテストを実行」をクリック

4. サンプルの実行
   ```powershell
   # コマンドラインから実行する場合（build/Debugディレクトリから）
   cd C:\work\repos\TcpServer\build\Debug\examples
   .\echo_server.exe

   # ポートを指定する場合
   .\echo_server.exe 8080
   ```

   Visual Studio IDEから実行する場合：
   - ソリューションエクスプローラーで`examples/echo_server`を右クリック
   - 「デバッグ」→「新しいインスタンスとして開始」を選択

5. トラブルシューティング
   - vcpkgのパッケージが見つからない場合：
     ```powershell
     # vcpkgのパッケージリストを更新
     cd C:\vcpkg
     git pull
     .\vcpkg update
     ```
   - CMakeがvcpkgを見つけられない場合：
     - `CMakeSettings.json`の`CMAKE_TOOLCHAIN_FILE`パスが正しいか確認
     - 環境変数`VCPKG_ROOT`が正しく設定されているか確認

### WSL（Windows Subsystem for Linux）でのビルド

```bash
# 必要なパッケージをインストール
sudo apt update
sudo apt install build-essential cmake libboost-all-dev libspdlog-dev libgtest-dev

# ビルドディレクトリを作成して移動
mkdir build && cd build

# CMakeでプロジェクトを構成
cmake ..

# ビルド
make

# テスト実行
ctest
```

### 実行例

エコーサーバーの例を実行するには：

```bash
# ビルドディレクトリから
./examples/echo_server

# ポートを指定する場合
./examples/echo_server 8080
```

## 使用方法

サーバーを作成して実行する例：

```cpp
#include "tcp_server/tcp_server.h"
#include <string>

int main() {
  // メッセージハンドラを定義
  auto message_handler = [](const std::string& message) -> std::string {
    return "応答: " + message;
  };
  
  // ポート12345でTCPサーバーを作成
  tcp_server::TcpServer server(12345, message_handler);
  
  // サーバーを起動（2スレッドで）
  server.Start(2);
  
  // メインスレッドを停止させないためのループ
  while (server.IsRunning()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  
  return 0;
}
```

## ライセンス

MITライセンス

## プロジェクト構造

```
TcpServer/
├── CMakeLists.txt           # メインのCMakeファイル
├── include/                 # 公開ヘッダー
│   └── tcp_server/
│       ├── tcp_server.h     # メインTCPサーバークラス
│       └── version.h        # バージョン情報
├── src/                     # ソースファイル
│   ├── tcp_server.cpp       # TCPサーバーの実装
│   └── internal/            # 内部実装のディレクトリ
│       ├── connection.h     # 接続クラスのヘッダー
│       └── connection.cpp   # 接続クラスの実装
├── tests/                   # テストディレクトリ
│   ├── CMakeLists.txt       # テスト用CMakeファイル
│   └── tcp_server_test.cpp  # ユニットテスト
└── examples/                # サンプルコード
    ├── CMakeLists.txt       # サンプル用CMakeファイル
    └── echo_server.cpp      # エコーサーバーの例
```