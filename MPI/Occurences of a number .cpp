 // Task 1&2 - Find the occurrences of a number in linear array by using parallel programing. 
 // Task 1 was easier subset of Task 2 so here is only Task 2

#include <iostream>
#include <mpi.h>
#include <time.h>
using namespace std;
// 1- A communicator defines a communication domani - set of porcesses that are allowed to communicate -> these informations are store in MPI_Comm
// 2- MPI_COMM_WORLD is the default communicator which includes all processes

int main(int argc, char **argv)
{
    int rank, procs;

    MPI_Init(NULL, NULL);                  // Initlialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &procs); // determines the number of process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // determines the label of the calling process  (Ranges from 0 upto size of communicator - 1 )

    int count = 0;
    int *numbers = NULL;
    int key = 9;

    if (rank == 0)
    {
        int size = 0;
        cout << "Enter the size of the array:" << endl;
        cin >> size;
        count = size;

        numbers= (int*)malloc(size * sizeof(int));
        srand((unsigned) time(NULL));
        for (int i = 0; i < size; i++)
        {
            numbers[i] = rand() % 10;
            cout << numbers[i] << ",";
        }
    }

    // braodcast count of numbers to all the processes
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD); // the zero here is the master node (root node)

    int numbers_for_each_processor = count / procs;

    // extra numbers which are not equally divisible
    int count_extra = count % procs;

    int count_rk = numbers_for_each_processor;

    int i = 0;

    if (rank == 0)
    {
        // extra numbers are handled by process with Rank zero
        count_rk += count_extra;

        // Rank 0 process and send the appropriate share of numbers to the processes of other Ranks.

        for (i = 1; i < procs; i++)
        {
            MPI_Send(&numbers[i * numbers_for_each_processor + count_extra], numbers_for_each_processor, MPI_INT, i, 100, MPI_COMM_WORLD);
        }
    }
    else
    {
        // allocate memory at other processes for storing their numbers
        numbers = (int *)malloc(numbers_for_each_processor * sizeof(int));

        // Receive numbers from 0 rank process
        MPI_Recv(numbers, numbers_for_each_processor, MPI_INT, 0, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // find most occuring

    int total = 0;
    for (i = 0; i < count_rk; i++)
    {
        if (numbers[i] == key)
        {
            total++;
        }
    }
    int temp=0;
    
    if (rank != 0)
    {
        MPI_Send(&total, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
    }
    else
    {
        for (i = 1; i < procs; i++)
        {
            // receive total from other processes
            MPI_Recv(&temp, 1, MPI_INT, i, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += temp;
        }

        // display everything
        printf("Total is %d\n", total);
    }
    free(numbers);

    MPI_Finalize();
    return 0;
}
