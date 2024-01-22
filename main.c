#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
    int maxSize;
    pthread_mutex_t mutex;
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;
} ThreadSafeList;

void put(ThreadSafeList *list, void *el) {
    pthread_mutex_lock(&list->mutex);

    while (list->size >= list->maxSize) {
        pthread_cond_wait(&list->notFull, &list->mutex);
    }

    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->data = el;
    newNode->next = NULL;

    if (list->tail == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }

    list->size++;

    pthread_cond_signal(&list->notEmpty);

    pthread_mutex_unlock(&list->mutex);
}

void *get(ThreadSafeList *list) {
    pthread_mutex_lock(&list->mutex);

    while (list->size == 0) {
        pthread_cond_wait(&list->notEmpty, &list->mutex);
    }

    Node *nodeToRemove = list->head;
    void *data = nodeToRemove->data;

    list->head = nodeToRemove->next;
    free(nodeToRemove);

    list->size--;

    pthread_cond_signal(&list->notFull);

    pthread_mutex_unlock(&list->mutex);

    return data;
}

int removeElement(ThreadSafeList *list, void *el) {
    pthread_mutex_lock(&list->mutex);

    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL) {
        if (current->data == el) {
            if (previous == NULL) {
                list->head = current->next;
                if (list->head == NULL) {
                    list->tail = NULL;
                }
            } else {
                previous->next = current->next;
                if (previous->next == NULL) {
                    list->tail = previous;
                }
            }

            free(current);
            list->size--;

            pthread_mutex_unlock(&list->mutex);
            return 1;
        }

        previous = current;
        current = current->next;
    }

    pthread_mutex_unlock(&list->mutex);
    return 0;
}

int getCount(ThreadSafeList *list) {
    pthread_mutex_lock(&list->mutex);
    int count = list->size;
    pthread_mutex_unlock(&list->mutex);
    return count;
}

void setMaxSize(ThreadSafeList *list, int newSize) {
    pthread_mutex_lock(&list->mutex);
    list->maxSize = newSize;
    pthread_mutex_unlock(&list->mutex);
    printf("List max size set to %d.\n", newSize);
}

void *testFunction(void *arg) {
    ThreadSafeList *list = (ThreadSafeList *) arg;

    // put test
    for (int i = 0; i <= 4; ++i) {
        put(list, (void *) i);
        printf("Thread %lu added %d to list.\n", pthread_self(), i);
        printf("Current list size:  %d\n", getCount(list));
        sleep(1);
    }

    // remove test
    for (int i = 0; i < 2; ++i) {
        int elementToRemove = i * 2;
        int result = removeElement(list, (void *) elementToRemove);

        if (result) {
            printf("Thread %lu removed %d from list.\n", pthread_self(), elementToRemove);
            printf("Current list size:  %d\n", getCount(list));
        } else {
            printf("Thread %lu not found Element %d.", pthread_self(), elementToRemove);
            printf("Current list size:  %d\n", getCount(list));
        }

        sleep(1);
    }

    // get test
    for (int i = 0; i < 2; ++i) {
        void *data = get(list);
        printf("Thread %lu removed %d from list.\n", pthread_self(), (int) data);
        printf("Current list size:  %d\n", getCount(list));
        sleep(1);
    }

    setMaxSize(list, 10);

    // another remove test
    int nonExistentElement = 999;
    int resultNonExistent = removeElement(list, (void *) nonExistentElement);

    if (resultNonExistent) {
        printf("Thread %lu removed %d from list.\n", pthread_self(), nonExistentElement);
        printf("Current list size:  %d\n", getCount(list));
    } else {
        printf("Thread %lu not found Element %d.\n", pthread_self(), nonExistentElement);
        printf("Current list size:  %d\n", getCount(list));
    }

    return NULL;
}

int main() {
    ThreadSafeList list;
    list.head = NULL;
    list.tail = NULL;
    list.size = 0;

    int maxsize;
    printf("Project: Limited capacity list\n");
    sleep(1);
    printf("Author: Radosław Bujny 155950\n");
    sleep(1);
    printf("Enter a list max size: \n");
    scanf("%d", &maxsize);
    while (maxsize <= 0) {
        printf("List max size must be positive. Try agian: \n");
        scanf("%d", &maxsize);
    }
    setMaxSize(&list, maxsize);

    pthread_mutex_init(&list.mutex, NULL);
    pthread_cond_init(&list.notFull, NULL);
    pthread_cond_init(&list.notEmpty, NULL);

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, testFunction, (void *) &list);
    pthread_create(&thread2, NULL, testFunction, (void *) &list);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&list.mutex);
    pthread_cond_destroy(&list.notFull);
    pthread_cond_destroy(&list.notEmpty);

    return 0;
}