// TASK6 : Use MPI_Scatter in TASK3 (Sieve's Algorithm- Block distribution)

#include <iostream>
#include <cmath>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int processors;
    MPI_Comm_size(MPI_COMM_WORLD, &processors);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *numbers = nullptr;
    int *sqrtArray = nullptr;

    int sizeOfArray;
    int sqRoot;

    if (rank == 0)
    {
        cout << "Enter the range of numbers that you wanna generato prime numbers : \n";
        cin >> sizeOfArray;
        sqRoot = sqrt(sizeOfArray);
        numbers = new int[sizeOfArray];
        numbers[0] = 2;
        for (int i = 1; i < (sizeOfArray - 1); i++)
        {
            numbers[i] = i + 2;
        }

        for (int i = 0; i < (sizeOfArray - 1); i++)
        {
            cout << numbers[i] << ",";
        }
        cout << endl;
        sqrtArray = new int[sqRoot];
        for (int i = 0; i < sqRoot; i++)
        {
            sqrtArray[i] = numbers[i];
        }
    }

    MPI_Bcast(&sizeOfArray, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&sqRoot, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int initialBlocSize = sizeOfArray / processors;

    int *subArray = new int[initialBlocSize];

    MPI_Scatter(numbers, initialBlocSize, MPI_INT, subArray, initialBlocSize, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {

        for (int i = 1; i < processors; i++)
        {
            MPI_Send(sqrtArray, sqRoot, MPI_FLOAT, i, 101, MPI_COMM_WORLD);
        }
    }
    else
    {
        numbers = new int[initialBlocSize];
        sqrtArray = new int[sqRoot];
        MPI_Recv(sqrtArray, sqRoot, MPI_FLOAT, 0, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < sqRoot; i++)
    {

        for (int j = 0; j < initialBlocSize; j++)
        {
            if ((subArray[j] % sqrtArray[i]) == 0 && subArray[j] != sqrtArray[i])
            {
                subArray[j] = 0;
            }
        }
    }

    if (rank != 0)
    {
        MPI_Send(subArray, initialBlocSize, MPI_INT, 0, 102, MPI_COMM_WORLD);
    }
    else
    {
        for (int i = 0; i < initialBlocSize; i++)
        {
            if (subArray[i] != 0)
                cout << subArray[i] << ",";
        }
        int *recievedArray = new int[initialBlocSize];

        for (int i = 1; i < processors; i++)
        {
            MPI_Recv(recievedArray, initialBlocSize, MPI_INT, i, 102, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < initialBlocSize; i++)
            {
                if (recievedArray[i] != 0)
                    cout << recievedArray[i] << ",";
            }
        }
    }
    free(numbers);
    free(subArray);
    MPI_Finalize();
}
