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

# Test MODE command
test_mode_command() {
    log_info "=== Testing MODE Command ==="
    
    # Test MODE on channel by setting +i (invite-only)
    send_irc_session "mode_channel_set" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK modetest1" \
        "USER modetest1 0 * :Mode Tester 1" \
        "JOIN #modetest" \
        "MODE #modetest +i"
    
    if [ -f "$TEST_DIR/mode_channel_set.log" ]; then
        if grep -q "MODE #modetest \+i" "$TEST_DIR/mode_channel_set.log"; then
            log_success "✅ Channel mode +i set correctly"
        else
            log_warning "⚠️  Failed to detect MODE +i on channel"
        fi
    fi

    # Test MODE on channel by unsetting +i
    send_irc_session "mode_channel_unset" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK modetest2" \
        "USER modetest2 0 * :Mode Tester 2" \
        "JOIN #modetest" \
        "MODE #modetest -i"
    
    if [ -f "$TEST_DIR/mode_channel_unset.log" ]; then
        if grep -q "MODE #modetest -i" "$TEST_DIR/mode_channel_unset.log"; then
            log_success "✅ Channel mode -i unset correctly"
        else
            log_warning "⚠️  Failed to detect MODE -i on channel"
        fi
    fi

    # Test user MODE request (e.g., asking for their own modes)
    send_irc_session "mode_user" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK modetest3" \
        "USER modetest3 0 * :Mode Tester 3" \
        "MODE modetest3"
    
    if [ -f "$TEST_DIR/mode_user.log" ]; then
        if grep -q "221" "$TEST_DIR/mode_user.log"; then
            log_success "✅ User mode query (MODE <nick>) works"
        else
            log_warning "⚠️  User mode query might not be implemented"
        fi
    fi

    # Error case: MODE with missing parameters
    send_irc_session "mode_error" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK modetest4" \
        "USER modetest4 0 * :Mode Tester 4" \
        "MODE"
    
    if [ -f "$TEST_DIR/mode_error.log" ]; then
        if grep -q "461.*MODE" "$TEST_DIR/mode_error.log"; then
            log_success "✅ MODE error (not enough parameters) handled"
        else
            log_warning "⚠️  MODE error handling may be missing"
        fi
    fi
}

# Test full MODE behavior
test_mode_advanced() {
    log_info "=== Testing Advanced MODE Behavior ==="

    # Set +i (invite-only), then try to join without invite
    send_irc_session "mode_set_invite" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK opuser" \
        "USER opuser 0 * :Operator User" \
        "JOIN #secret" \
        "MODE #secret +i"

    send_irc_session "mode_join_denied" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK outsider" \
        "USER outsider 0 * :Outside User" \
        "JOIN #secret"

    if grep -q "473.*#secret" "$TEST_DIR/mode_join_denied.log"; then
        log_success "✅ Invite-only (+i) prevents unauthorized joins"
    else
        log_error "❌ Invite-only not enforced properly"
    fi

    # Set +t (topic only by op), then try to change topic as non-op
    send_irc_session "mode_set_topic" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK topicop" \
        "USER topicop 0 * :Topic Operator" \
        "JOIN #topicchan" \
        "MODE #topicchan +t"

    send_irc_session "mode_topic_fail" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK topicuser" \
        "USER topicuser 0 * :Topic User" \
        "JOIN #topicchan" \
        "TOPIC #topicchan :New topic"

    if grep -q "482.*#topicchan" "$TEST_DIR/mode_topic_fail.log"; then
        log_success "✅ Topic restriction (+t) enforced correctly"
    else
        log_error "❌ Topic restriction not enforced"
    fi

    # Set +k (password) and attempt join without password
    send_irc_session "mode_set_key" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK keyop" \
        "USER keyop 0 * :Key Op" \
        "JOIN #keychan" \
        "MODE #keychan +k secretpass"

    send_irc_session "mode_key_fail" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK keyfail" \
        "USER keyfail 0 * :Key Fail" \
        "JOIN #keychan"

    if grep -q "475.*#keychan" "$TEST_DIR/mode_key_fail.log"; then
        log_success "✅ Channel key (+k) blocks join without password"
    else
        log_error "❌ Channel key not enforced"
    fi

    # Set +l (limit to 1) and test second user blocked
    send_irc_session "mode_set_limit" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK limitop" \
        "USER limitop 0 * :Limit Op" \
        "JOIN #limitchan" \
        "MODE #limitchan +l 1"

    send_irc_session "mode_limit_fail" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK lim2" \
        "USER lim2 0 * :Too many users" \
        "JOIN #limitchan"

    if grep -q "471.*#limitchan" "$TEST_DIR/mode_limit_fail.log"; then
        log_success "✅ User limit (+l) enforced correctly"
    else
        log_error "❌ User limit not enforced"
    fi

    # Test giving operator (+o) to another user
    send_irc_session "mode_op_grant" 8 \
        "PASS $IRC_PASSWORD" \
        "NICK grantor" \
        "USER grantor 0 * :Op Giver" \
        "JOIN #optest" \
        "MODE #optest +o grantor"

    send_irc_session "mode_op_target" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK targetop" \
        "USER targetop 0 * :Target Op" \
        "JOIN #optest"

    send_irc_session "mode_op_give" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK grantor" \
        "USER grantor 0 * :Op Giver" \
        "MODE #optest +o targetop"

    if grep -q "MODE #optest \+o targetop" "$TEST_DIR/mode_op_give.log"; then
        log_success "✅ Operator privilege (+o) assigned correctly"
    else
        log_error "❌ Failed to assign operator"
    fi

    # Test removing +i, +t, +k, +l
    send_irc_session "mode_unset_all" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK unsetter" \
        "USER unsetter 0 * :Unset Modes" \
        "JOIN #unsettest" \
        "MODE #unsettest +i+t+k secret -i -t -k"

    if grep -q "MODE #unsettest.*-i" "$TEST_DIR/mode_unset_all.log"; then
        log_success "✅ Modes can be removed correctly"
    else
        log_warning "⚠️  Failed to verify mode removals"
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
    test_mode_command
    test_mode_advanced
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