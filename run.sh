#!/bin/bash

SCRIPT_DIR=$(dirname "${BASH_SOURCE[0]}")

PROJECT_NAME="server"

SERVER_COMMAND="./my_server"
SERVER_PORT=8080

check_for_errors()
{
  # Check if the script directory is equal to the desired path
  if [ "$SCRIPT_DIR" = "$DESIRED_PATH" ]; then
      echo "Error: This script must be located in $DESIRED_PATH"
      exit 1
  fi

  # Check if the binary exists in /etc/$PROJECT_NAME/bin
  if [ ! -x "$DESIRED_PATH/bin/$PROJECT_NAME" ]; then
      echo "Error: Server binary not found in $DESIRED_PATH/bin"
      exit 1
  fi
}

main()
{
    check_for_errors

    # shellcheck disable=SC2183
    FORMAT_PORT="$SERVER_PORT $(printf '%*s' "$((13 - ${#SERVER_PORT} - 1))" | tr ' ' '-')"

    echo "-----------------------------------------------"
    echo "---- Starting server on port $FORMAT_PORT -----"
    echo "-----------------------------------------------"
    echo

    $SERVER_COMMAND --port "$SERVER_PORT" &

    # Capture the process ID (PID) of the server
    SERVER_PID=$!

    # Wait for a few seconds to allow the server to start
    sleep 2

    # Check if the server is running
    if ps -p $SERVER_PID > /dev/null; then
        echo "Server started successfully with PID $SERVER_PID."
    else
        echo "Failed to start the server."
    fi
}

main