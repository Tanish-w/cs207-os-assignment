

# Part F: File-System and OS Case-Study Analysis

**Author:** Yashasvi Singh Bhardwaj (`@yashasvisb`)  
**Course:** CS207 — Operating Systems Design  
**Topic:** Linux and Windows Comparison — Process Management, File-System Organization, User Permissions, and Command-Line Tools

---

## 1. Introduction

Linux and Windows are widely used operating systems that provide process management, file-system organization, user access control and command-line interfaces. Both support multitasking and secure file management, but they differ in their file-system structures, permission mechanisms and system administration tools.

This section compares these features and supports the discussion with observations and commands executed in the Ubuntu virtual machine.

---

## 2. Linux and Windows Comparison

### 2.1 Process Management

Linux manages running programs as processes, each identified by a unique Process ID (PID). The `ps` command displays process information, while `top` provides a live view of running processes and resource utilization. The `kill` command can send signals to processes.

Windows also assigns unique PIDs to processes. Running processes can be monitored through Task Manager or PowerShell commands such as `Get-Process`. Both operating systems support multitasking and use CPU scheduling to manage process execution.

**Ubuntu Observation:**

The following commands were executed in the Ubuntu VM:

```bash
ps -p $$ -o pid,ppid,comm
ps -e
top
```

These commands displayed the current shell's PID and parent PID, the list of running processes, and system resource information.

### 2.2 File-System Organization

Linux follows a single hierarchical directory structure beginning at the root directory (`/`). Directories such as `/home`, `/etc`, `/usr` and `/var` are organized under this root. Storage devices and partitions are made accessible by mounting them at locations within this hierarchy.

Windows generally organizes storage using drive letters such as `C:` and `D:`, with a separate directory structure for each drive. NTFS is a commonly used Windows file system, while Linux supports file systems such as ext4.

**Ubuntu Observation:**

The following commands were executed:

```bash
pwd
ls /
df -h
```

The `pwd` command displayed the working directory as `/home/yashasvi/cs207-os-assignment`. The `ls /` command displayed the contents of the root directory, while `df -h` showed mounted file systems and their disk-space usage.

### 2.3 User Permissions and Protection

Linux uses a user-and-group-based permission model. Files and directories have an owner and an associated group, with read (`r`), write (`w`) and execute (`x`) permissions assigned to the owner, group and others. Commands such as `chmod` modify permissions, `chown` changes ownership, and `sudo` allows authorized administrative operations.

Windows uses user accounts and Access Control Lists (ACLs) to specify which users and groups can access files and directories. User Account Control (UAC) helps restrict unauthorized administrative operations.

**Ubuntu Observation:**

The following commands were executed:

```bash
id
ls -l README.md
stat README.md
```

The `id` command displayed the current user's UID, GID and group memberships. The `ls -l` and `stat` commands displayed file permissions, ownership and metadata.

For example, the `README.md` file had permissions `0664` (`-rw-rw-r--`), meaning the owner and group had read and write permissions, while other users had read-only permission.

### 2.4 Command-Line Tools

Linux commonly uses the Bash shell and command-line utilities for system administration and file management. Commands such as `pwd`, `ls`, `cd`, `cp`, `mv` and `grep` are used to navigate directories, manage files and search text.

Windows provides Command Prompt and PowerShell. Common commands include `dir`, `cd`, `copy` and `move`, while PowerShell also provides commands such as `Get-Process` for process management.

**Ubuntu Observation:**

The Ubuntu VM used the Bash shell (`/bin/bash`), with GNU Bash version 5.2.21 and GNU grep version 3.11. Commands such as `pwd`, `ls` and `grep` were executed successfully.

---

## 3. Conclusion

Linux and Windows provide essential operating-system functions, including process management, file organization, access control and command-line administration. Their primary differences lie in their file-system structures, permission mechanisms and command-line environments.

The Ubuntu VM observations demonstrated Linux process inspection, hierarchical file organization, user and group permissions, and the use of Bash utilities. These observations supported the comparison with Windows and provided practical understanding of operating-system administration.

