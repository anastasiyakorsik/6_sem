#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

typedef struct ThreadsData {
    int id;
    double a;
    double b;
    int step;
    double result; // результат интегрирования
    double time;
} ThreadsData;

double Function(double x) {
    return sin(1./x);
}
// Формула средних прямоугольников для заданной функции
double DefiniteIntegral(double a, double b, int steps) {
    double result = 0, h = (b - a) / steps;

    for(int i = 0; i < steps; i++) {
        result += Function(a + h * (i + 0.5));
    }

    result *= h;
    return result;
}

void* ThreadFunc (void *thr_arguments) {
    struct timespec start_time, end_time;
    double elapsed_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time); // время начала выполнения вычислений

    ThreadsData *args = (ThreadsData*) thr_arguments;
    args->result = DefiniteIntegral(args->a, args->b, args->step);

    clock_gettime(CLOCK_MONOTONIC, &end_time); // время конца выполенения вычислений

    elapsed_time = end_time.tv_sec - start_time.tv_sec;
    elapsed_time += (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
    args->time = elapsed_time;
    // возвращаемое значение для pthread_join()
    return 0;
}


int main() {
    double A, B = 0;
    int integrate_iter, threads_num = 0;
    printf("Введите пределы интегрирования: \n");
    scanf_s("%lg %lg", &A, &B);
    printf("Введите кол-во итераций для посчета интеграла и количество потоков: \n");
    scanf_s("%d %d", &integrate_iter, &threads_num);

    // кол-во итераций для одного потока
    int thread_step = integrate_iter / threads_num;
    // шаг интегрирования
    double int_step = (B - A) / integrate_iter;

    int rc;

    // создаем массив наших нитей
    pthread_t threads[threads_num];
    // создаем массив аргументов
    ThreadsData args[threads_num];
    for (int i = 0; i < threads_num; ++i) {
        args[i].id = i;
        args[i].a = A + int_step * i * thread_step;
        args[i].b = A + int_step * ((i + 1) * thread_step);
        args[i].result = 0;
        args[i].step = thread_step;
    }
    // создаем нити и каждая из них считает интеграл
    for (int i = 0; i < threads_num; i++) {
        if ((rc = pthread_create(&threads[i], NULL, ThreadFunc, (void *) &args[i]))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < threads_num; i++) {
        void *returnValue;
        pthread_join(threads[i], &returnValue);
    }
    // Время выполенения для каждой нити
    for (int i = 0; i < threads_num; i++) {
        printf("Thread %d, ", args[i].id);
        printf("calculating time =  %lg sec\n", args[i].time);
    }
    printf("\n");

    // Считаем результат
    double integral = 0;
    for (int i = 0; i < threads_num; i++) {
        integral += args[i].result;
    }

    printf("Integral = %lg\n", integral);

    return 0;
}

