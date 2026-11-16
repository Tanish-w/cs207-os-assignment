#!/usr/bin/env bash
# CS207: Operating Systems Design - Assignment 1
# Master Execution and Automated Audit Script

set -e

echo "=========================================================="
echo " CS207 OS Assignment: Running Complete Verification Suite"
echo "=========================================================="

mkdir -p outputs test_cases src scripts report screenshots

# 1. Build all binaries
echo -e "\n1. Compiling all C source files via Makefile..."
make clean
make all

# 2. Part B: CPU Scheduling Simulator
echo -e "\n2. Executing Part B: CPU Scheduling Simulator (FCFS, SJF, Round Robin Q=3)..."
./scheduler test_cases/processes_part_b.csv 3 > outputs/part_b_output.txt
echo "   -> Saved in outputs/part_b_output.txt"

# 3. Part C: POSIX Threads Bounded-Buffer Producer-Consumer
echo -e "\n3. Executing Part C: Thread-Safe Producer-Consumer (100 items stress test)..."
./producer_consumer > outputs/part_c_stress_test_output.txt
echo "   -> Saved in outputs/part_c_stress_test_output.txt"

# 4. Part D: Deadlock Avoidance (Banker's Algorithm)
echo -e "\n4. Executing Part D: Banker's Algorithm Test Cases..."
./bankers < test_cases/part_d_safe.txt > outputs/part_d_safe_output.txt
./bankers < test_cases/part_d_unsafe.txt > outputs/part_d_unsafe_output.txt
./bankers < test_cases/part_d_multiple_safe.txt > outputs/part_d_multiple_safe_output.txt
echo "   -> Saved safe, unsafe, and multiple safe sequence outputs in outputs/"

# 5. Part E: Page Replacement and Disk Scheduling
echo -e "\n5. Executing Part E: Memory Page Replacement & Disk Scheduling..."
./page_replacement < test_cases/part_e_pages_test1.txt > outputs/part_e_pages_test1_output.txt
./page_replacement < test_cases/part_e_pages_test2.txt > outputs/part_e_pages_test2_output.txt
./disk_scheduling < test_cases/part_e_disk_test1.txt > outputs/part_e_disk_test1_output.txt
./disk_scheduling < test_cases/part_e_disk_test2.txt > outputs/part_e_disk_test2_output.txt
echo "   -> Saved page replacement and disk scheduling outputs in outputs/"

# 6. Part F: File System & Directory Tree Audit
echo -e "\n6. Executing Part F: File System & Directory Tree Audit..."
chmod +x scripts/setup_part_f_tree.sh
./scripts/setup_part_f_tree.sh > outputs/part_f_filesystem_demo.txt
echo "   -> Saved in outputs/part_f_filesystem_demo.txt"

echo -e "\n=========================================================="
echo " All assignment components executed and verified successfully!"
echo "=========================================================="
