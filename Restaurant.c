#include <stdio.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>   
#include <sys/types.h> 

// define message types -> menu, order, order ready, keep track of different messages
#define MSG_menu 1
#define MSG_order 2
#define MSG_order_ready 3
#define CUSTOMERS 3 // number of customer processes to create

// create a structure to hold all the message data i.e menu, order, order ready 
struct message {
    long msg_type; // variable to hold message type
    char msg_text[100]; // variable to hold message text
};

// activity log for restaurant -> simple function to log activities, keep track of orders 
void log_event(const char *activity){
    // want to log activity and time implement file logging later
    struct timeval timeValue; // struct to hold the time 
    gettimeofday(&timeValue, NULL); // get the current time
    long milliseconds = timeValue.tv_usec / 1000; // get milliseconds
    struct tm *tm_info = localtime(&timeValue.tv_sec); // convert to local time
    printf("[%02d.%03ld] %s\n", tm_info->tm_sec, milliseconds, activity);; // print the activity with timestamp to, need to format the time properly not unix time
}

int waiter_process(int msgid){
     // create a waiter process 
    pid_t waiter_pid = fork();
    // check if the fork was unsuccessful 
    if(waiter_pid < 0){
        printf("Fork failed for waiter process\n");
        return -1;
    }

    // if the fork was successful
    if(waiter_pid == 0){
        // waiter process code 
        /* want to send the menu to the customer
        want to receive the order from the customer
        want to send the order to the kitchen 
        take each message and add it to the message queue then the other process can read from the message queue 
        */

        // creating structs for each message
        struct message menu_msg;  // create a message structure to hold the menu 
        struct message order_msg; // create a message struct of the order 
        struct message order_ready_msg; // create a message struct for order ready

        // creating the menu message
        snprintf(menu_msg.msg_text, sizeof(menu_msg.msg_text), "Menu: 1. Pizza 2. Burger 3. Pasta"); // set message text to menu items, size of used to prevent overflow
        menu_msg.msg_type = MSG_menu; // set message type to menu

        // send initial menu -> have to enter the while loop with an initial menu send
        /* For the msgsnd: send it to the queue id, pointer to the message, size of the message excluding the long msg_type -> only text, and 0 to block if the queue is full
        */
        msgsnd(msgid, &menu_msg, sizeof(menu_msg) - sizeof(long), 0);
        log_event("Waiter sent menu to customer");

        while(1){
        
        // receive the order from the customer 
        // for msgrcv: receive from the queue id, pointer to the message, size of the message excluding the long msg_type i.e only the text,
        // message type to receive, and 0 to block if no message is available
        msgrcv(msgid, &order_msg, sizeof(order_msg) - sizeof(long), MSG_order, 0); // receive the order from the customer, - sizeof(long) to exclude msg_type from size -> 
        printf("Waiter received order: %s\n", order_msg.msg_text); // print the order received
        log_event("Waiter received order from customer"); 

        // send the order to the kitchen
        msgsnd(msgid, &order_msg, sizeof(order_msg) - sizeof(long), 0); // send the order to the kitchen
        log_event("Waiter sent order to kitchen"); // log the event 
        printf("Waiter sent order to kitchen: %s\n", order_msg.msg_text); // print the order sent

        // receive the order ready message from the chef process
        msgrcv(msgid, &order_ready_msg, sizeof(order_ready_msg) - sizeof(long), MSG_order_ready, 0); // receive the order ready message from the chef
        printf("Waiter received order ready: %s\n", order_ready_msg.msg_text); // say we got the message 
        log_event("Waiter received order ready from chef"); // log the event 


        // send order to customer when ready -> simulate with sleep 
        sleep(2);

        // send the menu another to the customer
        msgsnd(msgid, &menu_msg, sizeof(menu_msg) - sizeof(long), 0); // send the menu to the customer
        log_event("Waiter sent menu to customer");
        }

        exit(0); // exit the waiter process
    }
    return waiter_pid; // return the waiter process id
}

int customer_process(int msgid){
    // want to create a customer process that can recieve menu and order food 
    // create the customer process
    pid_t customer_pid = fork(); 
    // check if the fork was unsuccessful
    if(customer_pid < 0){
        printf("Fork failed for customer process\n");
        return -1;
    }

    // if the fork was successful
    if(customer_pid == 0){

        // struct declarations for the menu and order
        struct message menu_msg;
        struct message order_msg;

        srand(916313516 + getpid()); // seed the random number generator with my tuID

        // create the menu 
        const char *menu_items[] = {"Pizza", "Burger", "Pasta"};
        int num_items = 3;

        while(1) { // while loop to simulate multiple customers/tables -> they don't go away 

        // want to receive the menu from the waiter
        // want to send the order to the waiter 

        // receive the menu from the waiter
        msgrcv(msgid, &menu_msg, sizeof(menu_msg) - sizeof(long), MSG_menu, 0); // receive the menu from the waiter
        printf("Customer received menu: %s\n", menu_msg.msg_text); // print the menu received
        log_event("Customer received menu from waiter");

        sleep(1); // simulate time taken to decide order

        // get the order from the customer 
        int choice = rand() % num_items; // random choice from the menu items
        order_msg.msg_type = MSG_order; // set message type to order
        snprintf(order_msg.msg_text, sizeof(order_msg.msg_text), "Order: %s", menu_items[choice]); // set message text to order item 

        // send the order to the waiter
        msgsnd(msgid, &order_msg, sizeof(order_msg) - sizeof(long), 0); // send the order to the waiter
        log_event("Customer sent order to waiter"); // add event to the log 
        printf("Customer sent order to waiter: %s\n", order_msg.msg_text); // print the order sent

        // customer eat time 
    
        int eat_time = rand() % 5 + 2; // random eat time between 1 and 6 seconds
        sleep(eat_time); // simulate eating time
        }

        exit(0); // exit the customer process
    }
    return customer_pid; // return the customer process id
}

int chef_process(int msgid){
    // create the chef process 
    pid_t chef_pid = fork();
    // check if the fork was unsuccessful
    if(chef_pid < 0){
        printf("Fork failed for chef process\n");
        return -1;
    }
    // if the fork was successful
    if(chef_pid == 0){
        // want to receive the order from the waiter
        // make the chef process keep running to simulate multiple orders
        // want to send the order ready message to the waiter

        // struct declaration for the order
        struct message order_msg;
        
        srand(916313516 + getpid()); // seed the random number generator with my tuID

        while(1){ // loop to keep chef going 
            // receive the order from the waiter
            msgrcv(msgid, &order_msg, sizeof(order_msg) - sizeof(long), MSG_order, 0); // receive the order from the waiter
            printf("Chef received order: %s\n", order_msg.msg_text); // print the order received
            log_event("Chef received order from waiter"); // log the event

            // simulate cooking time
            int cook_time = rand() % 5 + 1; // random cook time between 2 and 5 seconds
            sleep(cook_time); // simulate cooking time

            // send the order ready message to the waiter
            struct message order_ready_msg;
            order_ready_msg.msg_type = MSG_order_ready; // set message type to order ready
            snprintf(order_ready_msg.msg_text, sizeof(order_ready_msg.msg_text), "Order Ready: %s", order_msg.msg_text); // set message text to order ready

            msgsnd(msgid, &order_ready_msg, sizeof(order_ready_msg) - sizeof(long), 0); // send the order ready message to the waiter
            log_event("Chef sent order ready to waiter"); // log the event 
            printf("Chef sent order ready to waiter: %s\n", order_ready_msg.msg_text); // print the order ready sent

        }
        exit(0); // exit the chef process
    }
    return chef_pid; // return the chef process id
}

// main function 
int main(int argc, char *argv[]){
    /* want to create a message queue to hold all the messages to be used by the restaurant processes. 
    */

    // create a key to be used for message queue -> this key is used to identify the message queue
    key_t key = ftok("restaurant.c", 65);

    // create a message queue to hold all the messages 
    // 0666 is the permission to read and write the message queue, IPC_CREAT is used to create the message queue if it does not exist
    int msgid = msgget(key, 0666 | IPC_CREAT);
    
    // check if the message queue was created successfully
    if (msgid == -1) {
        printf("Message queue creation failed\n");
        return 1;
    }
    else { // success message 
        printf("Message queue created successfully with id: %d\n", msgid);
    }

    // call all the functions 
    pid_t waiter_pid = waiter_process(msgid); // create 1 waiter process

    // create muliple customer processes to simulate multiple tables
    pid_t customers[CUSTOMERS];
    for(int i = 0; i < CUSTOMERS; i++){
        customers[i] = customer_process(msgid); // create the customer process
    }

    pid_t chef_pid = chef_process(msgid); // create the chef process

    sleep(10); // run the resturant for n seconds 
    // after n seconds we will end the simulation -> stop all processes
    kill(waiter_pid, SIGTERM);
    for(int i = 0; i < CUSTOMERS; i++){
        kill(customers[i], SIGTERM);
    }
    kill(chef_pid, SIGTERM);

    
    // destroy the message queue after use
    msgctl(msgid, IPC_RMID, NULL); // remove the messages from the queue
    printf("simulation ended, message queue destroyed\n");

    return 0; 
}