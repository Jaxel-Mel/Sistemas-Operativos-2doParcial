#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define LIMITE_INFERIOR   1
#define LIMITE_SUPERIOR   10000
#define INICIO_CRITICO    4000
#define FIN_CRITICO       6000
#define NUM_HILOS         2
#define ARCHIVO_SALIDA    "Semaforos.txt"

sem_t  mutex;
FILE  *archivo;

void  abrir_archivo(void);
void  cerrar_archivo(void);
void  inicializar_semaforo(void);
void  destruir_semaforo(void);
void  crear_hilos(pthread_t hilos[], int ids[]);
void  esperar_hilos(pthread_t hilos[]);
void *cuenta(void *arg);
void  entrar_seccion_critica(int id, int i);
void  salir_seccion_critica(int id, int i);

int main(void)
{
    pthread_t hilos[NUM_HILOS];
    int       ids[NUM_HILOS] = {1, 2};

    abrir_archivo();
    inicializar_semaforo();
    crear_hilos(hilos, ids);
    esperar_hilos(hilos);
    destruir_semaforo();
    cerrar_archivo();

    printf("Programa finalizado. Revisa '%s'.\n", ARCHIVO_SALIDA);
    return EXIT_SUCCESS;
}

void abrir_archivo(void)
{
    archivo = fopen(ARCHIVO_SALIDA, "w");
    if (archivo == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
}

void cerrar_archivo(void)
{
    if (archivo != NULL)
        fclose(archivo);
}

/*Inicializa el semáforo binario en 1*/
void inicializar_semaforo(void)
{
    if (sem_init(&mutex, 0, 1) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}

void destruir_semaforo(void)
{
    sem_destroy(&mutex);
}

void crear_hilos(pthread_t hilos[], int ids[])
{
    for (int i = 0; i < NUM_HILOS; i++) {
        if (pthread_create(&hilos[i], NULL, cuenta, &ids[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
}

void esperar_hilos(pthread_t hilos[])
{
    for (int i = 0; i < NUM_HILOS; i++)
        pthread_join(hilos[i], NULL);
}

void *cuenta(void *arg)
{
    int id = *(int *)arg;

    for (int i = LIMITE_INFERIOR; i <= LIMITE_SUPERIOR; i++) {

        if (i == INICIO_CRITICO)
            entrar_seccion_critica(id, i);

        fprintf(archivo, "Hilo %d: %d\n", id, i);

        if (i == FIN_CRITICO)
            salir_seccion_critica(id, i);
    }

    return NULL;
}

/*Adquiere el semáforo*/
void entrar_seccion_critica(int id, int i)
{
    sem_wait(&mutex);
    fprintf(archivo, "Hilo %d > ENTRA a sección crítica en i=%d\n", id, i);
}

/*Libera el semáforo*/
void salir_seccion_critica(int id, int i)
{
    fprintf(archivo, "Hilo %d < SALE de sección crítica en i=%d\n", id, i);
    sem_post(&mutex);
}
