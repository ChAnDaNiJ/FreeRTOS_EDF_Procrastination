# EDF Scheduler with FreeRTOS Integration

## Overview

This project implements an **Earliest Deadline First (EDF) real-time scheduler**
with:

- Dynamic job scheduling
- Preemption support
- Procrastination scheduling
- Shutdown optimization
- Performance metrics
- FreeRTOS POSIX integration

The project began as a custom EDF simulator and was later extended to run on
the **FreeRTOS POSIX/Linux simulator**, preserving the original scheduling logic.

---

## Features

### EDF Scheduling
Jobs are scheduled according to earliest absolute deadline.

### Preemption
Running jobs are preempted when a higher-priority job arrives.

### Procrastination Scheduling
The scheduler intentionally delays execution when safe to create larger idle windows.

### Shutdown Optimization
If idle time exceeds threshold, system enters shutdown state to simulate energy savings.

### Metrics
The scheduler computes:

- Context switches
- Voluntary / involuntary switches
- Preemptions
- Cache impact
- CPU utilization
- Response time
- Waiting time
- Lateness

---

## Project Structure

```bash
RTS_FreeRTOS/
│
├── build/
├── freertos/
│
├── edf.c
├── edf.h
├── task.c
├── task.h
├── pq.c
├── pq.h
├── metrics.c
├── metrics.h
├── procrastination.c
├── procrastination.h
├── FreeRTOSConfig.h
├── freertos_runner.c
├── main.c
└── file1.txt
```

---

## Scheduling Flow

1. Read task set
2. Compute hyperperiod
3. Release jobs periodically
4. Schedule using EDF
5. Handle preemption
6. Detect idle intervals
7. Apply procrastination algorithm
8. Enter shutdown if threshold satisfied
9. Generate metrics

---

## Compilation

From build directory:

```bash
gcc -I../freertos/include \
-I../freertos/portable/ThirdParty/GCC/Posix \
-I../freertos/portable/ThirdParty/GCC/Posix/utils \
-I.. \
../*.c \
../freertos/*.c \
../freertos/portable/MemMang/heap_4.c \
../freertos/portable/ThirdParty/GCC/Posix/*.c \
../freertos/portable/ThirdParty/GCC/Posix/utils/*.c \
-pthread -lm \
-o freertos_edf
```

---

## Run

```bash
./freertos_edf
```

---

## Example Output

```text
FreeRTOS started EDF Scheduler

T1 J1 released at 0
T1 J1 DONE at 2

SHUTDOWN 12 -> 28

Preemption point at 70
```

---

## Technologies Used

- C
- FreeRTOS
- POSIX/Linux
- Priority Queue Scheduling
- Real-Time Systems

---

## Future Improvements

- Rate Monotonic scheduling
- GUI visualization
- Power consumption estimation
- Multi-core scheduling

---

## Author

Chandani
BITS Pilani Goa Campus
Real-Time Systems Project
