#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAX_BACKOFF_ATTEMPTS 5
#define SLOT_TIME 1 // Time taken for one slot (in seconds)

// Function to simulate transmission with binary exponential backoff
int binary_exponential_backoff(int attempt) {
    int backoff_time = (1 << attempt) * SLOT_TIME;
    printf("Attempt %d: Backing off for %d slot times.\n", attempt, backoff_time);
    return backoff_time;
}

int backoff() {
    int success = 0;
    int attempt = 0;
    int backoff_time;
    
    while (!success && attempt < MAX_BACKOFF_ATTEMPTS) {
        // Simulate transmission attempt
        if (rand() % 5 == 0) { // Simulate a successful transmission with 20% probability
            printf("Transmission successful!\n");
            success = 1;
        } else {
            backoff_time = binary_exponential_backoff(attempt);
            usleep(backoff_time); // Micro Sleep for backoff time
            attempt++;
        }
    }
    
    if (!success) {
        printf("Transmission failed after %d attempts.\n", MAX_BACKOFF_ATTEMPTS);
    }
    
    return 0;
}

long getMicrotime() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

int x = 0;

void capture() {
    exit(0);
}

int get() {
    return x;
}

void put() {
    x++;
}

void node(char *p) {
    int name;
    int seq1, seq2, i = 0;
    long timemicro;
    
    name = atoi(p);
    
    while (1) {
        seq1 = get();
        seq2 = get();
        
        if (seq1 == seq2) {
            put();
            seq1 = get();
            timemicro = getMicrotime();
            printf("station %d transmitting frame %d at %ld \n", name, ++i, timemicro);
            usleep(10);
            seq2 = get();
            
            if (seq1 != seq2) {
                printf("station %d collision occurred %d \n", name, i--);
                backoff();
            } else {
                printf("station %d transmission of frame %d success \n", name, i);
            }
        }
        
        usleep(200000);
    }
}

int main() {
    pthread_t t1, t2, t3;
    signal(SIGINT, capture);
    
    pthread_create(&t1, NULL, (void *)node, "1");
    pthread_create(&t2, NULL, (void *)node, "2");
    pthread_create(&t3, NULL, (void *)node, "3");
    
    while (1);
    
    return 0;
}
