# Part C: Concurrency Theory & Unsafe Design Analysis

**Author:** Tanish Kumar (`@Tanish-w`)  
**Course:** CS207 — Operating Systems Design  
**Topic:** POSIX Threads, Synchronization Primitives, and Bounded-Buffer Concurrency  

---

## 1. The Critical Section Problem

A **Critical Section** is a code segment that accesses shared mutable state (such as memory buffers, table indices, or global variables) where concurrent execution by multiple threads can lead to unpredictable or erroneous behavior.

In the classic Bounded-Buffer Producer–Consumer architecture, the shared critical resources include:
1. The circular storage array: `buffer[BUFFER_SIZE]`.
2. The buffer insertion pointer: `in = (in + 1) % BUFFER_SIZE`.
3. The buffer extraction pointer: `out = (out + 1) % BUFFER_SIZE`.
4. The buffer occupancy counter: `count`.

### The Three Necessary Criteria for a Valid Solution
Any valid protocol addressing the critical section problem must satisfy three fundamental properties:
1. **Mutual Exclusion:** At most one thread may execute within its critical section at any given point in time.
2. **Progress:** If no thread is executing in its critical section and one or more threads wish to enter, the selection of the next thread cannot be postponed indefinitely, and only threads not in their remainder section may participate in this decision.
3. **Bounded Waiting:** A bound must exist on the number of times other threads are granted entry into their critical sections after a thread has requested entry, guaranteeing that no thread experiences starvation.

---

## 2. Anatomy of a Race Condition

A **Race Condition** is a timing-dependent defect where the output or state of a multi-threaded system depends on the execution sequence or scheduling of concurrent threads.

### Assembly-Level Breakdown of Non-Atomic Operations
Even a single high-level C statement such as `count++` or `count--` is **not atomic**. On modern processor architectures, it compiles into three distinct machine instructions:

```assembly
; Thread 1: Producer (count++)        ; Thread 2: Consumer (count--)
MOV EAX, [count]                      MOV EBX, [count]
ADD EAX, 1                            SUB EBX, 1
MOV [count], EAX                      MOV [count], EBX
```

If preemptive thread scheduling interrupts execution between these instructions:
1. **Initial state:** `count = 3`.
2. **Producer** executes `MOV EAX, [count]` ($EAX = 3$) and `ADD EAX, 1` ($EAX = 4$). *(Preempted)*
3. **Consumer** is scheduled, loads `EBX = 3`, decrements `SUB EBX, 1` ($EBX = 2$), and writes `count = 2`.
4. **Producer** resumes and executes its pending store: `MOV [count], EAX` (writes $4$).
5. **Observed Failure:** One item was produced and one was consumed, yet `count` ends at $4$ instead of $3$. A state corruption has occurred due to a lost update.

---

## 3. Role of POSIX Synchronization Primitives

To coordinate concurrent producers and consumers without inefficient CPU busy-waiting (spinning), three standard POSIX primitives are deployed:

| Primitive | Type | Initial Value | Architectural Role |
| :--- | :--- | :---: | :--- |
| `pthread_mutex_t mutex` | Binary Mutex | `PTHREAD_MUTEX_INITIALIZER` | Enforces **Mutual Exclusion** over the buffer indices (`in`, `out`) and counter (`count`), guaranteeing atomicity. |
| `sem_t empty` | Counting Semaphore | `BUFFER_SIZE` (5) | Tracks available **empty slots**. Blocks producers via `sem_wait(&empty)` when the buffer is full. |
| `sem_t full` | Counting Semaphore | `0` | Tracks available **filled slots**. Blocks consumers via `sem_wait(&full)` when the buffer is empty. |

### Lock Acquisition Ordering & Deadlock Prevention
The synchronization sequence must follow a strict two-phase protocol:

```c
// PRODUCER PROTOCOL                       // CONSUMER PROTOCOL
sem_wait(&empty);                           sem_wait(&full);
pthread_mutex_lock(&mutex);                 pthread_mutex_lock(&mutex);

/* CRITICAL SECTION */                      /* CRITICAL SECTION */
buffer[in] = item;                         item = buffer[out];
in = (in + 1) % BUFFER_SIZE;               out = (out + 1) % BUFFER_SIZE;
count++;                                   count--;

pthread_mutex_unlock(&mutex);               pthread_mutex_unlock(&mutex);
sem_post(&full);                            sem_post(&empty);
```

> **Deadlock Pitfall:** If the locking order is inverted (`pthread_mutex_lock` called *before* `sem_wait`), a producer could acquire the mutex and subsequently block inside `sem_wait(&empty)` on a full buffer. Because it holds the mutex, no consumer can ever enter the critical section to free a slot, producing an unrecoverable **Deadlock**.

---

## 4. Deliberately Flawed / Unsafe Design Analysis

To examine what happens when synchronization is omitted, consider the following flawed pseudocode where neither mutexes nor semaphores are used, relying solely on naive software checks:

### Flawed / Unsafe Pseudocode:
```c
// UNSAFE PRODUCER (NO MUTEX, NO SEMAPHORE)
void* unsafe_producer(void* arg) {
    while (has_items_to_produce()) {
        while (count >= BUFFER_SIZE) { /* spin / busy-wait */ }
        
        // RACE WINDOW: Multiple producers read same 'in'
        int current_in = in;
        buffer[current_in] = create_item();
        in = (current_in + 1) % BUFFER_SIZE;
        count++; // NON-ATOMIC INCREMENT
    }
}

// UNSAFE CONSUMER (NO MUTEX, NO SEMAPHORE)
void* unsafe_consumer(void* arg) {
    while (has_items_to_consume()) {
        while (count <= 0) { /* spin / busy-wait */ }
        
        // RACE WINDOW: Multiple consumers read same 'out'
        int current_out = out;
        item = buffer[current_out];
        out = (current_out + 1) % BUFFER_SIZE;
        count--; // NON-ATOMIC DECREMENT
    }
}
```

### Analysis of Resulting Failure Modes:

1. **Lost Updates & Silent Overwrites (Missing Items):**
   - Two producer threads simultaneously observe `count = 2` and read `in = 1`.
   - Producer 1 writes its item to `buffer[1]`.
   - Before Producer 1 updates `in`, Producer 2 also writes its item to `buffer[1]`.
   - **Result:** Producer 1's item is permanently obliterated without ever being consumed.

2. **Ghost Reads & Duplicated Processing:**
   - Two consumer threads concurrently read `out = 3`.
   - Both consumers extract the item from `buffer[3]` and process it independently.
   - **Result:** An item is processed twice, violating the required invariant that every produced item is consumed exactly once.

3. **Buffer Underflow / Overflow (`count < 0` or `count > BUFFER_SIZE`):**
   - When `count = 1`, two consumers evaluate `while (count <= 0)` simultaneously.
   - Both pass the condition and decrement `count`, causing `count` to drop to `0` and then `-1`.
   - A negative count causes consumers to attempt to read unallocated or stale memory slots.

---

## 5. Conclusion

This analysis proves that un-synchronized concurrent designs are fundamentally broken due to instruction-level race conditions on shared memory. Correct POSIX concurrency strictly requires:
1. **Counting Semaphores** to manage producer/consumer flow control and prevent buffer overflow/underflow.
2. **Mutex Locks** to enforce strict mutual exclusion over shared buffer manipulation.
