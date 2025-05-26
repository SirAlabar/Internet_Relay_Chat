#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
IRC_PORT=6667
IRC_PASSWORD="testpass123"
SERVER_PID=""
TEST_DIR="tests/logs"

# Test results
TESTS_PASSED=0
TESTS_FAILED=0

# Utility functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
    ((TESTS_PASSED++))
}

log_error() {
    echo -e "${RED}[FAIL]${NC} $1"
    ((TESTS_FAILED++))
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Create test directory
mkdir -p "$TEST_DIR"

# Start IRC server
start_server() {
    log_info "Starting IRC server on port $IRC_PORT..."
    ./ircserv $IRC_PORT $IRC_PASSWORD > "$TEST_DIR/server.log" 2>&1 &
    SERVER_PID=$!
    sleep 3
    
    if kill -0 $SERVER_PID 2>/dev/null; then
        log_success "IRC server started (PID: $SERVER_PID)"
        return 0
    else
        log_error "Failed to start IRC server"
        return 1
    fi
}

# Stop IRC server
stop_server() {
    if [ ! -z "$SERVER_PID" ]; then
        log_info "Stopping IRC server (PID: $SERVER_PID)..."
        kill -TERM $SERVER_PID 2>/dev/null
        sleep 2
        if kill -0 $SERVER_PID 2>/dev/null; then
            kill -KILL $SERVER_PID 2>/dev/null
        fi
        wait $SERVER_PID 2>/dev/null
        log_info "IRC server stopped"
    fi
}

# Enhanced IRC command sender with detailed logging
send_irc_session() {
    local session_name="$1"
    local timeout="${2:-8}"
    shift 2
    local commands=("$@")
    
    log_info "Running IRC session: $session_name"
    
    # Show what we're sending
    for cmd in "${commands[@]}"; do
        log_info ">>> SENDING: $cmd"
    done
    
    # Create command sequence with proper IRC formatting
    (
        for cmd in "${commands[@]}"; do
            printf "%s\r\n" "$cmd"
            sleep 0.5  # Small delay between commands
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

# Test basic connection and authentication
test_authentication() {
    log_info "=== Testing Authentication ==="
    
    # Test correct authentication
    send_irc_session "auth_correct" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK testuser" \
        "USER testuser 0 * :Test User"
    
    sleep 1
    
    # Check for welcome message (001)
    if [ -f "$TEST_DIR/auth_correct.log" ] && grep -q ":server 001\|:server 1 " "$TEST_DIR/auth_correct.log"; then
        log_success "✅ Correct authentication works"
    else
        log_error "❌ Authentication failed"
        log_info "Expected: RPL_WELCOME (001)"
        log_info "Looking for patterns: ':server 001' or ':server 1 '"
    fi
    
    # Test wrong password
    send_irc_session "auth_wrong" 3 \
        "PASS wrongpassword" \
        "NICK testuser2" \
        "USER testuser2 0 * :Test User 2"
    
    if [ -f "$TEST_DIR/auth_wrong.log" ] && grep -q "464\|Password incorrect" "$TEST_DIR/auth_wrong.log"; then
        log_success "✅ Wrong password correctly rejected"
    else
        log_error "❌ Wrong password handling failed"
    fi
}

# Test PING/PONG
test_ping_pong() {
    log_info "=== Testing PING/PONG ==="
    
    send_irc_session "ping_test" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK pinguser" \
        "USER pinguser 0 * :Ping User" \
        "PING :testserver"
    
    if [ -f "$TEST_DIR/ping_test.log" ] && grep -q "PONG.*testserver" "$TEST_DIR/ping_test.log"; then
        log_success "✅ PING/PONG works correctly"
    else
        log_error "❌ PING/PONG failed"
    fi
}

# Test nickname collision with timeout protection
test_nick_collision() {
    log_info "=== Testing Nickname Collision ==="
    
    # First user - shorter timeout to avoid hanging
    send_irc_session "nick1" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK samenick" \
        "USER user1 0 * :User 1" &
    
    local first_pid=$!
    sleep 2
    
    # Second user with same nick - quick test
    send_irc_session "nick2" 3 \
        "PASS $IRC_PASSWORD" \
        "NICK samenick" \
        "USER user2 0 * :User 2"
    
    # Wait for first user but with timeout
    local wait_count=0
    while kill -0 $first_pid 2>/dev/null && [ $wait_count -lt 8 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $first_pid 2>/dev/null; then
        log_warning "Killing hung first user session"
        kill $first_pid 2>/dev/null
    fi
    
    if [ -f "$TEST_DIR/nick2.log" ] && grep -q "433\|Nickname is already in use" "$TEST_DIR/nick2.log"; then
        log_success "✅ Nickname collision correctly handled"
    else
        log_error "❌ Nickname collision not handled"
    fi
}

# Test channel operations
test_channel_operations() {
    log_info "=== Testing Channel Operations ==="
    
    # Note: These tests may fail if JOIN, LIST, PART are not implemented
    send_irc_session "channel_test" 8 \
        "PASS $IRC_PASSWORD" \
        "NICK channeluser" \
        "USER channeluser 0 * :Channel User" \
        "JOIN #testchannel" \
        "LIST" \
        "PART #testchannel :Leaving test"
    
    if [ -f "$TEST_DIR/channel_test.log" ]; then
        local response=$(cat "$TEST_DIR/channel_test.log")
        
        if echo "$response" | grep -q "JOIN.*#testchannel"; then
            log_success "✅ JOIN command works"
        else
            log_warning "⚠️  JOIN command not implemented or failed"
        fi
        
        if echo "$response" | grep -q "322.*#testchannel\|321.*Channel"; then
            log_success "✅ LIST command works"
        else
            log_warning "⚠️  LIST command not implemented or failed"
        fi
        
        if echo "$response" | grep -q "PART.*#testchannel"; then
            log_success "✅ PART command works"
        else
            log_warning "⚠️  PART command not implemented or failed"
        fi
    fi
}

# Test messaging with timeout protection
test_messaging() {
    log_info "=== Testing Messaging ==="
    
    # Start receiver with shorter timeout
    send_irc_session "msg_receiver" 8 \
        "PASS $IRC_PASSWORD" \
        "NICK receiver" \
        "USER receiver 0 * :Message Receiver" \
        "JOIN #msgtest" &
    
    local receiver_pid=$!
    sleep 3
    
    # Start sender
    send_irc_session "msg_sender" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK sender" \
        "USER sender 0 * :Message Sender" \
        "JOIN #msgtest" \
        "PRIVMSG #msgtest :Hello channel!" \
        "PRIVMSG receiver :Hello directly!"
    
    # Wait for receiver with timeout
    local wait_count=0
    while kill -0 $receiver_pid 2>/dev/null && [ $wait_count -lt 6 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $receiver_pid 2>/dev/null; then
        log_warning "Killing hung receiver session"
        kill $receiver_pid 2>/dev/null
    fi
    
    if [ -f "$TEST_DIR/msg_receiver.log" ]; then
        local receiver_log=$(cat "$TEST_DIR/msg_receiver.log")
        
        if echo "$receiver_log" | grep -q "PRIVMSG #msgtest :Hello channel!"; then
            log_success "✅ Channel messaging works"
        else
            log_warning "⚠️  Channel messaging not working or not implemented"
        fi
        
        if echo "$receiver_log" | grep -q "PRIVMSG receiver :Hello directly!"; then
            log_success "✅ Private messaging works"  
        else
            log_warning "⚠️  Private messaging not working or not implemented"
        fi
    else
        log_error "❌ No receiver log found"
    fi
}

# Test error handling
test_error_handling() {
    log_info "=== Testing Error Handling ==="
    
    send_irc_session "error_test" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK erroruser" \
        "USER erroruser 0 * :Error User" \
        "INVALIDCOMMAND" \
        "JOIN" \
        "PRIVMSG"
    
    if [ -f "$TEST_DIR/error_test.log" ]; then
        local response=$(cat "$TEST_DIR/error_test.log")
        
        if echo "$response" | grep -q "421.*Unknown command\|421.*INVALIDCOMMAND"; then
            log_success "✅ Unknown command error handling works"
        else
            log_error "❌ Unknown command error handling failed"
        fi
        
        if echo "$response" | grep -q "461.*Not enough parameters"; then
            log_success "✅ Parameter validation works"
        else
            log_warning "⚠️  Parameter validation may need improvement"
        fi
    fi
}

# Test CAP negotiation
test_cap_negotiation() {
    log_info "=== Testing CAP Negotiation ==="
    
    send_irc_session "cap_test" 5 \
        "CAP LS" \
        "CAP END" \
        "PASS $IRC_PASSWORD" \
        "NICK capuser" \
        "USER capuser 0 * :CAP User"
    
    if [ -f "$TEST_DIR/cap_test.log" ]; then
        local response=$(cat "$TEST_DIR/cap_test.log")
        
        if echo "$response" | grep -q "CAP.*LS"; then
            log_success "✅ CAP LS command works"
        else
            log_warning "⚠️  CAP LS response not found"
        fi
        
        if echo "$response" | grep -q ":server 001\|:server 1 "; then
            log_success "✅ Registration works after CAP negotiation"
        else
            log_error "❌ Registration after CAP negotiation failed"
        fi
    fi
}

# Test server stability
test_server_stability() {
    log_info "=== Testing Server Stability ==="
    
    if kill -0 $SERVER_PID 2>/dev/null; then
        log_success "✅ Server is still running after all tests"
    else
        log_error "❌ Server crashed during tests"
        return
    fi
    
    # Final connectivity test
    if nc -z localhost $IRC_PORT 2>/dev/null; then
        log_success "✅ Server is still accepting connections"
    else
        log_error "❌ Server is not accepting connections"
    fi
}

# Show server log excerpt
show_server_log() {
    local lines="${1:-30}"
    log_info "=== Server Log (last $lines lines) ==="
    if [ -f "$TEST_DIR/server.log" ]; then
        tail -$lines "$TEST_DIR/server.log" | sed 's/^/    /'
    else
        log_warning "Server log not found"
    fi
    log_info "=== End Server Log ==="
}

# Main test execution
main() {
    log_info "Starting Enhanced IRC Server Test Suite"
    log_info "========================================"
    
    # Clean up old logs
    rm -f "$TEST_DIR"/*.log 2>/dev/null
    
    # Start server
    if ! start_server; then
        log_error "Cannot start server, aborting tests"
        exit 1
    fi
    
    # Set up cleanup trap
    trap 'stop_server' EXIT
    
    # Wait for server to be ready
    sleep 2
    
    # Check if server is accepting connections
    if ! nc -z localhost $IRC_PORT 2>/dev/null; then
        log_error "Server is not accepting connections"
        show_server_log 50
        exit 1
    fi
    
    log_success "Server is accepting connections"
    
    # Run tests
    test_authentication
    test_ping_pong
    test_nick_collision
    test_cap_negotiation
    test_channel_operations
    test_messaging
    test_error_handling
    test_server_stability
    
    # Results
    log_info "========================================"
    log_info "Test Results Summary:"
    log_success "Tests passed: $TESTS_PASSED"
    
    if [ $TESTS_FAILED -gt 0 ]; then
        log_error "Tests failed: $TESTS_FAILED"
        log_info ""
        log_info "Debug Information:"
        show_server_log 50
        
        log_info "Test logs available in: $TEST_DIR/"
        ls -la "$TEST_DIR"/*.log 2>/dev/null | sed 's/^/    /'
        
        exit 1
    else
        log_success "All critical tests passed! 🎉"
        log_info "Test logs saved in: $TEST_DIR/"
        exit 0
    fi
}

# Run main function
main "$@"