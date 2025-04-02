#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

// semaphores
sem_t tabacco;
sem_t paper;
sem_t match;

// agent semaphores
sem_t agentSem;

// boolean variables
bool isTabacco = false;
bool isPaper = false;
bool isMatch = false;

/// smoker semaphores
sem_t mutex;
sem_t tabaccoSem;
sem_t paperSem;

// mutual exclusion semaphore
sem_t mutex;

void* agent(void* arg) {

    sem_wait(&agentSem);
    int ingredient = rand() % 3;
    usleep((rand() % 201) * 1000);
    switch (ingredient) {
        case 0:
            printf("Agent: Tabacco & paper \n");
            sem_post(&paper);
            sem_post(&tabacco);
            break;
        case 1:
            printf("Agent: Tabacco & match \n");
            sem_post(&tabacco);
            sem_post(&match);
            break;
        case 2:
            printf("Agent: Paper & match\n");
            sem_post(&paper);
            sem_post(&match);
            break;
    }

    return NULL;
}

void* tabacco_pusher(void* arg) {
    while (true) {
        P(tabacco);
        P(mutex);
        if (isPaper) {
            isPaper = false;
            V(matchSem);
        } else if (isMatch) {
            isMatch = false;
            V(paperSem);
        } else {
            isTabacco = true;
            V(mutex);
        }
    }
}

int main() {

    //create thread
    pthread_t agent_threads[3];
    for (int i = 0; i < 3; i++) {
        pthread_create(&agent_threads[i], NULL, agent, NULL);
    }
}