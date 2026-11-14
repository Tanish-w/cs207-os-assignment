#!/usr/bin/env bash
# CS207: Operating Systems Design - Assignment 1
# Part F: Sample Directory Tree & File System Metadata Demonstration

set -e

DEMO_DIR="fs_demo"
rm -rf "$DEMO_DIR"
mkdir -p "$DEMO_DIR"

echo "=== CS207: PART F - FILE SYSTEM & DIRECTORY TREE DEMONSTRATION ==="

# 1. Create nested directory structure (3 nested levels)
mkdir -p "$DEMO_DIR/system/kernel"
mkdir -p "$DEMO_DIR/system/config"
mkdir -p "$DEMO_DIR/system/logs"
mkdir -p "$DEMO_DIR/user_data/docs"

# 2. Create at least 5 files with distinct permissions
cat << 'EOF' > "$DEMO_DIR/system/kernel/vmlinuz.txt"
Linux Kernel v6.8.0-generic image binary stub.
Operating system core scheduler and memory management subsystem.
EOF

cat << 'EOF' > "$DEMO_DIR/system/config/sysctl.conf"
# System kernel tuning parameters
net.ipv4.ip_forward = 0
vm.swappiness = 10
EOF

cat << 'EOF' > "$DEMO_DIR/system/config/fstab"
# /etc/fstab: static file system information
UUID=e12b7a89 / ext4 errors=remount-ro 0 1
UUID=90a1bc34 /home ext4 defaults 0 2
EOF

cat << 'EOF' > "$DEMO_DIR/system/logs/syslog.log"
Nov 04 14:00:01 ubuntu systemd[1]: Starting OS Assignment Daemon...
Nov 04 14:00:02 ubuntu kernel: [0.000000] Linux version 6.8 (x86_64)
Nov 04 14:00:05 ubuntu systemd[1]: Started OS Assignment Daemon.
EOF

cat << 'EOF' > "$DEMO_DIR/user_data/docs/assignment_notes.md"
# CS207 Operating Systems Design Notes
- Part A: VirtualBox and Ubuntu Environment
- Part B: CPU Scheduling Simulator (FCFS, SJF, RR)
- Part C: POSIX Bounded Buffer Concurrency
- Part D: Banker's Deadlock Avoidance
- Part E: Virtual Memory and Disk Arm Scheduling
- Part F: Linux vs Windows Architecture Comparison
EOF

# Set illustrative permissions
chmod 644 "$DEMO_DIR/system/kernel/vmlinuz.txt"
chmod 600 "$DEMO_DIR/system/config/sysctl.conf"    # Owner read/write only
chmod 644 "$DEMO_DIR/system/config/fstab"
chmod 640 "$DEMO_DIR/system/logs/syslog.log"       # Owner rw, group r
chmod 755 "$DEMO_DIR/user_data/docs/assignment_notes.md"

echo "1. Directory Tree Visualization:"
if command -v tree >/dev/null 2>&1; then
    tree -p -u -g "$DEMO_DIR"
else
    find "$DEMO_DIR" | sed -e "s/[^-][^\/]*\// |/g" -e "s/|\([^ ]\)/| - \1/"
fi

echo "2. Absolute vs Relative Path Demonstration:"
ABS_PATH=$(realpath "$DEMO_DIR/system/config/sysctl.conf")
REL_PATH="./$DEMO_DIR/system/config/sysctl.conf"
echo "Absolute Path: $ABS_PATH"
echo "Relative Path: $REL_PATH"
echo "Navigating: ls ../$(basename "$PWD")/$DEMO_DIR/system/kernel -> $(ls "../$(basename "$PWD")/$DEMO_DIR/system/kernel")"

echo "3. File Permissions and Ownership Audit (ls -la):"
ls -la "$DEMO_DIR/system/config"

echo "4. Permission Modification (Symbolic vs Octal):"
echo "Original:              $(ls -l "$DEMO_DIR/system/config/sysctl.conf" | awk '{print $1, $3, $4, $9}')"
chmod 644 "$DEMO_DIR/system/config/sysctl.conf"
echo "Octal (chmod 644):     $(ls -l "$DEMO_DIR/system/config/sysctl.conf" | awk '{print $1, $3, $4, $9}')"
chmod u+x,g-w "$DEMO_DIR/system/config/sysctl.conf"
echo "Symbolic (chmod u+x):  $(ls -l "$DEMO_DIR/system/config/sysctl.conf" | awk '{print $1, $3, $4, $9}')"

echo "5. Detailed Inode & File Metadata Inspection (stat):"
stat "$DEMO_DIR/system/kernel/vmlinuz.txt"

echo "=== Directory Tree Demonstration Completed Successfully ==="
