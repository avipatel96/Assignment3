#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h> // needed to timestamp 

pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // declaring and intializing conditional variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;// declaring and initializing mutex

//comparator function for qsort
int sort(const void* p, const void* q) {
    return (*(int *)p - *(int *)q);
}

int syncClock;  //syncClock for synchronizing the the threads
int customerNumber; // number of customers per seller
char seats[10][10][10]; // mapping of the seats
// sequencing of the rows for mid row seller s given in the problem.
int middleSequence[10] = {5,6,4,7,3,8,2,9,1,0}/*{4,5,3,6,2,7,1,8,0,9} */; 
int highPriorityCustServed = 0, midPriorityCustServed = 0, lowPriorityCustServed = 0, soldOut = 0, salesClosed = 0; 

// function for the selling threads to execute
void *sell(void *sellerPriority_ptr) {
    // necessary variables.
    char *sellerPriority = sellerPriority_ptr;
    int *q;
    int i, j, k, finished;
    int currentTime = 0; 

    
    // allocate the size of queue for customers
    q = malloc(customerNumber * sizeof(int));
   
    for (i = 0; i < customerNumber; i++) {
		// generation of customers at random times during the hour.
		// q holds the customer at random minutes.
        q[i] = rand() % 60; // 60 because we have only an hr.
    }

    // q is sorted by using the function sort above to sort customers on the basis of their arrivals.
    qsort((void *) q, customerNumber, sizeof(int), sort);

    i = 0;

    while (i < customerNumber && syncClock < 60) {
        finished = 0;
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);

		//fill up the seats(array) of the theatre until time runs out but stop as 1 hr gets over.
        if (syncClock == 60) { 
            pthread_mutex_unlock(&mutex);
            break;
        }

		//time when customer arrived
        if (q[i] == syncClock) { 
            // print the arrival of customer
            //printf("At [0:%02d] customer priority : %c and seller number : %c customer %c%c%02d arrived in queue\n", syncClock, sellerPriority[0], sellerPriority[1], sellerPriority[0], sellerPriority[1], i+1);
            printf("[0:%02d] Customer %c%c%02d arrived in queue, having customer priority : %c with seller number : %c\n", syncClock, sellerPriority[0], sellerPriority[1], i+1, sellerPriority[0], sellerPriority[1]);

        }

		//serving the next customer
        if (currentTime <= syncClock && q[i] <= syncClock) { 
            // check priority and check seat availability to fill the seat.
            if (sellerPriority[0] == 'L') {
                for (j = 9; !finished && j >= 0; j--)
                    for (k = 0; !finished && k < 10; k++)
                        if (strlen(seats[j][k]) == 0) {
                            sprintf(seats[j][k], "%c%c%02d", sellerPriority[0], sellerPriority[1], i+1);
                            //printf("[0:%02d] %c%c - alloted seat  %d to customer %c%c%02d\n", syncClock, sellerPriority[0], sellerPriority[1], j*10+k+1, sellerPriority[0], sellerPriority[1], i+1);
                            printf("[0:%02d] customer %c%c%02d was alloted seat %d by seller number %c%c\n", syncClock, sellerPriority[0], sellerPriority[1], i+1, j*10+k+1, sellerPriority[0], sellerPriority[1]);
                            finished = 1;
                            lowPriorityCustServed++;
                            
                        }

                //if ticket is sold increment the global counter
                if (finished)
                    currentTime = syncClock + rand() % 4 + 4;
            }

            if (sellerPriority[0] == 'M') {
                for (j = 0; !finished && j < 10; j++)
                    for (k = 0; !finished && k < 10; k++)
						//if seat is available
                        if (strlen(seats[middleSequence[j]][k]) == 0) {
                            sprintf(seats[middleSequence[j]][k], "%c%c%02d", sellerPriority[0], sellerPriority[1], i+1);
                            //printf("[0:%02d] %c%c - alloted seat  %d to customer %c%c%02d\n", syncClock, sellerPriority[0], sellerPriority[1], middleSequence[j]*10+k+1, sellerPriority[0], sellerPriority[1], i+1);
                            printf("[0:%02d] customer %c%c%02d was alloted seat %d by seller number %c%c\n", syncClock, sellerPriority[0], sellerPriority[1], i+1, middleSequence[j]*10+k+1, sellerPriority[0], sellerPriority[1]);
                            finished = 1;
                            midPriorityCustServed++;
                            
                        }

                //if ticket is sold increment the global counter
                if (finished)
                    currentTime = syncClock + rand() % 3 + 2;
            }
            
            
            if (sellerPriority[0] == 'H') {
                for (j = 0; !finished && j < 10; j++)
                    for (k = 0; !finished && k < 10; k++)
                        //if seat is available
                        if (strlen(seats[j][k]) == 0) { 
                            sprintf(seats[j][k], "%c%c%02d", sellerPriority[0], sellerPriority[1], i+1);
                            //printf("[0:%02d] %c%c - alloted seat  %d to customer %c%c%02d\n", syncClock, sellerPriority[0], sellerPriority[1], j*10+k+1, sellerPriority[0], sellerPriority[1], i+1);
                            printf("[0:%02d] customer %c%c%02d was alloted seat %d by seller number %c%c\n", syncClock, sellerPriority[0], sellerPriority[1], i+1, j*10+k+1, sellerPriority[0], sellerPriority[1]);
                            finished = 1;
                            highPriorityCustServed++;
                            
                        }

                //if ticket is sold increment the global counter
                if (finished)
                    currentTime = syncClock + rand() % 2 + 1;
            }
            

			//ticket sold out
            if (!finished) 
                while (i < customerNumber && q[i] <= syncClock) {
                   // printf("[0:%02d] %c%c|| Tickets are sold out.Customer - %c%c%02d returned without ticket. \n", syncClock, sellerPriority[0], sellerPriority[1], sellerPriority[0], sellerPriority[1], i+1);
                    printf("[0:%02d] %c%c%02d returned without ticket as tickets are sold out. \n", syncClock, sellerPriority[0], sellerPriority[1], i+1);
                    soldOut++;
                    i++;
                }
            else
                i++;
        }

         pthread_mutex_unlock(&mutex);
    }

    //when the time runsout - 
    while (i < customerNumber) {
        printf("[0:%02d] %c%c%02d couldn't get ticket as time has run out.\n", syncClock, sellerPriority[0], sellerPriority[1], i+1);
        salesClosed++;
        i++;
    }

    free(q);
    return NULL;
}

void wakeup_all_seller_threads(void) {
    // function will wake up all 1- threads 
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    struct timespec tees = {0, 1000000L}; 
    // allow the threads to finish
    nanosleep(&tees, NULL);
}

int main(int argc, char *argv[]) {
    int i, j;
    pthread_t threadIds[10];
    char sellerPriority[10][2];

    //if number of customers is not entered
    if (argc != 2) {
       printf("Number of customers : ");
       scanf("%d",&customerNumber);
       return 1; 
    }

    sscanf(argv[1], "%d", &customerNumber); // get  number of customers
    srand(time(NULL)); 

    memset(seats, 0, 10*10*10); 
    // set each seat to 0 in the theatre data structure.

    //indicate the seller priority and the number of queue and create threads 
    sellerPriority[0][0] = 'H';
    sellerPriority[0][1] = '1';
    pthread_create(&threadIds[0], NULL, sell, sellerPriority[0]);

    for (i = 1; i < 4; i++) {
        sellerPriority[i][0] = 'M';
        sellerPriority[i][1] = '0' + i;
        pthread_create(&threadIds[i], NULL, sell, sellerPriority[i]);
    }

    for (i = 4; i < 10; i++) {
        sellerPriority[i][0] = 'L';
        sellerPriority[i][1] = '0' + i - 3;
        pthread_create(&threadIds[i], NULL, sell, sellerPriority[i]);
    }

    // wake up the seller threds and run for 61 minutes 
    // 1 extra minute for the completion.
    for (syncClock = 0; syncClock < 61; syncClock ++) {
        wakeup_all_seller_threads();
    }

    // pthread join is used to wait for the thread to finished 
    // it is put at the end of creation and waking up of all threads.
    for (i = 0; i < 10; i++) {
        pthread_join(threadIds[i], NULL);
    }

    printf("\n\n");
    printf("The final seating arrangement - \n\n");

    printf("                             Theatre                            \n\n");

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            printf("[%4s]", seats[i][j]);
        }
        printf("\n\n");
    }

    printf("-------------------------------------------------------------\n\n");

    printf("STATS - \n");

    // print seat statistics after selling
    printf("High priority customers served - %d\nMedium priority customers served- %d\nLow priority customers served- %d\nCustomers turned away because tickets sold out- %d\nCustomers turned away because sales closed- %d\n", highPriorityCustServed, midPriorityCustServed, lowPriorityCustServed, soldOut, salesClosed);

    return 0;
}