# Sample Using Installed TcpServer Library

This sample demonstrates how to implement an application server using the installed `TcpServer` library.

## Overview

`application_server` is a server that responds to client messages based on predefined response patterns.

## Build Instructions

```bash
# Create and move to build directory
mkdir build && cd build

# Configure project with CMake
cmake ..

# Build
make
```

## Running the Server

```bash
# Start with default port (8765)
./application_server

# Start with custom port
./application_server 9999
```

## Usage

After starting the server, you can connect using netcat or telnet and send commands:

```bash
# Connect using netcat
nc localhost 8765
```

Available commands:

- `hello`: Returns a greeting
- `help`: Shows list of available commands
- `info`: Shows server information
- `time`: Returns time information
- `date`: Returns date information
- `weather`: Returns weather information
- `exit`: Returns goodbye message

For any unrecognized command, the server will return a help message.

## Design Details

This sample consists of the following components:

1. **ResponseProvider**: Class that manages request-response patterns
2. **Message Handler**: Function that processes received messages and generates responses
3. **Signal Handling**: Mechanism to properly handle Ctrl+C and other signals

The TcpServer library is referenced from the system installation path (external to the project).

## Requirements

- TcpServer library must be installed in `/usr/local/lib`
- If using a different installation path, adjust the `PATHS` in CMakeLists.txt accordingly 