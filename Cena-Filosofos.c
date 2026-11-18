#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

void *filosofo(void *arg);
void tomar_tenedores(int i);
void soltar_tenedores(int i);
void probar(int i);
void pensar(int i);
void comer(int i);
void escribir_log(const char *accion, int id);

int estado[N];
sem_t mutex;
sem_t s[N];
FILE *archivo_log;

int main() {
    pthread_t hilos[N];
    int ids[N];
    archivo_log = fopen("Cena de los filosofos.txt", "w");
    if (archivo_log == NULL) {
        perror("Error al crear el archivo");
        return EXIT_FAILURE;
    }

    sem_init(&mutex, 0, 1);
    for (int i = 0; i < N; i++) {
        sem_init(&s[i], 0, 0);
        estado[i] = THINKING;
        ids[i] = i;
    }

    for (int i = 0; i < N; i++) {
        pthread_create(&hilos[i], NULL, filosofo, (void *)&ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }
    sem_destroy(&mutex);
    for (int i = 0; i < N; i++) {
        sem_destroy(&s[i]);
    }
    fclose(archivo_log);

    return 0;
}

void *filosofo(void *arg) {
    int id = *((int *)arg);
    while (1) {
        pensar(id);
        tomar_tenedores(id);
        comer(id);
        soltar_tenedores(id);
    }
    return NULL;
}

void tomar_tenedores(int i) {
    sem_wait(&mutex);
    estado[i] = HUNGRY;
    escribir_log("tiene hambre", i);
    probar(i);
    sem_post(&mutex);
    sem_wait(&s[i]);
}

void soltar_tenedores(int i) {
    sem_wait(&mutex);
    estado[i] = THINKING;
    escribir_log("deja de comer y piensa", i);
    probar((i + N - 1) % N);
    probar((i + 1) % N);
    sem_post(&mutex);
}

void probar(int i) {
    if (estado[i] == HUNGRY &&
        estado[(i + N - 1) % N] != EATING &&
        estado[(i + 1) % N] != EATING) {
        estado[i] = EATING;
        escribir_log("toma los tenedores y COME", i);
        sem_post(&s[i]);
    }
}

void pensar(int i) {
    escribir_log("esta pensando", i);
    sleep(1);
}

void comer(int i) {
    sleep(2);
}

void escribir_log(const char *accion, int id) {
    printf("Filosofo %d %s.\n", id, accion); 
    fprintf(archivo_log, "Filosofo %d %s.\n", id, accion);
    fflush(archivo_log); 
}
