#!/bin/bash

# IRC PING Test Script - Auto-reconnect version
# Tests server behavior when interrupted during PING/PONG exchange
# Automatically reconnects when connection is lost

HOST="localhost"
PORT="6666"
PASS="test"
NICK="pingtest"
USER="pingtest"
RECONNECT_DELAY=3

echo "Starting IRC PING test with auto-reconnect..."
echo "Server: $HOST:$PORT"
echo "Will automatically reconnect every $RECONNECT_DELAY seconds if connection lost"
echo "Press Ctrl+C HERE to stop the script completely"
echo "Press Ctrl+C on SERVER side to test the bug - script will reconnect"
echo "================================================================"

# Global ping counter to track across reconnections
GLOBAL_PING_COUNT=1

# Function to attempt connection and run ping loop
attempt_connection() {
    local connection_num="$1"
    
    echo "[$(date '+%H:%M:%S')] Connection attempt #$connection_num"
    
    # Function to send commands and read responses
    send_and_wait() {
        local cmd="$1"
        local wait_time="${2:-1}"
        echo "$cmd"
        sleep "$wait_time"
    }

    # Try to connect and run the session
    {
        echo "Sending login sequence..."
        send_and_wait "PASS $PASS" 0.5
        send_and_wait "NICK $NICK" 0.5  
        send_and_wait "USER $USER * * * *" 2
        
        echo "Login complete, starting PING loop..."
        echo "----------------------------------------"
        
        # Continuous PING loop
        while true; do
            TIMESTAMP=$(date +%s)
            send_and_wait "PING LAG$TIMESTAMP$GLOBAL_PING_COUNT" 0.05
            GLOBAL_PING_COUNT=$((GLOBAL_PING_COUNT + 1))
        done
        
    } | timeout 300 nc -C "$HOST" "$PORT" 2>/dev/null
    
    return $?
}

# Main reconnection loop
connection_attempt=1
while true; do
    attempt_connection $connection_attempt
    exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo "[$(date '+%H:%M:%S')] Connection ended normally"
    else
        echo "[$(date '+%H:%M:%S')] Connection lost (exit code: $exit_code)"
    fi
    
    echo "[$(date '+%H:%M:%S')] Waiting $RECONNECT_DELAY seconds before reconnecting..."
    sleep $RECONNECT_DELAY
    
    connection_attempt=$((connection_attempt + 1))
    echo
done
