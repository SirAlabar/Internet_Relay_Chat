#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

IRC_PORT=6669
IRC_PASSWORD="testpass123"
SERVER_PID=""
TEST_DIR="tests/conn_logs"
TESTS_PASSED=0
TESTS_FAILED=0

# Create test directory
mkdir -p "$TEST_DIR"

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[PASS]${NC} $1"; ((TESTS_PASSED++)); }
log_error() { echo -e "${RED}[FAIL]${NC} $1"; ((TESTS_FAILED++)); }
log_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }

start_server() {
    log_info "Starting IRC server for connection tests..."
    ./ircserv $IRC_PORT $IRC_PASSWORD > "$TEST_DIR/server.log" 2>&1 &
    SERVER_PID=$!
    sleep 2
    
    if kill -0 $SERVER_PID 2>/dev/null; then
        log_success "Server started for connection tests"
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

test_wrong_password() {
    log_info "Testing wrong password..."
    
    send_irc_session "wrong_pass" 3 \
        "PASS wrongpassword" \
        "NICK testuser" \
        "USER testuser 0 * :Test"
    
    if grep -q "464\|Password incorrect" "$TEST_DIR/wrong_pass.log"; then
        log_success "Wrong password correctly rejected"
    else
        log_error "Wrong password not properly handled"
    fi
}

test_correct_authentication() {
    log_info "Testing correct authentication..."
    
    send_irc_session "valid_auth" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK validuser" \
        "USER validuser 0 * :Valid User"
    
    if grep -q "001\|Welcome" "$TEST_DIR/valid_auth.log"; then
        log_success "Correct authentication works"
    else
        log_error "Correct authentication failed"
    fi
}

test_duplicate_nickname() {
    log_info "Testing duplicate nickname..."
    
    # First client with shorter timeout
    send_irc_session "nick1" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK samenick" \
        "USER user1 0 * :User 1" &
    
    local first_pid=$!
    sleep 2
    
    # Second client with same nick
    send_irc_session "nick2" 3 \
        "PASS $IRC_PASSWORD" \
        "NICK samenick" \
        "USER user2 0 * :User 2"
    
    # Wait for first client with timeout
    local wait_count=0
    while kill -0 $first_pid 2>/dev/null && [ $wait_count -lt 4 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $first_pid 2>/dev/null; then
        log_warning "Killing hung first client session"
        kill $first_pid 2>/dev/null
    fi
    
    if grep -q "433\|Nickname is already in use" "$TEST_DIR/nick2.log"; then
        log_success "Duplicate nickname correctly rejected"
    else
        log_error "Duplicate nickname not properly handled"
    fi
}

test_cap_negotiation() {
    log_info "Testing CAP negotiation..."
    
    send_irc_session "cap_test" 4 \
        "CAP LS" \
        "CAP END" \
        "PASS $IRC_PASSWORD" \
        "NICK captest" \
        "USER captest 0 * :Cap Test"
    
    if grep -q "CAP.*LS" "$TEST_DIR/cap_test.log" && grep -q "001\|Welcome" "$TEST_DIR/cap_test.log"; then
        log_success "CAP negotiation works"
    else
        log_error "CAP negotiation failed"
    fi
}

test_ping_pong() {
    log_info "Testing PING/PONG..."
    
    send_irc_session "ping_test" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK pingtest" \
        "USER pingtest 0 * :Ping Test" \
        "PING :testserver"
    
    if grep -q "PONG.*testserver" "$TEST_DIR/ping_test.log"; then
        log_success "PING/PONG works correctly"
    else
        log_error "PING/PONG failed"
    fi
}

test_registration_order() {
    log_info "Testing different registration orders..."
    
    # Test NICK before PASS
    send_irc_session "order1" 4 \
        "NICK ordertest1" \
        "PASS $IRC_PASSWORD" \
        "USER ordertest1 0 * :Order Test 1"
    
    # Test USER before NICK
    send_irc_session "order2" 4 \
        "PASS $IRC_PASSWORD" \
        "USER ordertest2 0 * :Order Test 2" \
        "NICK ordertest2"
    
    local success_count=0
    if grep -q "001\|Welcome.*ordertest1" "$TEST_DIR/order1.log"; then
        ((success_count++))
    fi
    if grep -q "001\|Welcome.*ordertest2" "$TEST_DIR/order2.log"; then
        ((success_count++))
    fi
    
    if [ $success_count -eq 2 ]; then
        log_success "Registration order flexibility works"
    elif [ $success_count -eq 1 ]; then
        log_warning "Partial registration order flexibility"
    else
        log_error "Registration order handling failed"
    fi
}

main() {
    log_info "Starting Connection Tests"
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
    
    test_wrong_password
    test_correct_authentication
    test_duplicate_nickname
    test_cap_negotiation
    test_ping_pong
    test_registration_order
    
    log_info "========================"
    log_info "Connection Test Results:"
    log_success "Passed: $TESTS_PASSED"
    
    if [ $TESTS_FAILED -gt 0 ]; then
        log_error "Failed: $TESTS_FAILED"
        log_info "Logs available in: $TEST_DIR/"
        exit 1
    else
        log_success "All connection tests passed! 🎉"
        exit 0
    fi
}

main "$@"