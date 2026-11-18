#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define N 5
#define NUM_PRODUCTORES 2
#define NUM_CONSUMIDORES 2
#define ITEMS_A_PRODUCIR 10

int buffer[N];
int front = 0;
int rear = 0;

sem_t empty;
sem_t full;
sem_t mutexP;
sem_t mutexC;

FILE *archivo;

void* productor(void* arg);
void* consumidor(void* arg);

int main() {
    pthread_t prod_hilos[NUM_PRODUCTORES];
    pthread_t cons_hilos[NUM_CONSUMIDORES];
    int prod_ids[NUM_PRODUCTORES];
    int cons_ids[NUM_CONSUMIDORES];

    archivo = fopen("Productor-Consumidor.txt", "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo.\n");
        return 1;
    }

    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    sem_init(&mutexP, 0, 1);
    sem_init(&mutexC, 0, 1);
    srand(time(NULL));

    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prod_hilos[i], NULL, productor, &prod_ids[i]);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        cons_ids[i] = i + 1;
        pthread_create(&cons_hilos[i], NULL, consumidor, &cons_ids[i]);
    }
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(prod_hilos[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(cons_hilos[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutexP);
    sem_destroy(&mutexC);
    fprintf(archivo, " Fin del programa \n");
    fclose(archivo);

    printf("Ejecucion terminada. Revisa el archivo 'Productor-Consumidor.txt'\n");
    return 0;
}

void sleep_random() {
    usleep((rand() % 1000) * 1000);
}

void* productor(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < ITEMS_A_PRODUCIR; i++) {
        int data = rand() % 100;
        fprintf(archivo, "Productor %d Producido: %d\n", id, data);
        sem_wait(&empty);
        sem_wait(&mutexP);

        buffer[rear] = data;
        fprintf(archivo, "\t> Productor %d Insertado en pos %d\n", id, rear);
        rear = (rear + 1) % N;

        sem_post(&mutexP);
        sem_post(&full);

        sleep_random();
    }
    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < (NUM_PRODUCTORES * ITEMS_A_PRODUCIR) / NUM_CONSUMIDORES; i++){
        sem_wait(&full);
        sem_wait(&mutexC);

        int result = buffer[front];
        fprintf(archivo, "\t< Consumidor %d Leido de pos %d: %d\n", id, front, result);
        front = (front + 1) % N;

        sem_post(&mutexC);
        sem_post(&empty);

        fprintf(archivo, "Consumidor %d Consumido: %d\n", id, result);
        sleep_random();
    }
    return NULL;
}
