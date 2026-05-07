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
 T1 J1 released at 0  exec=2  deadline=10
 T2 J1 released at 0  exec=2  deadline=20
 T3 J1 released at 0  exec=3  deadline=80
 T1 J1  DONE  at 2
 T2 J1  DONE  at 4
 T3 J1  DONE  at 7
 IDLE      7 -> 10  (duration=3)
 T1 J2 released at 10  exec=2  deadline=20
 T1 J2  DONE  at 12
 SHUTDOWN  12 -> 28  (duration=16)
 T1 J3 released at 20  exec=2  deadline=30
 T2 J2 released at 20  exec=3  deadline=40
 T1 J3  DONE  at 30
 T1 J4 released at 30  exec=1  deadline=40
 T1 J4  DONE  at 31
 T2 J2  DONE  at 34
 IDLE      34 -> 40  (duration=6)
 T1 J5 released at 40  exec=1  deadline=50
 T2 J3 released at 40  exec=3  deadline=60
 T1 J5  DONE  at 41
 T2 J3  DONE  at 44
 IDLE      44 -> 50  (duration=6)
 T1 J6 released at 50  exec=2  deadline=60
 T1 J6  DONE  at 52
 SHUTDOWN  52 -> 68  (duration=16)
 T1 J7 released at 60  exec=1  deadline=70
 T2 J4 released at 60  exec=3  deadline=80
 T1 J7  DONE  at 69
 T1 J8 released at 70  exec=1  deadline=80
 Preemption point at 70 (T2 J4 remaining=2)
 T1 J8  DONE  at 71
 T2 J4  DONE  at 73
 IDLE      73 -> 80  (duration=7)
 T1 J9 released at 80  exec=2  deadline=90
 T2 J5 released at 80  exec=3  deadline=100
 T3 J2 released at 80  exec=4  deadline=160

EDF execution completed
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
