# CS207: Operating Systems Design — Assignment 1

**Date:** 19 November 2026  
**Authors:**  
- **Tanish Kumar** (Roll No: `24/EE/269`, [@Tanish-w](https://github.com/Tanish-w))  
- **Yashasvi Singh Bhardwaj** (Roll No: `24/EN/074`, [@yashasvisb](https://github.com/yashasvisb))  
**Environment:** Ubuntu 24.04 LTS (Oracle VirtualBox x86_64, 2 vCPUs, 4 GB RAM, 25 GB Disk)

---

## Work Division

| Member & Roll No. | Components |
| :--- | :--- |
| **Tanish Kumar**<br>`24/EE/269` | Repository setup, Makefile build system, Part A (VM setup & diagnostics), Part B (CPU scheduling simulator), Part C (concurrency analysis & race conditions), Part F (filesystem tree script & inode audit). |
| **Yashasvi Singh Bhardwaj**<br>`24/EN/074` | Part C (POSIX bounded-buffer implementation & thread synchronization), Part D (Banker's algorithm & safe sequence search), Part E (FIFO/LRU page replacement & FCFS/SSTF disk scheduling), Part F (Linux vs. Windows case study). |
| **Joint Work** | Report compilation & academic typesetting (PDF), master test harness (`scripts/run_all.sh`), end-to-end VM verification, and submission packaging. |

---

## Build Instructions

```bash
# Compile all programs
make all

# Clean compiled binaries
make clean
```

All source code compiles with `gcc -Wall -Wextra -std=c11` (and `-pthread` for Part C).

---

## Execution Guide

### Automated Verification
To compile all programs and execute all benchmarks in one step:
```bash
./scripts/run_all.sh
```

### Individual Execution

#### Part A — System Diagnostics
```bash
gcc --version && uname -a && lscpu && free -h && df -h
```
*Output reference:* `outputs/part_a_system_info.txt` | *Screenshot:* `screenshots/vm_setup_part_a.png`

#### Part B — CPU Scheduling Simulator (FCFS, SJF, Round Robin)
```bash
# Syntax: ./scheduler <process_file.csv> <round_robin_quantum>
./scheduler test_cases/processes_part_b.csv 3
```
*Output reference:* `outputs/part_b_output.txt` | *Screenshots:* `screenshots/part_b/`

#### Part C — POSIX Bounded-Buffer Producer–Consumer
```bash
./producer_consumer
```
*Output reference:* `outputs/part_c_stress_test_output.txt` | *Screenshots:* `screenshots/part_c/`

#### Part D — Banker's Algorithm (Deadlock Avoidance)
```bash
# Safe state (16 safe sequences)
./bankers < test_cases/part_d_safe.txt

# Unsafe state (diagnoses unsatisfied resource claims; unsafe != deadlock)
./bankers < test_cases/part_d_unsafe.txt

# Multiple safe sequences (all 6 permutations valid)
./bankers < test_cases/part_d_multiple_safe.txt
```
*Output references:* `outputs/part_d_*_output.txt` | *Screenshots:* `screenshots/part_d/`

#### Part E — Virtual Memory & Disk Arm Scheduling
```bash
# Page Replacement (FIFO & LRU frame traces)
./page_replacement < test_cases/part_e_pages_test1.txt
./page_replacement < test_cases/part_e_pages_test2.txt

# Disk Scheduling (FCFS & SSTF cylinder head movement)
./disk_scheduling < test_cases/part_e_disk_test1.txt
./disk_scheduling < test_cases/part_e_disk_test2.txt
```
*Output references:* `outputs/part_e_*_output.txt` | *Screenshots:* `screenshots/part_e/`

#### Part F — File-System Organization & Metadata
```bash
./scripts/setup_part_f_tree.sh
```
*Output references:* `outputs/part_f_filesystem_demo.txt`, `outputs/part_f_linux_observations.txt` | *Screenshots:* `screenshots/part_f/`

---

## Directory Structure

```text
.
├── Makefile                        # Builds all binaries
├── README.md                       # Project documentation
├── CS207_Assignment.pdf            # Assignment specification
├── src/                            # C source files
│   ├── scheduler.c                 # Part B: CPU scheduler
│   ├── producer_consumer_part_c.c  # Part C: Bounded buffer
│   ├── bankers_part_d.c            # Part D: Banker's algorithm
│   ├── page_replacement_part_e.c   # Part E: Page replacement
│   └── disk_scheduling_part_e.c    # Part E: Disk scheduling
├── test_cases/                     # Benchmark inputs
├── outputs/                        # Execution logs
├── screenshots/                    # VM and terminal screenshots
├── scripts/
│   ├── run_all.sh                  # Master execution script
│   └── setup_part_f_tree.sh        # Part F directory tree setup
└── report/
    └── OS_assignment_report.pdf    # Academic submission report (12 pages)
```
