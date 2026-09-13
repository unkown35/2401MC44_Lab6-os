# Operating System Lab Assignment 6

**Name:** Sai Vignan  
**Roll Number:** 2401MC44

## Overview

This assignment demonstrates four important operating-system techniques related to deadlocks: deadlock avoidance using Banker's Algorithm, deadlock detection using a wait-for graph, deadlock prevention through resource ordering, and synchronization with multiple shared resources.

The implementation is organized into the following programs:

- `tests/bankers.c` — Banker's Algorithm simulation
- `tests/deadlockdetect.c` — Deadlock detection using a wait-for graph
- `tests/resourceorder.c` — Deadlock prevention using resource ordering
- `tests/syncdeadlock.c` — Combined synchronization and deadlock avoidance

The `diffs` directory contains the corresponding xv6 changes and patches used by the programs.

---

## Question 1 — Banker's Algorithm Simulation

### Implementation

`tests/bankers.c` models **5 processes** and **3 resource types** using the standard Banker's Algorithm data structures:

- `Allocation` — resources currently allocated to each process.
- `Max` — maximum resource requirement of each process.
- `Available` — currently available instances of each resource type.
- `Need` — calculated at runtime using:

```text
Need = Max - Allocation
```

### Safety Algorithm

The safety algorithm checks whether the current state is safe. A temporary `Work` vector is initialized from `Available`. A process can finish when all of its remaining `Need` can be satisfied by `Work`. Once it finishes, its allocated resources are returned to `Work`.

If all processes can finish, the system is safe and the program prints a safe sequence. Otherwise, the state is unsafe.

### Resource Request Algorithm

A resource request from a process is checked against both its remaining `Need` and the currently available resources. If the request can be considered, the allocation is temporarily updated and the safety algorithm is executed again.

- If the resulting state is safe, the request is granted.
- If the resulting state is unsafe, the tentative allocation is rolled back and the request is denied.

The program contains test scenarios demonstrating safe and rejected requests.

---

## Question 2 — Deadlock Detection Using Resource Allocation Graph

### Implementation

`tests/deadlockdetect.c` represents processes and resource instances using allocation and request information.

A **wait-for graph** is constructed from these matrices. An edge

```text
Pi -> Pj
```

means that process `Pi` is waiting for a resource currently held by process `Pj`.

### Detection

A DFS-based cycle detection algorithm is used on the wait-for graph. A cycle indicates that the processes involved are in a deadlock.

The program tests two cases:

1. An acyclic graph where no deadlock exists.
2. A cyclic graph containing a circular wait among three processes.

When a cycle is found, the program prints the processes involved in the cycle.

---

## Question 3 — Deadlock Prevention via Resource Ordering

### Implementation

`tests/resourceorder.c` demonstrates deadlock caused by inconsistent resource acquisition order and then demonstrates the prevention technique.

### Bad ordering

In the bad configuration:

- Process A acquires `Lock1` and then attempts to acquire `Lock2`.
- Process B acquires `Lock2` and then attempts to acquire `Lock1`.

With a suitable interleaving, this creates a circular wait:

```text
Process A -> Lock2
Process B -> Lock1
```

and the processes can remain blocked.

### Fixed ordering

The fixed configuration imposes one global ordering:

```text
Lock1 -> Lock2
```

Both processes acquire the locks in this same order. Therefore, a circular wait cannot be formed and the workload completes.

The program prints step markers showing lock acquisition, waiting, and release so that the behavior can be observed in the output.

---

## Question 4 — Combined Synchronization and Deadlock Avoidance

### Implementation

`tests/syncdeadlock.c` models **5 processes** and **3 shared resource types**:

- Printer
- Scanner
- Disk

Each resource type has a limited number of instances, and each process needs two resource types to complete its work.

The program uses shared state to coordinate access to the resource pools and applies **resource ordering** as the deadlock-prevention strategy.

For every process, the two required resources are ordered according to their global resource number before acquisition. Thus, a process always obtains its required resources in the same global order.

### Why deadlock is prevented

Deadlock requires a circular-wait condition. Because all processes follow the same resource ordering, a process cannot hold a higher-ordered resource while waiting for a lower-ordered resource. Consequently, a circular wait cannot form.

Each process:

1. Requests its resources.
2. Acquires them according to the global ordering.
3. Performs its simulated work.
4. Releases the resources.
5. Repeats for multiple cycles.

The output demonstrates that all five processes complete their work without permanently blocking and that resource usage remains within the available limits.

---

## Files and Outputs

### Source programs

```text
 tests/
 ├── bankers.c
 ├── deadlockdetect.c
 ├── resourceorder.c
 └── syncdeadlock.c
```

### xv6 patches

```text
 diffs/
 ├── kernel_changes.diff
 ├── q1_bankers.diff
 ├── q2_deadlockdetect.diff
 ├── q3_resourceorder.diff
 └── q4_syncdeadlock.diff
```

### Output screenshots

The `screenshots` directory contains the recorded outputs for the four experiments, including the Banker's Algorithm safe/request tests, deadlock detection results, resource-ordering demonstration, and multi-resource synchronization runs.

---

## How to Build and Run

Apply the required xv6 changes from the `diffs` directory and place the test programs in the appropriate xv6 user-program location. Then build and run xv6 using:

```bash
make clean
make qemu
```

Run each corresponding user program from the xv6 shell to observe its output.

---

## Conclusion

The assignment demonstrates the three major approaches to handling deadlocks:

- **Avoidance:** Banker's Algorithm checks whether granting a request keeps the system in a safe state.
- **Detection:** A wait-for graph is examined for cycles to identify deadlocks that have already occurred.
- **Prevention:** A fixed resource ordering prevents circular wait from forming in the first place.

The final synchronization example combines shared-resource coordination with a deadlock-prevention strategy so that multiple processes can repeatedly perform work without deadlock.
