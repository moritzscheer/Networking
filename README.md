# Embedded Webserver with HTTP/3 Support

## Overview

This project is a high-performance webserver designed for embedded systems like the Raspberry Pi running on Linux. The webserver leverages the IO_uring framework for efficient I/O operations and the ngtcp2 library to provide HTTP/3 support with multithreading capabilities.

## Features

- **HTTP/3 Support**: Powered by the ngtcp2 library.
- **Efficient I/O**: Utilizes the IO_uring framework for asynchronous I/O operations.
- **Multithreaded**: Supports concurrent request handling for improved performance.
- **Embedded System Optimization**: Designed to run efficiently on devices like the Raspberry Pi.

## Requirements

- **Hardware**: Raspberry Pi or any similar embedded system.
- **Operating System**: Linux (preferably the latest version).
- **Libraries**:
  - [liburing](https://github.com/axboe/liburing)
  - [ngtcp2](https://github.com/ngtcp2/ngtcp2)
  - [nghttp3](https://github.com/ngtcp2/nghttp3)

## Installation

1. **Update your system**:
   ```sh
   sudo apt-get update
   sudo apt-get upgrade