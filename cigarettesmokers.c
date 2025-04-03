#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>

// semaphores
sem_t tabacco;
sem_t paper;
sem_t match;

// agent semaphores
sem_t agentSem;

// boolean variables
int isTabacco = 0;
int isPaper = 0;
int isMatch = 0;

/// smoker semaphores
sem_t tabaccoSem;
sem_t paperSem;
sem_t matchSem;

// mutual exclusion semaphore
sem_t mutex;

// agent functions
void* agent_tabacco_paper(void* arg) {
    for (int i = 0; i < 6; i++) {
        sem_wait(&agentSem);
        printf("Agent places tobacco and paper\n");
        
        sem_post(&tabacco);
        sem_post(&paper);
        usleep(rand() % 200000);
    }
    pthread_exit(NULL);
}

void* agent_paper_match(void* arg) {
    for (int i = 0; i < 6; i++) {
        sem_wait(&agentSem);
        printf("Agent places paper and match\n");
        
        sem_post(&paper);
        sem_post(&match);
        usleep(rand() % 200000);
    }
    pthread_exit(NULL);
}

void* agent_tobacco_match(void* arg) {
    for (int i = 0; i < 6; i++) {
        sem_wait(&agentSem);
        printf("Agent places tobacco and match\n");
        
        sem_post(&tabacco);
        sem_post(&match);
        usleep(rand() % 200000);
    }
    pthread_exit(NULL);
}

void* tabacco_pusher(void* arg) {
    for (int i = 0; i < 12; i++) {
        sem_wait(&tabacco);
        sem_post(&mutex);
        if (isPaper) {
            isPaper = 0;
            sem_post(&matchSem);
        } else if (isMatch) {
            isMatch = 0;
            sem_post(&paperSem);
        } else {
            isTabacco = 1;
        }
        sem_post(&mutex);
    }
    pthread_exit(NULL);
}

void* paper_pusher(void* arg) {
    for (int i = 0; i < 12; i++) {
        sem_wait(&paper);
        sem_post(&mutex);
        if (isTabacco) {
            isTabacco = 0;
            sem_post(&matchSem);
        } else if (isMatch) {
            isMatch = 0;
            sem_post(&tabaccoSem);
        } else {
            isPaper = 1;
        }
        sem_post(&mutex);
    }
    pthread_exit(NULL);
}

void* match_pusher(void* arg) {
    for (int i = 0; i < 12; i++) {
        sem_wait(&match);
        sem_post(&mutex);
        if (isTabacco) {
            isTabacco = 0;
            sem_post(&paperSem);
        } else if (isPaper) {
            isPaper = 0;
            sem_post(&tabaccoSem);
        } else {
            isMatch = 1;
        }
        sem_post(&mutex);
    }
    pthread_exit(NULL);
}

void* paper_smoker(void* arg) {
    int smokerId = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        sem_wait(&paperSem);
        printf("Smoker %d with paper is smoking\n", smokerId);
        sem_post(&agentSem);
        usleep(rand() % 50000);
    }
    pthread_exit(NULL);
}

void* tabacco_smoker(void* arg) {
    int smokerId = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        sem_wait(&tabaccoSem);
        printf("Smoker %d with tobacco is smoking\n", smokerId);
        sem_post(&agentSem);
        usleep(rand() % 50000);
    }
    pthread_exit(NULL);
}

void* match_smoker(void* arg) {
    int smokerId = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        sem_wait(&matchSem);
        printf("Smoker %d with match is smoking\n", smokerId);
        sem_post(&agentSem);
        usleep(rand() % 50000);
    }
    pthread_exit(NULL);
}

int main() {

    //initialize semaphores
    sem_init(&tabacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);
    sem_init(&agentSem, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&tabaccoSem, 0, 0);
    sem_init(&paperSem, 0, 0);
    sem_init(&matchSem, 0, 0);
    
    //create thread
    pthread_t agent_threads[3];
    pthread_create(&agent_threads[0], NULL, agent_tabacco_paper, NULL);
    pthread_create(&agent_threads[1], NULL, agent_paper_match, NULL);
    pthread_create(&agent_threads[2], NULL, agent_tobacco_match, NULL);

    int smoker_ids[6] = {1,2,3,4,5,6};

    pthread_t smoker_threads[6];
    pthread_create(&smoker_threads[0], NULL, paper_smoker, &smoker_ids[0]);
    pthread_create(&smoker_threads[1], NULL, tabacco_smoker, &smoker_ids[1]);
    pthread_create(&smoker_threads[2], NULL, match_smoker, &smoker_ids[2]);
    pthread_create(&smoker_threads[3], NULL, paper_smoker, &smoker_ids[3]);
    pthread_create(&smoker_threads[4], NULL, tabacco_smoker, &smoker_ids[4]);
    pthread_create(&smoker_threads[5], NULL, match_smoker, &smoker_ids[5]);

    pthread_t pusher_threads[3];
    pthread_create(&pusher_threads[0], NULL, tabacco_pusher, NULL);
    pthread_create(&pusher_threads[1], NULL, paper_pusher, NULL);
    pthread_create(&pusher_threads[2], NULL, match_pusher, NULL);
    
    // join agent threads
    for(int i = 0; i < 3; i++) {
        pthread_join(agent_threads[i], NULL);
    }
    //join smoker threads
    for(int i = 0; i < 6; i++) {
        pthread_join(smoker_threads[i], NULL);
    }
    
    //join pusher threads
    for(int i = 0; i < 3; i++) {
        pthread_join(pusher_threads[i], NULL);
    }


    sem_destroy(&tabacco);
    sem_destroy(&paper);
    sem_destroy(&match);
    sem_destroy(&agentSem);
    sem_destroy(&mutex);
    sem_destroy(&tabaccoSem);
    sem_destroy(&paperSem);
    sem_destroy(&matchSem);

    return 0;
}
