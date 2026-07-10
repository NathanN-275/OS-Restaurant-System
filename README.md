# Project-3-F25
### This project is a simulation of a real-world process: a restaurant management system. The system involves:

- Waiter that delivers a menu to customers

- Customers that place orders at a random time and wait for and eat food

- Chefs that prepare orders one at a time

- Waiter that delivers completed food orders to customers.

- The system operates using a shared message queue that represents the kitchen and is where orders are placed and processed.

**The key features for this project are:**

Multiple Processes:

- The personnel of the restaurant operate as independent processes. This includes customers, chefs and waiters.

Message Queue:

- Orders are passed between the processes (the personnel) using IPC (interprocess communication) implemented through a System V message queue.

Concurrency:

- Multiple customers place orders concurrently.

- Multiple chefs work on different orders.

- A waiter delivers menus and orders to customers

**How the System Works**

1.         **Customers**:

- Arrive and wait for a menu from the waiter via the message queue

- Create orders and add them to the message queue.

- Each customer/table runs as a separate, independent process

2.         **Chefs**:

- Retrieve orders from the message queue, simulate food preparation, and send completed orders back to the queue.

3.         **Waiter**:

- Delivers a menu to a customer via the message queue

- Receives completed orders by retrieving them from the queue and after a delay (representing delivery of the order and customer completing the meal) sends a menu to the next customer.

4.         **Message Queue**:

- Acts as a shared resource for communication and synchronization among customer, chef, and waiter processes.

**Details**

Message queues are an IPC mechanism that we introduce in this project. A single message queue is used for communication among all processes. The message queue mechanism allows including a message ID (type) with the message. A process can request and read a message with a specified ID from the queue.

This means that a customer process makes an order by sending a message to the queue with an ID of “*order*”. The customer process then delays for a random period between 1 and 6 seconds. Note that unlike a real situation, each customer process will represent more than 1 customer over time. (It is more like a customer process represents a table that accommodates a customer during their meal).  After the delay (the customer completes their meal) , the customer process will loop back, wait for a menu and generate a new order (for a new customer).

A chef process requests to receive a message from the queue with an ID of “*order*”. When a chef has completed preparing the order, they send a message with an ID of “*order-complete*” to the message queue. The chef will then delay for a random time between 1 and 5 seconds. After the delay, the chef process will loop back to receive and process a new order.

The waiter requests a message with ID of “*order-complete*” from the queue and simulates the delivery of the order to the customer by invoking a delay of 3 seconds . The waiter then sends a message of “*menu*” to the message queue. This message will be received by a customer process waiting (with a new customer) to make an order.

**Customer Process**: each customer process handles new customers sequentially.

- After initialization, the process enters a loop:

- Performs message receive waiting for a message with ID  “*menu*” 

- Delays 1 second (deciding on the order)

- Sends a message with ID “*order*” to the message queue.

- Delays a random interval of 2-6 seconds (simulating the wait for food and eating)

- Returns to top of loop (new customer arrival, waiting for a menu)

**Chef Process**: 

- After initialization, the process enters a loop:

- Performs a message receive of a message with ID “*order*”

- Delays a random time of 1- 5 seconds (simulating the preparation of the food order)

- Sends a message with ID “*order-complete*” to the message queue.

- Returns to the top of the loop to get a new order to process.

**Waiter Process**:

- After initialization, sends a message with ID  “*menu*” to each of the customer processes, and then enters a loop:

- Performs a message receive request for a message of ID “*order-complete*”

- Waiter delays for 2 seconds

- Waiter sends a message with ID “*menu*” to the message queue

- Waiter returns to the top of the loop to wait for a completed order.

**Message Queue Functions**

The System V message queue API consists of the following system calls:

[msgget(2)](https://www.man7.org/linux/man-pages/man2/msgget.2.html)

Create a new message queue or obtain the ID of an existing message queue. This call returns an identifier that is used in the remaining APIs.

[msgsnd(2)](https://www.man7.org/linux/man-pages/man2/msgsnd.2.html)

Add a message to a queue.

[msgrcv(2)](https://www.man7.org/linux/man-pages/man2/msgrcv.2.html)

Remove a message from a queue.

[msgctl(2)](https://www.man7.org/linux/man-pages/man2/msgctl.2.html)

Perform various control operations on a queue, including deletion.

In addition, the ftok() function is used to get the “key” required in the creation of the message queue.

The structure for a message in the message queue is:

    // Message structure

    struct order {

     long type; //Message type: 1 for menus, 2 for orders, 3 for completed orders, etc.

     char content[MSG_SIZE];

    };

The “*order*” message that originates with the customer will include as content:

"Order from Customer ‘PID’", where PID is the PID of the customer process sending the message.

This *message content* will be passed to the Chef processing the order and then to the Waiter that receives the completed order.

Each time a process receives or sends a message, the process will print information about the event and the time of the event, creating a log of the activities in the restaurant. Example printed events are:

"Chef PID received: ‘order content msg from customer’ ”, time_of_event

"Chef PID completed: ‘order content msg from customer’ ” ,  time_of_event

"Customer PID placed an order: ‘order content msg from customer’ ” , time_of_event

‘order content msg from customer’ is the message sent from the customer with the order including the customer PID.

You need to have adequate precision for the time of the event to ensure that the actual time of each event is unique. Use the gettimeofday() function to get seconds and microseconds.

The log can be used by you to help trace activities and ensure that all events are correlated.

Input parameters for control of your project include:

- Number of orders per customer process (each customer process will request the same number of orders)

- Number of customer processes

- Number of chef processes

**Submission**

In addition to your commented source code, you will submit the log of a complete run of your simulation.

The top line of your log is to include

Your TUID # of Orders # Customers # Chefs

NOTE: Be sure to remove all messages from your queue and all processes when your program terminates. Be sure to check when debugging your program that none of the elements are left behind. If you do not remove these resources, you may exhaust the resources on the server and you will not be able to continue work.

Before termination of the program, be sure to wait for all processes to complete and remove the message queue.

**Project Requirements**

You are to implement the complete ‘restaurant management system’ as described in this document. This includes designing, implementing and managing the waiter, customer and chef processes. You are to implement the System V message queue and the messaging system parameters to communicate among the various system processes.

Your solution should take as input a variable number of orders, chefs and customer processes. The same number of orders are to be submitted by each customer process. You are to use your TUID as the seed for random number generation.

Week 1 Deliverables

- Learn to use the System V message functions.
- Develop use of the time function to use in the activity logging
- Design the messaging components
- Design and implement the control and launch of the various processes in the system.
- Complete the Waiter Process Functionality

Week 2 Submission

- Complete the Customer Process Functionality
- Complete the Chef Process Functionality
- Implement a solution to shutdown the processes when there are no more customer orders.
- Test the integrated components in a complete solution.
- Produce several runs of the project with different parameters (orders, customers, chefs) and save the logs.  
