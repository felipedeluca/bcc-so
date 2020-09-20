/*
    Find PI using Monte Carlo algoritm.
    Total CPU processes: 4.

    References:
        https://www.geeksforgeeks.org/estimating-value-pi-using-monte-carlo/
        https://www.tutorialspoint.com/c-program-to-demonstrate-fork-and-pipe
        https://tldp.org/LDP/lpg/node11.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

time_t t;
typedef unsigned long long points_t;
double calculatePI(int, points_t);
void parentProcess(int, points_t, int[]);
void childProcess(int, points_t, int[]);
void writeResult(int, int[], double);
void processTask(int, int[], points_t);


int main(void) {
    printf("\n");
    printf("\n -> Calculating PI using Monte Carlo Method using distributed processes.\n");
    points_t pointCount = 100000000;
    int taskResults1[2];
    int taskResults2[2];
    int taskResults3[2];

    // Create pipeline to allow each task to send back the results to the main process.
    if (pipe(taskResults1) == -1 || pipe(taskResults2) == -1 || pipe(taskResults3) == -1) {
        fprintf(stderr, "Falied to create pipe");
        return 1;
    }

    // Distribute the number of points per task.
    points_t pointsPerProcess = (points_t) (pointCount / 4);
    points_t remainingPoints = (points_t) (pointCount % 4);
    int taskID = -1;

    // Create separated processes
    pid_t id1;
    pid_t id2;
    id1 = fork();
    id2 = fork();

    if (id1 == 0 && id2 == 0) {
        // Task branch: Main(id1) -> child -> child(id2)
        processTask(3, taskResults3, pointsPerProcess);
    } else if (id1 == 0 && id2 > 0) {
        // Task branch: Main(id1) -> child -> parent(id2)
        processTask(2, taskResults2, pointsPerProcess);
    } else if (id1 > 0 && id2 == 0) {
        // Task branch: Main(id1) -> parent -> child(id2)
        processTask(1, taskResults1, pointsPerProcess);
    } else if (id1 > 0 && id2 > 0) {
        // MAIN PROCESS
        // Task branch: Main(id1) -> parent -> parent(id2)
        wait(NULL);
        printf("\n Task (%d) - pID: %ld \t pPID: %ld", 0, (long)getpid(), (long)getppid());
        wait(NULL);

        double result1;
        close(taskResults1[1]);
        read(taskResults1[0], &result1, sizeof(double));

        double result2;
        close(taskResults2[1]);
        read(taskResults2[0], &result2, sizeof(double));

        double result3;
        close(taskResults3[1]);
        read(taskResults3[0], &result3, sizeof(double));

        double result4 = calculatePI(taskID, pointsPerProcess);
        double PI = (result1 + result2 + result3 + result4) / 4.0;
        printf("\n\n-----------------------\n");
        printf("FINAL RESULT \n");
        printf("-----------------------\n");
        printf("-> PI (aprox.) = %1.7f\n", PI);
        printf("\n");
    }

    return(EXIT_SUCCESS);
}


void processTask(int taskID, int taskPipe[], points_t pointsPerProcess) {
    printf("\nTask (%d) - pID: %ld \t pPID: %ld", taskID, (long)getpid(), (long)getppid());
    double PI = calculatePI(taskID, pointsPerProcess);
    close(taskPipe[0]);
    write(taskPipe[1], &PI, sizeof(double));
}


double calculatePI(int taskID, points_t numOfPoints) {
    srand((unsigned) time(&t) + (long) getpid());
    unsigned long long pointsInCircle = 0;
    unsigned long long pointsInSquare = 0;

    for (; pointsInSquare < numOfPoints; pointsInSquare++) {
        double x = (rand() % 101) / (double)100;
        double y = (rand() % 101) / (double)100;

        double p = (x * x) + (y * y);
        if (p <= 1) {
            pointsInCircle += 1;
        }
    }

    double PI = 4.0 * (pointsInCircle / (double)pointsInSquare);

    printf("\n   Task ID: %d\n",  taskID);
    printf("   Points in circle: %llu \t Total points: %llu \t PI = %1.7f\n\n", pointsInCircle, pointsInSquare, PI);
    return PI;
}