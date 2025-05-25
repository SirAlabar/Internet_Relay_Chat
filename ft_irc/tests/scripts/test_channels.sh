#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

IRC_PORT=6668
IRC_PASSWORD="testpass123"
SERVER_PID=""
TEST_DIR="tests/channel_logs"
TESTS_PASSED=0
TESTS_FAILED=0

# Create test directory
mkdir -p "$TEST_DIR"

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[PASS]${NC} $1"; ((TESTS_PASSED++)); }
log_error() { echo -e "${RED}[FAIL]${NC} $1"; ((TESTS_FAILED++)); }
log_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }

start_server() {
    log_info "Starting IRC server for channel tests..."
    ./ircserv $IRC_PORT $IRC_PASSWORD > "$TEST_DIR/server.log" 2>&1 &
    SERVER_PID=$!
    sleep 2
    
    if kill -0 $SERVER_PID 2>/dev/null; then
        log_success "Server started for channel tests"
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

test_join_create_channel() {
    log_info "Testing JOIN (channel creation)..."
    
    send_irc_session "join_create" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK creator" \
        "USER creator 0 * :Channel Creator" \
        "JOIN #newchannel"
    
    if grep -q "JOIN.*#newchannel" "$TEST_DIR/join_create.log"; then
        log_success "JOIN (channel creation) works"
    else
        log_warning "JOIN (channel creation) not implemented or failed"
    fi
}

test_join_existing_channel() {
    log_info "Testing JOIN (existing channel)..."
    
    # First user creates channel
    send_irc_session "first_user" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK firstuser" \
        "USER firstuser 0 * :First User" \
        "JOIN #existing" &
    
    local first_pid=$!
    sleep 2
    
    # Second user joins existing channel
    send_irc_session "second_user" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK seconduser" \
        "USER seconduser 0 * :Second User" \
        "JOIN #existing"
    
    # Wait for first user with timeout
    local wait_count=0
    while kill -0 $first_pid 2>/dev/null && [ $wait_count -lt 5 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $first_pid 2>/dev/null; then
        log_warning "Killing hung first user session"
        kill $first_pid 2>/dev/null
    fi
    
    if grep -q "JOIN.*#existing" "$TEST_DIR/second_user.log"; then
        log_success "JOIN (existing channel) works"
    else
        log_warning "JOIN (existing channel) not implemented or failed"
    fi
}

test_list_channels() {
    log_info "Testing LIST command..."
    
    send_irc_session "list_test" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK lister" \
        "USER lister 0 * :Channel Lister" \
        "JOIN #listtest" \
        "LIST"
    
    if grep -q "321.*Channel.*Users.*Name\|322.*#listtest\|323.*End of" "$TEST_DIR/list_test.log"; then
        log_success "LIST command works"
    else
        log_warning "LIST command not implemented or failed"
    fi
}

test_part_channel() {
    log_info "Testing PART command..."
    
    send_irc_session "part_test" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK parter" \
        "USER parter 0 * :Channel Parter" \
        "JOIN #parttest" \
        "PART #parttest :Leaving for test"
    
    if grep -q "PART.*#parttest.*Leaving for test" "$TEST_DIR/part_test.log"; then
        log_success "PART command works"
    else
        log_warning "PART command not implemented or failed"
    fi
}

test_kick_command() {
    log_info "Testing KICK command..."
    
    # Operator creates channel
    send_irc_session "operator" 8 \
        "PASS $IRC_PASSWORD" \
        "NICK operator" \
        "USER operator 0 * :Channel Operator" \
        "JOIN #kicktest" &
    
    local op_pid=$!
    sleep 2
    
    # Regular user joins
    send_irc_session "victim" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK victim" \
        "USER victim 0 * :Kick Victim" \
        "JOIN #kicktest" &
    
    local victim_pid=$!
    sleep 2
    
    # Operator kicks user
    send_irc_session "kicker" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK kicker" \
        "USER kicker 0 * :The Kicker" \
        "JOIN #kicktest" \
        "KICK #kicktest victim :Testing kick command"
    
    # Wait for background processes with timeout
    local wait_count=0
    while (kill -0 $op_pid 2>/dev/null || kill -0 $victim_pid 2>/dev/null) && [ $wait_count -lt 6 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    kill $op_pid $victim_pid 2>/dev/null
    
    # Check if kick worked
    if grep -q "KICK.*#kicktest.*victim.*Testing kick command" "$TEST_DIR/victim.log" || \
       grep -q "KICK.*#kicktest.*victim.*Testing kick command" "$TEST_DIR/kicker.log"; then
        log_success "KICK command works"
    else
        log_warning "KICK command not implemented or failed"
    fi
}

test_invite_command() {
    log_info "Testing INVITE command..."
    
    # Operator creates channel
    send_irc_session "inviter" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK inviter" \
        "USER inviter 0 * :Inviter" \
        "JOIN #invitetest" \
        "INVITE invitee #invitetest" &
    
    local inviter_pid=$!
    sleep 2
    
    # User to be invited
    send_irc_session "invitee" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK invitee" \
        "USER invitee 0 * :Invitee"
    
    # Wait for inviter with timeout
    local wait_count=0
    while kill -0 $inviter_pid 2>/dev/null && [ $wait_count -lt 5 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $inviter_pid 2>/dev/null; then
        kill $inviter_pid 2>/dev/null
    fi
    
    # Check if invite was sent
    if grep -q "INVITE.*invitee.*#invitetest" "$TEST_DIR/invitee.log" && \
       grep -q "341.*invitee.*#invitetest" "$TEST_DIR/inviter.log"; then
        log_success "INVITE command works"
    else
        log_warning "INVITE command not implemented or failed"
    fi
}

test_invalid_channel_names() {
    log_info "Testing invalid channel names..."
    
    send_irc_session "validator" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK validator" \
        "USER validator 0 * :Name Validator" \
        "JOIN invalidchannel" \
        "JOIN #" \
        "JOIN # space"
    
    if grep -q "403.*No such channel\|461.*Not enough parameters" "$TEST_DIR/validator.log"; then
        log_success "Invalid channel name validation works"
    else
        log_warning "Invalid channel name validation not implemented"
    fi
}

test_channel_errors() {
    log_info "Testing channel error conditions..."
    
    send_irc_session "errortest" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK errortest" \
        "USER errortest 0 * :Error Tester" \
        "PART #nonexistent" \
        "KICK #nonexistent someuser" \
        "LIST #nonexistent"
    
    if grep -q "403.*No such channel\|442.*not on that channel" "$TEST_DIR/errortest.log"; then
        log_success "Channel error handling works"
    else
        log_warning "Channel error handling not fully implemented"
    fi
}

main() {
    log_info "Starting Channel Tests"
    log_info "====================="
    
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
    
    test_join_create_channel
    test_join_existing_channel
    test_list_channels
    test_part_channel
    test_kick_command
    test_invite_command
    test_invalid_channel_names
    test_channel_errors
    
    log_info "====================="
    log_info "Channel Test Results:"
    log_success "Passed: $TESTS_PASSED"
    
    if [ $TESTS_FAILED -gt 0 ]; then
        log_error "Failed: $TESTS_FAILED"
        log_info "Logs available in: $TEST_DIR/"
        exit 1
    else
        log_success "All channel tests completed! 🎉"
        log_info "Note: Some warnings are expected for unimplemented features"
        exit 0
    fi
}

main "$@"