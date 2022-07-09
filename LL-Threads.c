#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>

#define error(x) fprintf(stderr, "\n%s\n", x)

/*
Fast Linked List
multi threaded display :)
*/

typedef struct node{
    int data;
    struct node *next;
}node_t;

typedef node_t *Link;

void insert(Link *);
void pList(Link);
void *display(void *);

volatile long long i = 0, j = 0;

int main(void){
    struct rlimit old, new;
    clock_t start = 0, end = 0;
    pthread_t thread = 0, tid = 0;
    pid_t pid = 0;
    int c = 0;
    Link head = NULL;

    /* Get & Set HEAP Limit */
    new.rlim_cur = 1e+9;
    new.rlim_max = 2e+9;

    pid = getpid(); 
    tid = pthread_self();

    if(getrlimit(RLIMIT_DATA, &old) != -1){
        printf("Old limits: soft=%lld, hard=%lld\n", (long long)old.rlim_cur, (long long)old.rlim_max);
    } else {
        error(strerror(errno));
        return 1;
    }
    if(setrlimit(RLIMIT_DATA, &new) == -1){
        error(strerror(errno));
        return 1;
    } 
     if(getrlimit(RLIMIT_DATA, &old) != -1){
        printf("New limits: soft=%lld, hard=%lld\n", (long long)old.rlim_cur, (long long)old.rlim_max);
    } else {
        error(strerror(errno));
        return 1;
    }
    
    srand(time(NULL));
    printf("? ");
    scanf("%d", &c);
    
    /* Main Logic Loop */
    while(c != 3){
        switch(c){
            case 1:
                printf("\nHow many nodes? ");
                scanf("%d", &i);
                pthread_create(&thread, NULL, display, NULL); // Create new thread for display()
                start = clock();
                    insert(&head);
                end = clock();
                pthread_join(thread, NULL); // Wait for main() thread
                printf("\nInserted %d nodes in %f cpu_time, tid: %ld\n", i, (double)(end-start)/CLOCKS_PER_SEC, tid);
                i = 0; // Reset Global Variable
                break;
            case 2:
                pList(head);
                break;
            default:
                printf("Invalid option.\n\n");
                break;
        }
        printf("\n? ");
        scanf("%d", &c);
    }
    pthread_exit(NULL);
    free(head);
    return 0;
}

void *display(void *v){
    while(j < i){ // No 'critical sections' of code
        printf("\rAdding... %d/%d, tid: %ld", j, i, pthread_self());
        fflush(stdout);
    }
}

void insert(Link *head){
    Link new = NULL;
    for(j = 0; j < i; j++){
        if((new = (Link)malloc(sizeof(node_t))) != NULL){
            new->data = rand() % 127;
            new->next = *head;
            *head = new;
        } else {
            error(strerror(errno));
            exit(EXIT_FAILURE);
        }
    } 
}

void pList(Link head){
    char buf[10];
    if(head != NULL){
        while(head != NULL){
            sprintf(buf, "%d --> ", head->data);
            write(1, buf, sizeof(buf));
            head = head->next;
        }
        printf("NULL\n");
    } else {
        printf("Nothing to print.\n\n");
    }
}
