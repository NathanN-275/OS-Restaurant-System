# OS Restaurant System

A concurrent restaurant simulation written in C that demonstrates process creation, interprocess communication, synchronization, and resource cleanup using UNIX processes and a System V message queue.

## Overview

The program models a small restaurant in which the waiter, customers, and chef operate as independent processes. A shared message queue acts as the communication channel between those processes, allowing menus, orders, and completed orders to move through the restaurant workflow.

The current simulation runs with:

- 1 waiter process
- 3 customer/table processes
- 1 chef process
- A 10-second simulation period

Each process prints timestamped activity to the terminal so the order of concurrent events can be followed.

## How It Works

1. The parent process creates a System V message queue and launches the restaurant processes with `fork()`.
2. The waiter sends a menu message to an available customer process.
3. The customer receives the menu, randomly selects Pizza, Burger, or Pasta, and submits an order.
4. The waiter receives the order and places it back into the queue for the chef.
5. The chef receives the order, waits for a randomly generated cooking time, and sends an order-ready message.
6. The waiter receives the completed order, simulates delivery, and sends another menu so the cycle can continue.
7. When the simulation ends, the parent process terminates the child processes and removes the message queue.

## Process Architecture

```text
                         System V Message Queue
                    ┌────────────────────────────┐
                    │  MSG_menu                  │
                    │  MSG_order                 │
                    │  MSG_order_ready           │
                    └────────────────────────────┘
                         ▲         ▲         ▲
                         │         │         │
                    ┌────┴───┐ ┌───┴────┐ ┌──┴───┐
                    │ Waiter │ │Customers│ │ Chef │
                    └────────┘ └────────┘ └──────┘
```

Messages contain a numeric type and a text payload. Calls to `msgrcv()` block until the requested message type is available, providing synchronization between otherwise independent processes.

## Operating-System Concepts Demonstrated

- Process creation with `fork()`
- Interprocess communication with System V message queues
- Typed messages using `msgsnd()` and `msgrcv()`
- Blocking synchronization between concurrent processes
- Process IDs and signal-based termination
- Randomized service, cooking, and eating delays
- Millisecond event logging with `gettimeofday()`
- IPC resource cleanup with `msgctl(..., IPC_RMID, ...)`

## Build and Run

### Requirements

- A UNIX-like operating system with System V IPC support
- GCC or Clang

### Compile

```bash
gcc -std=c11 -Wall -Wextra Restaurant.c -o restaurant
```

### Run

```bash
./restaurant
```

The terminal output shows menus being delivered, orders being placed and prepared, and the message queue being removed when the simulation finishes.

> **Portability note:** The source currently calls `ftok("restaurant.c", 65)`. On a case-sensitive filesystem, change that filename to `Restaurant.c` so that it matches the source file in this repository.

## Repository Contents

```text
Restaurant.c   Main simulation and process-management implementation
README.md      Project overview and usage documentation
```

## Possible Future Improvements

- Accept the number of customers, chefs, orders, and runtime as command-line arguments
- Add multiple chefs and waiters
- Replace signal-based termination with coordinated graceful shutdown messages
- Check and report errors from every IPC and process-management system call
- Save the timestamped activity log to a file
- Add automated tests for message flow and cleanup behavior

## Coursework Attribution

This project was originally developed for **CIS 3207 at Temple University** during Fall 2025. The restaurant-simulation concept and project requirements were provided through the course; the C implementation and portfolio documentation in this repository represent my work.

The repository is shared as a portfolio example of operating-systems programming. Students completing similar coursework should follow their institution's academic-integrity policies and should not submit this implementation as their own.
