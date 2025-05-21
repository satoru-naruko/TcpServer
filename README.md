# TCP Server Library

A multiplexed TCP server library implemented in C++17.

## Features

- Handles multiple client connections simultaneously (up to 8 connections)
- Asynchronous I/O processing (using Boost.Asio)
- Multithreaded worker pool
- Flexible response processing via custom message handlers
- Cross-platform support (Windows/Linux)

## Requirements

- C++17 compatible compiler
- CMake 3.20 or later
- Boost 1.71.0 or later
- spdlog
- GoogleTest (for running tests only)

## Build Instructions

### Building with Visual Studio 2022

1. Preparation
   ```cmd
   # Install vcpkg (if not already installed)
   cd C:\
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install

   # Install required packages
   .\vcpkg install boost:x64-windows
   .\vcpkg install spdlog:x64-windows
   .\vcpkg install gtest:x64-windows

   # Set environment variables
   # Add the following to your system environment variables (via Control Panel)
   # VCPKG_ROOT = C:\vcpkg
   ```

2. Building from the Command Line

   a. Build and test in one step (recommended):
   ```cmd
   # Build and test with default settings (Debug configuration)
   scripts\windows\build\build-and-test.cmd

   # Build and test with Release configuration
   scripts\windows\build\build-and-test.cmd Release

   # Specify a custom vcpkg path for build and test
   scripts\windows\build\build-and-test.cmd Debug C:\work\vcpkg
   ```

   b. Build only:
   ```cmd
   # Build with default settings (Debug configuration)
   scripts\windows\build\build-utf8.cmd

   # Build with Release configuration
   scripts\windows\build\build-utf8.cmd Release

   # Specify a custom vcpkg path for build
   scripts\windows\build\build-utf8.cmd Debug C:\work\vcpkg
   ```
   
   c. Run tests only:
   ```cmd
   # Run tests for Debug build
   scripts\windows\test\run-tests.cmd

   # Run tests for Release build
   scripts\windows\test\run-tests.cmd Release
   ```

3. Building with Visual Studio IDE
   - Launch Visual Studio 2022
   - From the menu, select "File" → "Open" → "CMake..."
   - Select `CMakeLists.txt` in the project root directory
   - Wait for CMake configuration to complete (may take a few minutes)
   - To build and test:
     1. Set the toolbar configuration to `x64-Debug`
     2. Select "Build" → "Build All" (or press F7)
     3. Open "Test" → "Test Explorer"
     4. Click "Run All Tests" in the Test Explorer

4. Running the Sample
   ```cmd
   # From the command line (from build/Debug directory)
   cd build\Debug\examples
   echo_server.exe

   # To specify a port
   echo_server.exe 8080
   ```

   To run from Visual Studio IDE:
   - In Solution Explorer, right-click `examples/echo_server`
   - Select "Debug" → "Start New Instance"

5. Troubleshooting
   - If vcpkg packages are not found:
     ```cmd
     # Update vcpkg package list
     cd C:\work\vcpkg
     git pull
     .\vcpkg update
     ```
   - If CMake cannot find vcpkg:
     - Check that the `CMAKE_TOOLCHAIN_FILE` path in `CMakeSettings.json` is correct
     - Ensure the `VCPKG_ROOT` environment variable is set correctly

### Building with WSL (Windows Subsystem for Linux)

```bash
# Install required packages
sudo apt update
sudo apt install build-essential cmake libboost-all-dev libspdlog-dev libgtest-dev

# Create and move to build directory
mkdir build && cd build

# Configure project with CMake
cmake ..

# Build
make

# Install
make install

# By default, installs under /usr/local
# To change the install location, specify as follows
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
make install

# Run tests
ctest
```

### Example Usage

To run the echo server example:

```bash
# From the build directory
./examples/echo_server

# To specify a port
./examples/echo_server 8080
```

## Usage

Example of creating and running a server:

```cpp
#include "tcp_server/tcp_server.h"
#include <string>

int main() {
  // Define message handler
  auto message_handler = [](const std::string& message) -> std::string {
    return "Response: " + message;
  };
  
  // Create TCP server on port 12345
  tcp_server::TcpServer server(12345, message_handler);
  
  // Start server (with 2 threads)
  server.Start(2);
  
  // Loop to prevent main thread from exiting
  while (server.IsRunning()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  
  return 0;
}
```

## License

MIT License

## Project Structure

```
TcpServer/
├── CMakeLists.txt           # Main CMake file
├── include/                 # Public headers
│   └── tcp_server/
│       ├── tcp_server.h     # Main TCP server class
│       └── version.h        # Version info
├── scripts/                 # Build scripts
│   └── windows/            # Windows scripts
│       ├── build/          # Build scripts
│       │   ├── build-utf8.cmd
│       │   └── build-and-test.cmd
│       └── test/           # Test scripts
│           └── run-tests.cmd
├── src/                     # Source files
│   ├── tcp_server.cpp       # TCP server implementation
│   └── internal/            # Internal implementation
│       ├── connection.h     # Connection class header
│       └── connection.cpp   # Connection class implementation
├── tests/                   # Test directory
│   ├── CMakeLists.txt       # Test CMake file
│   └── tcp_server_test.cpp  # Unit tests
└── examples/                # Sample code
    ├── CMakeLists.txt       # Sample CMake file
    └── echo_server.cpp      # Echo server example
```