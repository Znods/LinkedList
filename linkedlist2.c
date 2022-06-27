#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/resource.h>

#define DEBUG

#define MAX_MEMORY 5e+9
#define HEAP_JUNK 1024

typedef struct node{
    int data;
    struct node * next;
}node;

typedef node * Link;

int randomLinks(Link *head, int nlinks);
void insertAtFirst(Link *head, int val);
void insertAtLast(Link *head, int val);
void insertAtPos(Link *head, int val, int pos);
int delete(Link *head, int val);
int update(Link *head, int val, int pos);
int search(Link *head, int val);
void printList(Link head);
int isEmpty(Link head);

int main(){
    Link head = NULL;
    struct rlimit old_limit, new_limit, lim;
    int choice = 0, val = 0, pos = 0, check = 0;
    long soft = 0;
    char *options = "Insert at first = 1\nInsert at last = 2\nInsert at position = 3\nDelete = 4\nUpdate = 5\nGen Random List = 6\nPrint list = 7\nShow options = 8\nChange Soft Limit = 9\nEXIT = 10\n";
    
    if(getrlimit(RLIMIT_DATA, &old_limit) == 0){
        printf("Old limits -> \nHard limit = %ld\nSoft limit = %ld\n\n", old_limit.rlim_max, old_limit.rlim_cur);
    } else {
        fprintf(stderr, "%s\n", strerror(errno));
    }

    lim.rlim_cur = MAX_MEMORY;
    lim.rlim_max = -1;
    
    if(setrlimit(RLIMIT_DATA, &lim) == -1){
        fprintf(stderr, "%s\n", strerror(errno));
    }

    if(getrlimit(RLIMIT_DATA, &new_limit) == 0){
        printf("New limits -> \nHard limit = %ld\nSoft limit = %ld\n\n", new_limit.rlim_max, new_limit.rlim_cur);
    } else {
        fprintf(stderr, "%s\n", strerror(errno));
    }

    puts(options);
    printf("? ");
    scanf("%d", &choice);
    while(choice != 10){
        switch(choice){
            case 1:
                printf("\nInsert: ");
                scanf("%d", &val);
                insertAtFirst(&head, val);
                printList(head);
                break;
            case 2:
                printf("\nInsert: ");
                scanf("%d", &val);
                insertAtLast(&head, val);
                printList(head);
                break;
            case 3:
                printf("\nInsert: ");
                scanf("%d", &val);
                printf("\nPos: ");
                scanf("%d", &pos);
                insertAtPos(&head, val, pos);
                printList(head);
                break;
            case 4:
                if(!isEmpty(head)){
                    printf("\nDelete: ");
                    scanf("%d", &val);
                    if( (check = delete(&head, val)) != NULL){
                        printf("\ndeleted %d, success %d\n", val, check);
                    } else {
                        printf("\nfailed to delete %d, failed %d\n", val, check);
                    }
                    printList(head);
                } else {
                    printf("List is empty.\n");
                }
                break;
            case 5:
                if(!isEmpty(head)){
                    printf("\nUpdate: ");
                    scanf("%d", &val);
                    printf("\nPos: ");
                    scanf("%d", &pos);
                    if((check = update(&head, val, pos)) != NULL){
                        printf("\nUpdated %c at pos %d, success %d\n", val, pos, check);
                    } else {
                        printf("\nFailed %c at pos %d, failed %d\n", val, pos, check);
                    }
                } else {
                    printf("List is empty.\n");
                }
                printList(head);
                break;
            case 6:
                printf("Generate Random Linked list\n"
                       "How many random links?: ");
                scanf("%d", &val);
                if((check = randomLinks(&head, val) != NULL)){
                    printf("Linked list created. success %d\n", check);
                } else {
                    printf("Failed to create list. failed %d\n", check);
                }
                printList(head);
                break;
            case 7:
                printList(head);
                break;
            case 8:
                puts(options);
                break;
            case 9:
                printf("Bytes: ");
                scanf("%ld", &soft);
                lim.rlim_cur = soft;
                if(setrlimit(RLIMIT_DATA, &lim) == -1){
                    fprintf(stderr, "%s\n", strerror(errno));
                }
                if(getrlimit(RLIMIT_DATA, &new_limit) == 0){
                    printf("New limits -> \nHard limit = %ld\nSoft limit = %ld\n\n", new_limit.rlim_max, new_limit.rlim_cur);
                } else {
                    fprintf(stderr, "%s\n", strerror(errno));
                }
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
        printf("\n? ");
        scanf("%d", &choice);
    }
    free(head);
    return 0;
}

int randomLinks(Link *head, int nlinks){
    Link new = NULL;
    srand(time(0));
    for(int i = 0; i < nlinks; i++){
        #ifdef DEBUG
        if((new = (Link)malloc(sizeof(node) * HEAP_JUNK)) == NULL){
        #else
        if((new = (Link)malloc(sizeof(node))) == NULL){
        #endif
            printf("Out of memory\n");
            return NULL;
        }
        new->data = rand() % 55 + 1;
        new->next = *head;
        *head = new;
        #ifdef DEBUG
            printf("Adding -> %d\n", new->data);
        #endif
    }
    return 1;
}
int update(Link *head, int val, int pos){
    Link cur = *head;
    Link prev = NULL;
    if(pos == 0){
        (*head)->data = val;
        return 1;
    }
    for(int i = 0; i < pos; i++){
        prev = cur;
        cur = cur->next;
        if(cur == NULL){
            prev->data = val;
            prev->next = NULL;
            return 1;
        }
    }
    prev->data = val;
    return 1;
}
int delete(Link *head, int val){
    Link cur = *head;
    Link prev = NULL;
    Link tmp = NULL;

    if((*head)->data == val){
        tmp = *head;
        *head = (*head)->next;
        free(tmp);
        return 1;
    }
    while(cur->data != val && cur != NULL){
        prev = cur;
        cur = cur->next; 
    }
    if(cur == NULL){
        return NULL;
    }
    tmp = cur; 
    prev->next = cur->next;
    free(tmp);
    return 1;
}
void insertAtPos(Link *head, int val, int pos){
    int size = 0;
    Link prev = NULL;
    Link cur = *head;
    Link tmp = *head;
    Link new = (Link)malloc(sizeof(node));
    if(new == NULL){
        printf("Out of memory\n");
    } else {
        if(cur == NULL || pos == 0){
            new->data = val;
            new->next = *head;
            *head = new;
            return;
        }
        // find length
        while(tmp->next != NULL){
            size++;
            tmp = tmp->next;
        }
        if(pos > size){
            printf("Out of bounds!\n");
            return;
        } else if(pos == size-1){
            while(cur->next != NULL){
                cur = cur->next;
            }
            cur->next = (Link)malloc(sizeof(node));
            cur->next->data = val;
            cur->next->next = NULL;
        } else {
            for(int i = 0; i < pos-1; i++){
                prev = cur;
                cur = cur->next; // find pos
            }
            new->data = val;

            prev->next = new; // insert val
            new->next = cur;
        }

    }
}
void insertAtLast(Link *head, int val){
    Link new = (Link)malloc(sizeof(node));
    Link cur = *head;
    if(new == NULL){
        printf("Out of memory\n");
    } else {
        if(cur == NULL){
            new->data = val;
            new->next = *head;
            *head = new;
        } else {
            while(cur->next != NULL){ // loop till end
                cur = cur->next;
            }
            cur->next = (Link)malloc(sizeof(node)); // allocate memory for next ptr
            cur->next->data = val;
            cur->next->next = NULL;
        }
    }
}
void insertAtFirst(Link *head, int val){
    Link new = (Link)malloc(sizeof(node));
    if(new == NULL){
        printf("Out of memory.\n");
    } else {
        new->data = val; 
        new->next = *head; 
        *head = new; 
    }
}
void printList(Link head){
    if(head == NULL){
        printf("Nothing in the list.\n");
    } else {
        printf("List: \n");
        while(head != NULL){
            printf("%d --> ", head->data);
            head = head->next;
        }
        printf("NULL");
    }
}
int isEmpty(Link head){
    return head == NULL;
}