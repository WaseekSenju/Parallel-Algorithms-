//Task -3  Find Prime numbers from data(array) by using Sieve's Algorithm with Block distribution

#include <iostream>
#include <mpi.h>
#include <cmath>
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
    int sqrRoot = 0;

    int *numbers = NULL;
    int *sqrt_array = NULL;

    if (rank == 0)
    {
        cout << "Enter the number that you want to get prime numbers upto that number:" << endl;
        cin >> count; // assume count is 15
        sqrRoot = sqrt(count);

        numbers = new int[count];
        numbers[0] = 2;

        for (int i = 1; i < (count - 1); i++) // from 0 - 13 total 14 numbers
        {
            numbers[i] = i + 2;
        }

        for (int i = 0; i < (count - 1); i++) // from 0 - 13 total 14 numbers
        {
            cout << numbers[i] << ",";
        }

        cout<<"\n";

        sqrt_array = new int[sqrRoot];

        for (int i = 0; i < sqrRoot; i++)
        {
            sqrt_array[i] = numbers[i];
        }
    }

    // // braodcast current sqroot and count  to all the processes
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD); // the zero here is the master node (root node)
    MPI_Bcast(&sqrRoot, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int numbers_for_each_processor = count / procs;

    // extra numbers which are not equally divisible
    int count_extra = count % procs;

    int count_rk = numbers_for_each_processor;

    if (rank == 0)
    {

        // extra numbers are handled by process with Rank zero
        count_rk += count_extra;

        // Rank 0 process and send the appropriate share of numbers to the processes of other Ranks.

        for (int i = 1; i < procs; i++)
        {
            MPI_Send(&numbers[i * numbers_for_each_processor + count_extra], numbers_for_each_processor, MPI_INT, i, 100, MPI_COMM_WORLD);
            MPI_Send(sqrt_array, sqrRoot, MPI_INT, i, 101, MPI_COMM_WORLD); // amount of numbers upto sqrRoot will be Sqroot+1 for 0th index it has 1 right now for (2..15) array
        }
    }
    else
    {

        // allocate memory at other processes for storing their numbers
        numbers = (int *)malloc(count_rk * sizeof(int));

        sqrt_array = (int *)malloc(sqrRoot * sizeof(int));

        // Receive numbers array from process with the 0th rank
        MPI_Recv(numbers, count_rk, MPI_INT, 0, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(sqrt_array, sqrRoot, MPI_INT, 0, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Mark all the array numbers where the are multiple of the key

    for (int i = 0; i < sqrRoot; i++)
    {

        for (int j = 0; j < count_rk; j++)
        {
            if ((numbers[j] % sqrt_array[i]) == 0 && numbers[j] != sqrt_array[i])
            {
                numbers[j] = 0;
            }
        }
    }

    if (rank != 0)
    {
        MPI_Send(numbers, count_rk, MPI_INT, 0, 103, MPI_COMM_WORLD);
    }
    else
    {
        cout<<"The prime numbers in the range are : \n";
        for (int i = 0; i < count_rk; i++)
        {
            if (numbers[i] != 0)
                cout << numbers[i] << " ";
        }
        int *recievedArray = new int[numbers_for_each_processor];
        for (int i = 1; i < procs; i++)
        {
            MPI_Recv(recievedArray, numbers_for_each_processor, MPI_INT, i, 103, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < numbers_for_each_processor; i++)
            {
                if (recievedArray[i] != 0)
                    cout << recievedArray[i] << " ";
            }
        }
        cout<<endl;
    }
    free(numbers);
    MPI_Finalize();
    return 0;
}
