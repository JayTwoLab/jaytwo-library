#!/usr/bin/env bash
#
# Script to print major Linux TCP kernel parameters + descriptions
#  - Prints each sysctl value followed by its description

print_param_with_desc() {
    local label="$1"
    local key="$2"
    local desc="$3"

    if sysctl_output=$(sysctl "$key" 2>/dev/null); then
        printf "%-30s : %s\n" "$label" "$sysctl_output"
    else
        printf "%-30s : %s\n" "$label" "Query failed ($key)"
    fi
    printf "  Description: %s\n\n" "$desc"
}

echo "========================================"
echo " Check TCP Kernel Parameter Status"
echo "========================================"
echo

# 1) tcp_fin_timeout
print_param_with_desc \
"tcp_fin_timeout" \
"net.ipv4.tcp_fin_timeout" \
"Time (seconds) to keep TIME_WAIT after FIN-based close. Default is 60 seconds. Prevents delay in socket resource reclamation."

# 2) tcp_tw_reuse
print_param_with_desc \
"tcp_tw_reuse" \
"net.ipv4.tcp_tw_reuse" \
"Whether to reuse TIME_WAIT sockets. Value 2 in recent kernels means safe reuse for outbound connections."

# 3) tcp_max_tw_buckets
print_param_with_desc \
"tcp_max_tw_buckets" \
"net.ipv4.tcp_max_tw_buckets" \
"Maximum number of TIME_WAIT sockets. If exceeded, the kernel forcibly cleans up old TIME_WAITs to prevent resource exhaustion."

# 4) tcp_max_syn_backlog
print_param_with_desc \
"tcp_max_syn_backlog" \
"net.ipv4.tcp_max_syn_backlog" \
"Size of the queue for initial SYN requests. Larger values improve concurrent connection handling."

# 5) somaxconn
print_param_with_desc \
"somaxconn" \
"net.core.somaxconn" \
"Maximum backlog for listen() function. Directly affects server concurrent connection capacity."

# 6) ip_local_port_range
print_param_with_desc \
"ip_local_port_range" \
"net.ipv4.ip_local_port_range" \
"Ephemeral port range for client outbound connections. Directly related to port exhaustion."

echo "========================================"
echo " Check Current Number of TIME_WAIT Sockets"
echo "========================================"
echo

# Check number of TIME_WAIT sockets
timewait_count="N/A"

if command -v ss >/dev/null 2>&1; then
    timewait_count=$(ss -tan 2>/dev/null | grep TIME-WAIT | wc -l)
    echo "Current number of TIME_WAIT sockets (ss): $timewait_count"
elif command -v netstat >/dev/null 2>&1; then
    timewait_count=$(netstat -ant 2>/dev/null | grep TIME_WAIT | wc -l)
    echo "Current number of TIME_WAIT sockets (netstat): $timewait_count"
else
    echo "ss/netstat not found → Unable to check TIME_WAIT count"
fi

echo
echo "========================================"
echo " Note:"
echo "  - This script only queries values."
echo "  - To change settings, edit /etc/sysctl.conf or"
echo "    /etc/sysctl.d/*.conf and apply with 'sysctl -p'."
echo "========================================"
