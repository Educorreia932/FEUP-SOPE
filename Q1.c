#include "flagsQ.h"
#include <semaphore.h>

#include "utils.h"

static int public_fd;

static sem_t *sem;

void * handle_request(void * arg) { 


    char buffer[BUF_SIZE];
    strcpy(buffer, (char *)arg);
    
    // read(public_fd, buffer, BUF_SIZE);
    //printf("%s\n", buffer);


    char * pch;
    pch = strtok(buffer, " ,");
    
    char pid[25] , tid[25], ident[20];
    int cnt = 0;
    strcpy(ident, pch);
    while(pch != NULL) {
      
        cnt++;
    
        pch = strtok(NULL, " ,");
        if (cnt == 1){
            strcpy(pid, pch);
        }
        else if (cnt == 2) {
            strcpy(tid, pch);
        }
    }

    int i = atoi(ident);
    //open semaphore
    char sem_name[50];
    sprintf(sem_name , "/sem%d", i);
    sem = sem_open(sem_name,O_WRONLY,0600,0); 

    char privateFIFO[MAX_STR];
    sprintf(privateFIFO, "/tmp/%s.%s", pid, tid);
    
    int private_fd = open(privateFIFO, O_WRONLY);

    if (private_fd == -1 ) {
        printf("%s\n", privateFIFO);
        perror("Couldn't open private FIFO");
        pthread_exit(NULL);
    }

    printf("write %d\n", i);
    write(private_fd, "-", strlen("-"));
    close(private_fd);
    sem_post(sem);
    printf("exit %d \n", i);
    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    time_t begin = time(NULL);

    // Check Flags
    flagsQ* c = flagsQ_constructor();

    parse_flagsQ(argc, argv, c);

    if (argc != 4 /*8 in Part2*/ || c->fifoname == NULL || c->nsecs == 0) {
        //perror("Usage: Q1 <-t nsecs> [-l nplaces] [-n nthreads] fifoname");
        perror("Usage: Q1 <-t nsecs> fifoname");
        exit(1);
    }

    //Create and open public FIFO
    if(mkfifo(c->fifoname, 0660) == -1){
        perror("Failed to create fifo");
        exit(1);
    }

    if ((public_fd = open(c->fifoname,O_RDONLY)) == -1) {
        perror("Couldn't open FIFO.\n");
        exit(1);
    }
    
    //Thread creating
    pthread_t tid[MAX_THREADS];
    int t = 0;
    char line[100];

    srand(time(NULL));
    while( (time(NULL) - begin) < c->nsecs && t < MAX_THREADS){

        if (read(public_fd, &line, BUF_SIZE) > 0){

            if (pthread_create(&tid[t], NULL, handle_request, &line)){
                perror("Failed to create thread");
                exit(1);
            }
             t++;
        }
 
       
    }
    
    for(int i = 0; i < t; i++){
        pthread_join(tid[i], NULL);
    }
    
    //Close and Delete FIFO
    if(close(public_fd) == -1){
        perror("Failed closing fifo");
        exit(1);
    }

    if(unlink(c->fifoname) == -1){
        perror("Failed to delete FIFO");
        exit(1);
    }

    return 0;
}