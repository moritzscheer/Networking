#!/bin/bash

CURRENT_DIR="$(pwd)"

# 1. Project Location
PROJECT_DIR="/opt/server"
PROJECT_NAME="server"

# Define log file path
LOG_FILE="/var/log/nginx/error.log"

# Define log rotation parameters
COMPRESS="compress"
ROTATE_SIZE="10M"

move_directory()
{
    fprintf "[1/5] Moving files to $PROJECT_DIR... "

    # Check if the project directory already exists in /etc
    if [ -d "/etc/$PROJECT_NAME" ]; then
        echo "Error: Project directory already exists in /etc. Please choose a different project name."
        exit 1
    fi

    # Move the current directory to /etc/$PROJECT_NAME
    mv "$CURRENT_DIR" "/etc/$PROJECT_NAME" || { echo "Failed to move directory to /etc/$PROJECT_NAME"; exit 1; }

    # Change directory to /etc/$PROJECT_NAME
    cd "/etc/$PROJECT_NAME" || { echo "Failed to change directory to /etc/$PROJECT_NAME"; exit 1; }

    fprintf "Done\n"
}

install_dependencies()
{
    echo "[2/5] Installing dependencies..."

    for dep in "${DEPENDENCIES[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            echo "Installing $dep..."
            # Add your package manager commands here
            # For example, you might use apt-get for Debian-based systems
            sudo apt-get install -y "$dep"
            # Or use yum for Red Hat-based systems
            # sudo yum install -y "$dep"
        else
            echo "$dep is already installed."
        fi
    done
    fprintf " Done\n"
}

build_and_install()
{
    echo "[3/5] Building and installing $PROJECT_NAME..."
}

configure_logging()
{
    echo "[4/5] configure logging..."

    # Check if log file exists
    if [ ! -f "$LOG_FILE" ]; then
        echo "Log file $LOG_FILE does not exist. Creating..."
        sudo touch "$LOG_FILE"
    fi

    # Modify log rotation configuration
    sudo sed -i '
        /^# Rotate log files every day/ { s/.*/daily/ }

        /^# Limit the size to specified value per file/ { s/.*/\tsize '$ROTATE_SIZE'/ }

        /^# Do not raise an error if the log file is missing/ { s/.*/\tmissingok/ }

        /^# Keep the last 7 log files and delete older ones/ { s/.*/\trotate 7/ }

        /^# Use Gzip compression/ { s/.*/\t'$COMPRESS'/ }

        /^# Delay compressing until the new file has been created/ { s/.*/\tDelaycompress/ }

        /^# Skip rotation if the file is empty/ { s/.*/\tnotifempty/ }

        /^# Recreate the file with this ownership and permissions/ { s/.*/\tcreate 0644 www-data www-data/ }
    ' /var/log/nginx/error.log

    # Restart syslog-ng service
    sudo service syslog-ng restart
}

main()
{
    if [ "$(uname)" != "Linux" ]; then
        echo "Oh no! You're not on a Unix-based operating system.
            This project is only compatible with Unix-based systems.
            Time to reconsider your choice of OS!"
            return
    fi

    echo "-----------------------------------------------"
    echo "       Project installation steps [0/5]        "
    echo "-----------------------------------------------"
    echo

    # Moves directory to /etc
    move_directory

    # Check if dependencies are installed
    install_dependencies

    # Build and install the project
    build_and_install

    # Configures logging file
    configure_logging

    echo
    echo "-----------------------------------------------"
    echo "     Installation completed successfully!      "
    echo "-----------------------------------------------"
}

main