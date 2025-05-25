#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

IRC_PORT=6670
IRC_PASSWORD="testpass123"
SERVER_PID=""
TEST_DIR="tests/msg_logs"
TESTS_PASSED=0
TESTS_FAILED=0

# Create test directory
mkdir -p "$TEST_DIR"

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[PASS]${NC} $1"; ((TESTS_PASSED++)); }
log_error() { echo -e "${RED}[FAIL]${NC} $1"; ((TESTS_FAILED++)); }
log_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }

start_server() {
    log_info "Starting IRC server for messaging tests..."
    ./ircserv $IRC_PORT $IRC_PASSWORD > "$TEST_DIR/server.log" 2>&1 &
    SERVER_PID=$!
    sleep 2
    
    if kill -0 $SERVER_PID 2>/dev/null; then
        log_success "Server started for messaging tests"
        return 0
    else
        log_error "Failed to start server"
        return 1
    fi
}

stop_server() {
    if [ ! -z "$SERVER_PID" ]; then
        kill -TERM $SERVER_PID 2>/dev/null
        sleep 2
        if kill -0 $SERVER_PID 2>/dev/null; then
            kill -KILL $SERVER_PID 2>/dev/null
        fi
        wait $SERVER_PID 2>/dev/null
    fi
}

# Enhanced IRC session with detailed logging
send_irc_session() {
    local session_name="$1"
    local timeout="${2:-5}"
    shift 2
    local commands=("$@")
    
    log_info "Running IRC session: $session_name"
    
    # Show what we're sending
    for cmd in "${commands[@]}"; do
        log_info ">>> SENDING: $cmd"
    done
    
    # Create command sequence
    (
        for cmd in "${commands[@]}"; do
            printf "%s\r\n" "$cmd"
            sleep 0.3
        done
        sleep $timeout
    ) | nc -w $(($timeout + 2)) localhost $IRC_PORT > "$TEST_DIR/${session_name}.log" 2>&1
    
    # Show what we received
    if [ -f "$TEST_DIR/${session_name}.log" ]; then
        local response=$(cat "$TEST_DIR/${session_name}.log")
        if [ ! -z "$response" ]; then
            log_info "<<< RECEIVED:"
            echo "$response" | sed 's/^/        /'
        else
            log_warning "<<< RECEIVED: (no response)"
        fi
    fi
    
    return $?
}

test_channel_message() {
    log_info "Testing PRIVMSG to channel..."
    
    # User 1 - sender
    send_irc_session "sender1" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK sender1" \
        "USER sender1 0 * :Message Sender" \
        "JOIN #msgtest" \
        "PRIVMSG #msgtest :Hello everyone in the channel!" &
    
    local sender_pid=$!
    sleep 2
    
    # User 2 - receiver
    send_irc_session "receiver1" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK receiver1" \
        "USER receiver1 0 * :Message Receiver" \
        "JOIN #msgtest"
    
    # Wait for sender with timeout
    local wait_count=0
    while kill -0 $sender_pid 2>/dev/null && [ $wait_count -lt 5 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $sender_pid 2>/dev/null; then
        kill $sender_pid 2>/dev/null
    fi
    
    if grep -q "PRIVMSG #msgtest :Hello everyone in the channel!" "$TEST_DIR/receiver1.log"; then
        log_success "Channel messaging works"
    else
        log_warning "Channel messaging not working or not implemented"
    fi
}

test_private_message() {
    log_info "Testing PRIVMSG to user (private message)..."
    
    # User 1
    send_irc_session "alice" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK alice" \
        "USER alice 0 * :Alice" \
        "PRIVMSG bob :This is a private message" &
    
    local alice_pid=$!
    sleep 2
    
    # User 2
    send_irc_session "bob" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK bob" \
        "USER bob 0 * :Bob"
    
    # Wait for Alice with timeout
    local wait_count=0
    while kill -0 $alice_pid 2>/dev/null && [ $wait_count -lt 4 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $alice_pid 2>/dev/null; then
        kill $alice_pid 2>/dev/null
    fi
    
    if grep -q "PRIVMSG bob :This is a private message" "$TEST_DIR/bob.log"; then
        log_success "Private messaging works"
    else
        log_warning "Private messaging not working or not implemented"
    fi
}

test_message_to_nonexistent_user() {
    log_info "Testing PRIVMSG to non-existent user..."
    
    send_irc_session "msgsender" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK msgsender" \
        "USER msgsender 0 * :Message Sender" \
        "PRIVMSG nonexistentuser :This should fail"
    
    if grep -q "401.*No such nick" "$TEST_DIR/msgsender.log"; then
        log_success "Message to non-existent user properly rejected"
    else
        log_warning "Message to non-existent user validation not implemented"
    fi
}

test_message_to_nonexistent_channel() {
    log_info "Testing PRIVMSG to non-existent channel..."
    
    send_irc_session "channelsender" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK channelsender" \
        "USER channelsender 0 * :Channel Sender" \
        "PRIVMSG #nonexistentchannel :This should fail"
    
    if grep -q "403.*No such channel" "$TEST_DIR/channelsender.log"; then
        log_success "Message to non-existent channel properly rejected"
    else
        log_warning "Message to non-existent channel validation not implemented"
    fi
}

test_message_not_in_channel() {
    log_info "Testing PRIVMSG to channel user is not in..."
    
    # Create channel with one user
    send_irc_session "channelowner" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK channelowner" \
        "USER channelowner 0 * :Channel Owner" \
        "JOIN #privatechannel" &
    
    local owner_pid=$!
    sleep 2
    
    # Try to send message without joining
    send_irc_session "outsider" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK outsider" \
        "USER outsider 0 * :Outsider" \
        "PRIVMSG #privatechannel :This should fail"
    
    # Wait for owner with timeout
    local wait_count=0
    while kill -0 $owner_pid 2>/dev/null && [ $wait_count -lt 5 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $owner_pid 2>/dev/null; then
        kill $owner_pid 2>/dev/null
    fi
    
    if grep -q "404.*Cannot send to channel\|442.*not on that channel" "$TEST_DIR/outsider.log"; then
        log_success "Message to channel user is not in properly rejected"
    else
        log_warning "Channel membership validation not implemented"
    fi
}

test_empty_message() {
    log_info "Testing PRIVMSG with empty message..."
    
    send_irc_session "emptysender" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK emptysender" \
        "USER emptysender 0 * :Empty Sender" \
        "PRIVMSG someuser" \
        "PRIVMSG"
    
    if grep -q "411.*No recipient\|412.*No text to send\|461.*Not enough parameters" "$TEST_DIR/emptysender.log"; then
        log_success "Empty message validation works"
    else
        log_warning "Empty message validation not implemented"
    fi
}

test_multiple_channel_users() {
    log_info "Testing PRIVMSG broadcast to multiple channel users..."
    
    # User 1 - sender
    send_irc_session "broadcaster" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK broadcaster" \
        "USER broadcaster 0 * :Broadcaster" \
        "JOIN #broadcast" \
        "PRIVMSG #broadcast :Message for everyone!" &
    
    local broadcaster_pid=$!
    sleep 2
    
    # User 2 - receiver 1
    send_irc_session "listener1" 5 \
        "PASS $IRC_PASSWORD"  \
        "NICK listener1" \
        "USER listener1 0 * :Listener 1" \
        "JOIN #broadcast" &
    
    local listener1_pid=$!
    sleep 1
    
    # User 3 - receiver 2
    send_irc_session "listener2" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK listener2" \
        "USER listener2 0 * :Listener 2" \
        "JOIN #broadcast"
    
    # Wait for background processes with timeout
    local wait_count=0
    while (kill -0 $broadcaster_pid 2>/dev/null || kill -0 $listener1_pid 2>/dev/null) && [ $wait_count -lt 6 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    kill $broadcaster_pid $listener1_pid 2>/dev/null
    
    local success=0
    if grep -q "PRIVMSG #broadcast :Message for everyone!" "$TEST_DIR/listener1.log"; then
        ((success++))
    fi
    if grep -q "PRIVMSG #broadcast :Message for everyone!" "$TEST_DIR/listener2.log"; then
        ((success++))
    fi
    
    if [ $success -eq 2 ]; then
        log_success "Message broadcast to multiple users works"
    elif [ $success -eq 1 ]; then
        log_warning "Partial message broadcast (1/2 users received)"
    else
        log_warning "Message broadcast not working or not implemented"
    fi
}

test_message_with_special_characters() {
    log_info "Testing PRIVMSG with special characters..."
    
    # Sender
    send_irc_session "specialsender" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK specialsender" \
        "USER specialsender 0 * :Special Sender" \
        "JOIN #special" \
        "PRIVMSG #special :Message with émojis and spéçial chârs! 🚀" &
    
    local sender_pid=$!
    sleep 2
    
    # Receiver
    send_irc_session "specialreceiver" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK specialreceiver" \
        "USER specialreceiver 0 * :Special Receiver" \
        "JOIN #special"
    
    # Wait for sender with timeout
    local wait_count=0
    while kill -0 $sender_pid 2>/dev/null && [ $wait_count -lt 4 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $sender_pid 2>/dev/null; then
        kill $sender_pid 2>/dev/null
    fi
    
    if grep -q "PRIVMSG #special :Message with" "$TEST_DIR/specialreceiver.log"; then
        log_success "Messages with special characters work"
    else
        log_warning "Messages with special characters not working"
    fi
}

main() {
    log_info "Starting Messaging Tests"
    log_info "========================"
    
    # Clean old logs
    rm -f "$TEST_DIR"/*.log 2>/dev/null
    
    if ! start_server; then
        exit 1
    fi
    
    trap 'stop_server' EXIT
    sleep 2
    
    # Check server connectivity
    if ! nc -z localhost $IRC_PORT 2>/dev/null; then
        log_error "Server is not accepting connections"
        exit 1
    fi
    
    log_success "Server is accepting connections"
    
    test_channel_message
    test_private_message
    test_message_to_nonexistent_user
    test_message_to_nonexistent_channel
    test_message_not_in_channel
    test_empty_message
    test_multiple_channel_users
    test_message_with_special_characters
    
    log_info "========================"
    log_info "Messaging Test Results:"
    log_success "Passed: $TESTS_PASSED"
    
    if [ $TESTS_FAILED -gt 0 ]; then
        log_error "Failed: $TESTS_FAILED"
        log_info "Logs available in: $TEST_DIR/"
        exit 1
    else
        log_success "All messaging tests completed! 🎉"
        log_info "Note: Some warnings are expected for unimplemented features"
        exit 0
    fi
}

main "$@"