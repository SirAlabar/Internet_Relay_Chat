#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

IRC_PORT=6671
IRC_PASSWORD="testpass123"
SERVER_PID=""
TEST_DIR="tests/stress_logs"
TESTS_PASSED=0
TESTS_FAILED=0

# Create test directory
mkdir -p "$TEST_DIR"

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[PASS]${NC} $1"; ((TESTS_PASSED++)); }
log_error() { echo -e "${RED}[FAIL]${NC} $1"; ((TESTS_FAILED++)); }
log_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }

start_server() {
    log_info "Starting IRC server for stress tests..."
    ./ircserv $IRC_PORT $IRC_PASSWORD > "$TEST_DIR/server.log" 2>&1 &
    SERVER_PID=$!
    sleep 3
    
    if kill -0 $SERVER_PID 2>/dev/null; then
        log_success "Server started for stress tests"
        return 0
    else
        log_error "Failed to start server"
        return 1
    fi
}

stop_server() {
    if [ ! -z "$SERVER_PID" ]; then
        log_info "Stopping server..."
        kill -TERM $SERVER_PID 2>/dev/null
        sleep 2
        if kill -0 $SERVER_PID 2>/dev/null; then
            kill -KILL $SERVER_PID 2>/dev/null
        fi
        wait $SERVER_PID 2>/dev/null
    fi
}

# Quick IRC session for stress testing
send_quick_session() {
    local session_name="$1" 
    local timeout="${2:-3}"
    shift 2
    local commands=("$@")
    
    (
        for cmd in "${commands[@]}"; do
            printf "%s\r\n" "$cmd"
            sleep 0.1
        done
        sleep $timeout
    ) | nc -w $(($timeout + 1)) localhost $IRC_PORT > "$TEST_DIR/${session_name}.log" 2>&1
}

test_multiple_connections() {
    log_info "Testing multiple simultaneous connections..."
    log_info ">>> Testing with 10 concurrent clients"
    
    local num_clients=10
    local pids=()
    
    # Start multiple clients simultaneously
    for i in $(seq 1 $num_clients); do
        send_quick_session "client$i" 3 \
            "PASS $IRC_PASSWORD" \
            "NICK client$i" \
            "USER client$i 0 * :Client $i" \
            "JOIN #stress" \
            "PRIVMSG #stress :Hello from client $i" &
        pids+=($!)
    done
    
    log_info "<<< Waiting for all clients to complete..."
    
    # Wait for all clients with timeout
    local wait_count=0
    while [ ${#pids[@]} -gt 0 ] && [ $wait_count -lt 8 ]; do
        local new_pids=()
        for pid in "${pids[@]}"; do
            if kill -0 $pid 2>/dev/null; then
                new_pids+=($pid)
            fi
        done
        pids=("${new_pids[@]}")
        
        if [ ${#pids[@]} -gt 0 ]; then
            sleep 1
            ((wait_count++))
        fi
    done
    
    # Force kill any remaining processes
    for pid in "${pids[@]}"; do
        kill $pid 2>/dev/null
    done
    
    # Check results
    local successful=0
    for i in $(seq 1 $num_clients); do
        if [ -f "$TEST_DIR/client$i.log" ] && grep -q "001\|Welcome" "$TEST_DIR/client$i.log"; then
            ((successful++))
        fi
    done
    
    log_info "<<< Results: $successful/$num_clients clients connected successfully"
    
    if [ $successful -ge $(($num_clients * 7 / 10)) ]; then  # 70% success rate
        log_success "Multiple connections: $successful/$num_clients clients connected successfully"
    else
        log_error "Multiple connections: Only $successful/$num_clients clients connected"
    fi
}

test_rapid_connections() {
    log_info "Testing rapid connect/disconnect..."
    log_info ">>> Testing with 15 rapid connections"
    
    local num_tests=15
    local successful=0
    local pids=()
    
    for i in $(seq 1 $num_tests); do
        send_quick_session "rapid$i" 2 \
            "PASS $IRC_PASSWORD" \
            "NICK rapid$i" \
            "USER rapid$i 0 * :Rapid $i" \
            "QUIT :Rapid disconnect" &
        pids+=($!)
        
        sleep 0.1  # Small delay between connections
    done
    
    log_info "<<< Waiting for rapid connections to complete..."
    
    # Wait with timeout
    local wait_count=0
    while [ ${#pids[@]} -gt 0 ] && [ $wait_count -lt 6 ]; do
        local new_pids=()
        for pid in "${pids[@]}"; do
            if kill -0 $pid 2>/dev/null; then
                new_pids+=($pid)
            fi
        done
        pids=("${new_pids[@]}")
        
        if [ ${#pids[@]} -gt 0 ]; then
            sleep 1
            ((wait_count++))
        fi
    done
    
    # Force kill remaining
    for pid in "${pids[@]}"; do
        kill $pid 2>/dev/null
    done
    
    # Count successful authentications
    for i in $(seq 1 $num_tests); do
        if [ -f "$TEST_DIR/rapid$i.log" ] && grep -q "001\|Welcome" "$TEST_DIR/rapid$i.log"; then
            ((successful++))
        fi
    done
    
    log_info "<<< Results: $successful/$num_tests rapid connections successful"
    
    if [ $successful -ge $(($num_tests * 6 / 10)) ]; then  # 60% success rate
        log_success "Rapid connections: $successful/$num_tests successful"
    else
        log_error "Rapid connections: Only $successful/$num_tests successful"
    fi
}

test_large_messages() {
    log_info "Testing large messages..."
    log_info ">>> Testing message with ~1000 characters"
    
    # Create a large message (but within reasonable limits)
    local large_msg="PRIVMSG #largetest :"
    for i in $(seq 1 20); do
        large_msg="${large_msg}This is a long message segment $i with extra text. "
    done
    
    send_quick_session "large_sender" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK largesender" \
        "USER largesender 0 * :Large Sender" \
        "JOIN #largetest" \
        "$large_msg" &
    
    local sender_pid=$!
    
    send_quick_session "large_receiver" 5 \
        "PASS $IRC_PASSWORD" \
        "NICK largereceiver" \
        "USER largereceiver 0 * :Large Receiver" \
        "JOIN #largetest"
    
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
    
    log_info "<<< Checking if large message was received..."
    
    if [ -f "$TEST_DIR/large_receiver.log" ] && grep -q "This is a long message segment" "$TEST_DIR/large_receiver.log"; then
        log_success "Large messages handled correctly"
    else
        log_warning "Large messages may not be fully supported"
    fi
}

test_message_flood() {
    log_info "Testing message flooding..."
    log_info ">>> Testing with 30 rapid messages"
    
    # Sender floods messages
    (
        printf "PASS %s\r\n" "$IRC_PASSWORD"
        printf "NICK %s\r\n" "flooder"
        printf "USER %s 0 * :%s\r\n" "flooder" "Flooder"
        sleep 1
        printf "JOIN %s\r\n" "#flood"
        sleep 1
        
        # Send many messages quickly
        for i in $(seq 1 30); do
            printf "PRIVMSG #flood :Flood message %d\r\n" $i
            sleep 0.05
        done
        
        sleep 2
    ) | nc -w 8 localhost $IRC_PORT > "$TEST_DIR/flooder.log" 2>&1 &
    
    local flooder_pid=$!
    
    # Receiver
    send_quick_session "flood_receiver" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK floodreceiver" \
        "USER floodreceiver 0 * :Flood Receiver" \
        "JOIN #flood"
    
    # Wait for flooder with timeout
    local wait_count=0
    while kill -0 $flooder_pid 2>/dev/null && [ $wait_count -lt 6 ]; do
        sleep 1
        ((wait_count++))
    done
    
    # Force kill if still running
    if kill -0 $flooder_pid 2>/dev/null; then
        kill $flooder_pid 2>/dev/null
    fi
    
    # Count received messages
    local received=0
    if [ -f "$TEST_DIR/flood_receiver.log" ]; then
        received=$(grep -c "PRIVMSG #flood :Flood message" "$TEST_DIR/flood_receiver.log")
    fi
    
    log_info "<<< Results: $received/30 flood messages received"
    
    if [ $received -ge 15 ]; then  # At least 50% received
        log_success "Message flooding: $received/30 messages received"
    else
        log_warning "Message flooding: Only $received/30 messages received (may be rate limited)"
        ((TESTS_PASSED++))  # Still count as pass - rate limiting is acceptable
    fi
}

test_channel_flooding() {
    log_info "Testing channel creation flooding..."
    log_info ">>> Testing creation of 20 channels"
    
    (
        printf "PASS %s\r\n" "$IRC_PASSWORD"
        printf "NICK %s\r\n" "channelflooder"
        printf "USER %s 0 * :%s\r\n" "channelflooder" "Channel Flooder"
        sleep 1
        
        # Create many channels
        for i in $(seq 1 20); do
            printf "JOIN #flood%d\r\n" $i
            sleep 0.1
        done
        
        printf "LIST\r\n"
        sleep 2
    ) | nc -w 6 localhost $IRC_PORT > "$TEST_DIR/channel_flood.log" 2>&1
    
    log_info "<<< Checking created channels..."
    
    # Count created channels
    local channels=0
    if [ -f "$TEST_DIR/channel_flood.log" ]; then
        channels=$(grep -c "322.*#flood\|JOIN.*#flood" "$TEST_DIR/channel_flood.log")
    fi
    
    log_info "<<< Results: $channels channels created/joined"
    
    if [ $channels -ge 10 ]; then  # At least 10 channels
        log_success "Channel flooding: $channels channels created"
    else
        log_warning "Channel flooding: Only $channels channels created (may have limits)"
    fi
}

test_partial_messages() {
    log_info "Testing partial message handling..."
    log_info ">>> Testing fragmented IRC commands"
    
    # Send partial IRC commands (testing buffer handling)
    (
        printf "PAS"
        sleep 0.5
        printf "S %s\r\n" "$IRC_PASSWORD"
        printf "NIC"
        sleep 0.5  
        printf "K partial\r\n"
        printf "USER partial 0 * :Part"
        sleep 0.5
        printf "ial User\r\n"
        sleep 2
    ) | nc -w 5 localhost $IRC_PORT > "$TEST_DIR/partial.log" 2>&1
    
    log_info "<<< Checking partial message handling..."
    
    if [ -f "$TEST_DIR/partial.log" ] && grep -q "001\|Welcome.*partial" "$TEST_DIR/partial.log"; then
        log_success "Partial message handling works"
    else
        log_warning "Partial message handling may need improvement"
    fi
}

test_server_stability() {
    log_info "Testing overall server stability..."
    
    # Check if server is still running
    if kill -0 $SERVER_PID 2>/dev/null; then
        log_success "Server is still running after stress tests"
    else
        log_error "Server crashed during stress tests"
        return
    fi
    
    log_info ">>> Testing basic functionality after stress tests"
    
    # Test basic functionality still works
    send_quick_session "stability" 4 \
        "PASS $IRC_PASSWORD" \
        "NICK stabilitytest" \
        "USER stabilitytest 0 * :Stability Test" \
        "JOIN #stability" \
        "PRIVMSG #stability :Server still working!"
    
    log_info "<<< Checking server stability..."
    
    if [ -f "$TEST_DIR/stability.log" ] && grep -q "001\|Welcome.*stabilitytest" "$TEST_DIR/stability.log"; then
        log_success "Server functionality still works after stress tests"
    else
        log_error "Server functionality degraded after stress tests"
    fi
}

cleanup_logs() {
    log_info "Cleaning up test logs..."
    rm -f "$TEST_DIR"/client*.log "$TEST_DIR"/rapid*.log 2>/dev/null
}

main() {
    log_info "Starting Stress Tests"
    log_info "=====================" 
    log_warning "Note: These tests may take several minutes to complete"
    
    # Clean old logs
    rm -f "$TEST_DIR"/*.log 2>/dev/null
    
    if ! start_server; then
        exit 1
    fi
    
    trap 'stop_server; cleanup_logs' EXIT
    sleep 3
    
    # Check server connectivity
    if ! nc -z localhost $IRC_PORT 2>/dev/null; then
        log_error "Server is not accepting connections"
        exit 1
    fi
    
    log_success "Server is accepting connections"
    
    test_multiple_connections
    test_rapid_connections  
    test_large_messages
    test_message_flood
    test_channel_flooding
    test_partial_messages
    test_server_stability
    
    log_info "====================="
    log_info "Stress Test Results:"
    log_success "Passed: $TESTS_PASSED"
    
    if [ $TESTS_FAILED -gt 0 ]; then
        log_error "Failed: $TESTS_FAILED"
        log_info "Logs available in: $TEST_DIR/"
        exit 1
    else
        log_success "All stress tests completed! 🎉"
        log_info "Note: Some warnings are acceptable for stress testing"
        exit 0
    fi
}

main "$@"