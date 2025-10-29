#!/usr/bin/env bash
set -e

# === Configuration ===
INTERFACE="enp60s0"
IP_ADDR="192.168.88.50/24"
COMPOSE_FILE="docker-compose.yaml"

# === Helper functions ===
function log_info() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

function log_ok() {
    echo -e "\033[1;32m[OK]\033[0m $1"
}

function log_warn() {
    echo -e "\033[1;33m[WARN]\033[0m $1"
}

# === Step 1: Check if IP already exists ===
if ip addr show "$INTERFACE" | grep -q "${IP_ADDR%%/*}"; then
    log_ok "IP ${IP_ADDR%%/*} already assigned to ${INTERFACE}"
else
    log_info "Adding IP ${IP_ADDR} to ${INTERFACE}..."
    sudo ip addr add "$IP_ADDR" dev "$INTERFACE"
    log_ok "IP address added successfully."
fi

# === Step 2: Verify interface state ===
if ! ip link show "$INTERFACE" | grep -q "state UP"; then
    log_warn "Interface ${INTERFACE} is DOWN — attempting to bring it UP..."
    sudo ip link set "$INTERFACE" up
    log_ok "Interface ${INTERFACE} is now UP."
fi

# === Step 3: Allow X11 GUI access ===
if command -v xhost >/dev/null 2>&1; then
    log_info "Granting X11 access to Docker..."
    xhost +local:root >/dev/null
    log_ok "X11 access granted."
else
    log_warn "xhost not found — skipping GUI permission setup."
fi

# === Step 4: Start Docker container ===
if [ -f "$COMPOSE_FILE" ]; then
    log_info "Starting Docker container from ${COMPOSE_FILE}..."
    docker compose up -d
    log_ok "Docker container is running."
else
    log_warn "No docker-compose.yaml found in $(pwd). Please check your path."
fi

# === Step 5: Done ===
log_ok "Setup complete. You can now connect to the container with:"
echo "  docker exec -it livox_ros2 bash"
