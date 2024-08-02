#!/bin/bash

# Web server configuration

# ngtcp2 configuration
export NGTCP2_LOG_LEVEL="info"           # Options: trace, debug, info, warn, error, fatal
export NGTCP2_MAX_CONCURRENT_STREAMS=100
export NGTCP2_MAX_DATA=10485760          # Maximum data size in bytes

# nghttp3 configuration
export NGHTTP3_LOG_LEVEL="info"           # Options: trace, debug, info, warn, error, fatal
export NGHTTP3_MAX_CONCURRENT_STREAMS=100
export NGHTTP3_MAX_DATA=10485760          # Maximum data size in bytes

# Database configuration
export DB_HOST="localhost"                # Database host
export DB_PORT=5432                       # Database port
export DB_NAME="mydatabase"               # Database name
export DB_USER="myuser"                   # Database username
export DB_PASSWORD="mypassword"           # Database password
export DB_POOL_SIZE=10                    # Maximum number of database connections

# TLS configuration
export TLS_CERT_FILE="/path/to/cert.pem"  # Path to TLS certificate file
export TLS_KEY_FILE="/path/to/key.pem"    # Path to TLS key file
export TLS_CA_FILE="/path/to/ca.pem"      # Path to CA certificate file
export TLS_ALPN_PROTOCOLS="h3"            # ALPN protocols for HTTP/3

# Logging configuration
export LOG_LEVEL="info"                   # Logging level: trace, debug, info, warn, error, fatal
export LOG_FILE="/var/log/myserver.log"   # Path to the log file
export LOG_MAX_SIZE="10M"                 # Maximum size of the log file
export LOG_MAX_BACKUP="5"                 # Number of backup log files to keep

# Server configuration
export SERVER_HOST="0.0.0.0"              # Server IP address to bind to
export SERVER_PORT=8080                   # Server port number

# Debugging and performance
export DEBUG_MODE=false                  # Enable or disable debugging mode
export PERFORMANCE_MONITORING=true       # Enable or disable performance monitoring

# Miscellaneous
export MAX_REQUEST_SIZE=10485760         # Maximum request size in bytes
export TIMEOUT=60                        # Timeout in seconds