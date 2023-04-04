#include <stdio.h>
#include <stdlib.h>

// Define the Process structure
typedef struct Process
{
    int id;
    int arrival_time;
    int burst_time;
    int priority;
    int run_time;
    struct Process *next;
} Process;

// Define the ReadyQueue structure
typedef struct
{
    Process *head;
} ReadyQueue;

// Define the IOQueue structure
typedef struct
{
    Process *head;
} IOQueue;

// Initialize the ready queue
void init_ready_queue(ReadyQueue *queue)
{
    queue->head = NULL;
}

// Initialize the I/O queue
void init_io_queue(IOQueue *queue)
{
    queue->head = NULL;
}

// Add a process to the ready queue
void add_process_to_ready_queue(ReadyQueue *queue, Process *process)
{
    process->next = NULL;
    if (queue->head == NULL)
    {
        queue->head = process;
    }
    else
    {
        Process *current = queue->head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = process;
    }
}

// Remove a process from the ready queue
Process *remove_process_from_ready_queue(ReadyQueue *queue)
{
    if (queue->head == NULL)
    {
        return NULL;
    }
    else
    {
        Process *removed_process = queue->head;
        queue->head = queue->head->next;
        removed_process->next = NULL;
        return removed_process;
    }
}

// Add a process to the I/O queue
void add_process_to_io_queue(IOQueue *queue, Process *process)
{
    process->next = NULL;
    if (queue->head == NULL)
    {
        queue->head = process;
    }
    else
    {
        Process *current = queue->head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = process;
    }
}

// Remove a process from the I/O queue
Process *remove_process_from_io_queue(IOQueue *queue)
{
    if (queue->head == NULL)
    {
        return NULL;
    }
    else
    {
        Process *removed_process = queue->head;
        queue->head = queue->head->next;
        removed_process->next = NULL;
        return removed_process;
    }
}

// Run a process for a specified time slice
void run_process(Process *process, int time_slice)
{
    printf("Running process %d for %d time units\n", process->id, time_slice);
    process->run_time += time_slice;
}

// Put a process in the wait state for a specified time slice
void wait(Process *process, int time_slice)
{
    printf("Process %d is waiting for I/O for %d time units\n", process->id, time_slice);
}

// Main function
int main()
{
    // Initialize the ready queue and the I/O queue
    ReadyQueue ready_queue;
    init_ready_queue(&ready_queue);
    IOQueue io_queue;
    init_io_queue(&io_queue);

    // Create three processes
    Process *process1 = (Process *)malloc(sizeof(Process));
    process1->id = 1;
    process1->arrival_time = 0;
    process1->burst_time = 5;
    process1->priority = 1;
    process1->run_time = 0;

    Process *process2 = (Process *)malloc(sizeof(Process));
    process2->id = 2;
    process2->arrival_time = 0;
    process2->burst_time = 10;
    process2->priority = 2;
    process2->run_time = 0;

    Process *process3 = (Process *)malloc(sizeof(Process));
    process3->id = 3;
    process3->arrival_time = 0;
    process3->burst_time = 15;
    process3->priority = 3;
    process3->run_time = 0;

    // Add the processes to the ready queue
    add_process_to_ready_queue(&ready_queue, process1);
    add_process_to_ready_queue(&ready_queue, process2);
    add_process_to_ready_queue(&ready_queue, process3);

    // Set the time slice for each run of the CPU
    int time_slice = 2;

    // Set the context-switch time (CST) penalty factor
    int cst_penalty_factor = 1;

    // Set the aging threshold
    int aging_threshold = 5;

    // Initialize the simulation clock
    int clock = 0;

    // Run the simulation until all processes have completed
    while (ready_queue.head != NULL || io_queue.head != NULL)
    {
        // Check if there are any processes in the I/O queue that have completed I/O
        Process *io_process = io_queue.head;
        while (io_process != NULL)
        {
            if (io_process->run_time == io_process->burst_time)
            {
                // Remove the process from the I/O queue and add it to the ready queue
                printf("Process %d has completed I/O\n", io_process->id);
                io_process = remove_process_from_io_queue(&io_queue);
                add_process_to_ready_queue(&ready_queue, io_process);
            }
            else
            {
                io_process = io_process->next;
            }
        }

        // Check if there are any processes in the ready queue
        if (ready_queue.head != NULL)
        {
            // Remove the next process from the ready queue
            Process *current_process = remove_process_from_ready_queue(&ready_queue);

            // Check if the current process has completed its CPU burst
            if (current_process->run_time + time_slice >= current_process->burst_time)
            {
                // Run the process for the remaining time
                int remaining_time = current_process->burst_time - current_process->run_time;
                run_process(current_process, remaining_time);

                // Update the simulation clock
                clock += remaining_time;

                // Print the results
                printf("Process %d has completed at time %d\n", current_process->id, clock);

                // Free the memory for the process
                free(current_process);
            }
            else
            {
                // Run the process for the time slice
                run_process(current_process, time_slice);

                // Update the simulation clock
                clock += time_slice;

                // Add the process back to the ready queue
                add_process_to_ready_queue(&ready_queue, current_process);

                // Check if the process has run for too long
                if (current_process->run_time >= aging_threshold)
                {
                    // Transfer the process to the higher-priority queue
                    printf("Process %d has been transferred to the high-priority queue\n", current_process->id);
                    current_process = remove_process_from_ready_queue(&ready_queue);
                    current_process->priority--;
                    add_process_to_ready_queue(&ready_queue, current_process);
                }
            }
        }
        else
        {
            // No processes in the ready queue, so update the simulation clock
            clock += cst_penalty_factor;
        }

        // Check if there are any processes in the I/O queue
        if (io_queue.head != NULL)
        {
            // Wait for all processes in the I/O queue for the time slice
            Process *current_process = io_queue.head;
            while (current_process != NULL)
            {
                run_process(current_process, time_slice);
                current_process = current_process->next;
            }

            // Update the simulation clock
            clock += time_slice;

            // Add the CST penalty to the simulation clock
            clock += cst_penalty_factor;
        }
    }

    return 0;
}
