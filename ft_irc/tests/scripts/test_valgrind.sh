#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

IRC_PORT=6672
IRC_PASSWORD="testpass123"
VALGRIND_LOG="tests/valgrind_logs/valgrind_output.log"
TEST_DIR="tests/valgrind_logs"
TESTS_PASSED=0
TESTS_FAILED=0

# Create test directory
mkdir -p "$TEST_DIR"

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[PASS]${NC} $1"; ((TESTS_PASSED++)); }
log_error() { echo -e "${RED}[FAIL]${NC} $1"; ((TESTS_FAILED++)); }
log_warning() { echo -e "${YELLOW}[WARN]${NC} $1"; }

check_valgrind() {
    if ! command -v valgrind &> /dev/null; then
        log_error "Valgrind is not installed. Skipping memory leak tests."
        exit 0
    fi
}

start_server_with_valgrind() {
    log_info "Starting IRC server with Valgrind..."
    log_warning "This will be slower than normal execution"
    
    valgrind \
        --tool=memcheck \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --verbose \
        --log-file=$VALGRIND_LOG \
        ./ircserv $IRC_PORT $IRC_PASSWORD &
    
    SERVER_PID=$!
    
    # Wait longer for valgrind startup
    log_info "Waiting for server to start under Valgrind..."
    sleep 15
    
    # Check if server is responsive
    local wait_count=0
    while ! nc -z localhost $IRC_PORT 2>/dev/null && [ $wait_count -lt 10 ]; do
        sleep 2
        ((wait_count++))
        log_info "Still waiting for server... ($wait_count/10)"
    done
    
    if nc -z localhost $IRC_PORT 2>/dev/null; then
        log_success "Server is running under Valgrind"
        return 0
    else
        log_error "Server failed to start under Valgrind"
        return 1
    fi
}

stop_server() {
    if [ ! -z "$SERVER_PID" ]; then
        log_info "Stopping server and waiting for Valgrind to finish..."
        kill -TERM $SERVER_PID 2>/dev/null
        
        # Wait for graceful shutdown
        sleep 10
        
        # Force kill if still running
        if kill -0 $SERVER_PID 2>/dev/null; then
            log_warning "Server still running, forcing shutdown..."
            kill -KILL $SERVER_PID 2>/dev/null
        fi
        
        wait $SERVER_PID 2>/dev/null
        log_info "Server stopped"
    fi
}

# Simple IRC session for valgrind testing
send_simple_session() {
    local session_name="$1"
    local timeout="${2:-8}"
    shift 2
    local commands=("$@")
    
    log_info "Running memory test session: $session_name"
    
    # Show what we're sending (abbreviated for valgrind tests)
    log_info ">>> Commands: ${#commands[@]} IRC commands"
    
    (
        for cmd in "${commands[@]}"; do
            printf "%s\r\n" "$cmd"
            sleep 0.5  # Slower for valgrind
        done
        sleep $timeout
    ) | nc -w $(($timeout + 3)) localhost $IRC_PORT > "$TEST_DIR/${session_name}.log" 2>&1
    
    # Brief response check
    if [ -f "$TEST_DIR/${session_name}.log" ] && [ -s "$TEST_DIR/${session_name}.log" ]; then
        log_info "<<< Received response ($(wc -l < "$TEST_DIR/${session_name}.log") lines)"
    else
        log_warning "<<< No response received"
    fi
    
    return $?
}

run_basic_operations() {
    log_info "Running basic operations for memory leak testing..."
    
    # Test 1: Basic connection and disconnection
    send_simple_session "memtest1" 6 \
        "PASS $IRC_PASSWORD" \
        "NICK memtest1" \
        "USER memtest1 0 * :Memory Test 1" \
        "QUIT :Memory test complete"
    
    sleep 3
    
    # Test 2: Channel operations
    send_simple_session "memtest2" 8 \
        "PASS $IRC_PASSWORD" \
        "NICK memtest2" \
        "USER memtest2 0 * :Memory Test 2" \
        "JOIN #memtest" \
        "LIST" \
        "PRIVMSG #memtest :Testing memory usage" \
        "PART #memtest :Leaving" \
        "QUIT :Memory test complete"
    
    sleep 3
    
    # Test 3: Multiple channels and users
    send_simple_session "memtest3" 10 \
        "PASS $IRC_PASSWORD" \
        "NICK memtest3" \
        "USER memtest3 0 * :Memory Test 3" \
        "JOIN #mem1" \
        "JOIN #mem2" \
        "JOIN #mem3" \
        "LIST" \
        "PRIVMSG #mem1 :Message 1" \
        "PRIVMSG #mem2 :Message 2" \
        "PRIVMSG #mem3 :Message 3" \
        "PART #mem1" \
        "PART #mem2" \
        "PART #mem3" \
        "QUIT :Memory test complete"
    
    sleep 5
    
    log_info "Basic operations completed"
}

analyze_valgrind_output() {
    log_info "Analyzing Valgrind output..."
    
    if [ ! -f "$VALGRIND_LOG" ]; then
        log_error "Valgrind log file not found: $VALGRIND_LOG"
        return 1
    fi
    
    # Check for memory leaks
    local definitely_lost=$(grep "definitely lost:" "$VALGRIND_LOG" | tail -1 | awk '{print $4}' | tr -d ',')
    local indirectly_lost=$(grep "indirectly lost:" "$VALGRIND_LOG" | tail -1 | awk '{print $4}' | tr -d ',')
    local possibly_lost=$(grep "possibly lost:" "$VALGRIND_LOG" | tail -1 | awk '{print $4}' | tr -d ',')
    
    # Check for errors
    local error_summary=$(grep "ERROR SUMMARY:" "$VALGRIND_LOG" | tail -1)
    local num_errors=$(echo "$error_summary" | awk '{print $4}')
    
    log_info "Memory Leak Analysis:"
    log_info "====================="
    
    if [ ! -z "$definitely_lost" ] && [ "$definitely_lost" != "0" ]; then
        log_error "Definitely lost: $definitely_lost bytes"
    else
        log_success "No definite memory leaks detected"
    fi
    
    if [ ! -z "$indirectly_lost" ] && [ "$indirectly_lost" != "0" ]; then
        log_warning "Indirectly lost: $indirectly_lost bytes"
    else
        log_success "No indirect memory leaks detected"
    fi
    
    if [ ! -z "$possibly_lost" ] && [ "$possibly_lost" != "0" ]; then
        log_warning "Possibly lost: $possibly_lost bytes"
    else
        log_success "No possible memory leaks detected"
    fi
    
    log_info "Error Analysis:"
    log_info "==============="
    
    if [ ! -z "$num_errors" ] && [ "$num_errors" != "0" ]; then
        log_error "Valgrind detected $num_errors error(s)"
        
        # Show specific errors (limited for readability)
        log_info "Error details (first 10 lines):"
        grep -A 5 "Invalid read\|Invalid write\|Use of uninitialised\|Invalid free" "$VALGRIND_LOG" | head -20 | sed 's/^/    /'
    else
        log_success "No memory errors detected"
    fi
    
    # Check for heap summary
    log_info "Heap Summary:"
    log_info "============="
    grep -A 5 "HEAP SUMMARY:" "$VALGRIND_LOG" | tail -10 | sed 's/^/    /'
    
    # Overall assessment
    if [ "$definitely_lost" = "0" ] && [ "$num_errors" = "0" ]; then
        log_success "✅ Memory management looks good!"
        return 0
    elif [ "$definitely_lost" != "0" ] || [ "$num_errors" != "0" ]; then
        log_error "❌ Memory issues detected - check $VALGRIND_LOG for details"
        return 1
    else
        log_warning "⚠️  Partial analysis - check $VALGRIND_LOG manually"
        return 0
    fi
}

show_valgrind_summary() {
    log_info "Valgrind Summary (last 30 lines):"
    log_info "=================================="
    if [ -f "$VALGRIND_LOG" ]; then
        tail -30 "$VALGRIND_LOG" | sed 's/^/    /'
    else
        log_warning "Valgrind log not found"
    fi
}

cleanup() {
    stop_server
    rm -f "$TEST_DIR"/memtest*.log 2>/dev/null
    log_info "Cleanup completed. Valgrind log saved as: $VALGRIND_LOG"
}

main() {
    log_info "Starting Memory Leak Tests with Valgrind"
    log_info "========================================"
    log_warning "This test will take significantly longer than normal tests"
    
    # Clean old logs
    rm -f "$TEST_DIR"/*.log 2>/dev/null
    
    check_valgrind
    
    if ! start_server_with_valgrind; then
        log_error "Cannot start server with Valgrind"
        exit 1
    fi
    
    trap cleanup EXIT
    
    # Run operations that test memory management
    run_basic_operations
    
    # Stop server gracefully to get final valgrind report
    log_info "Shutting down server to complete Valgrind analysis..."
    stop_server
    
    # Wait for valgrind to finish writing
    sleep 5
    
    # Analyze results
    if analyze_valgrind_output; then
        log_success "Memory leak analysis completed successfully"
    else
        log_error "Memory issues detected"
        show_valgrind_summary
        exit 1
    fi
    
    log_info "========================================"
    log_info "Memory Test Results:"
    log_success "Analysis completed: $TESTS_PASSED checks"
    
    if [ $TESTS_FAILED -gt 0 ]; then
        log_error "Memory issues found: $TESTS_FAILED"
        log_info "Full Valgrind log available at: $VALGRIND_LOG"
        exit 1
    else
        log_success "Memory management tests passed! 🎉"
        log_info "Full Valgrind log available at: $VALGRIND_LOG"
        exit 0
    fi
}

main "$@"