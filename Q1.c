#include "flagsQ.h"
#include <semaphore.h>

#include "utils.h"

static int public_fd;

extern sem_t mutex;
void * handle_request(void * arg) { 
    
    char buffer[BUF_SIZE];
    strcpy(buffer, (char *)arg);
    
    // read(public_fd, buffer, BUF_SIZE);
    printf("%s\n", buffer);


    char * pch;
    pch = strtok(buffer, " ,");
    
    char pid[25] , tid[25];
    int cnt = 0;
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

    char privateFIFO[MAX_STR];
    sprintf(privateFIFO, "/tmp/%s.%s", pid, tid);
    
    int private_fd = open(privateFIFO, O_WRONLY);

    if (private_fd == -1 ) {
        printf("%s\n", privateFIFO);
        perror("Couldn't open private FIFO");
        pthread_exit(NULL);
    }

    write(private_fd, "-", strlen("-"));


    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    time_t begin = time(NULL);

    // Check Flags
    flagsQ* c = flagsQ_constructor();

    //open semaphore
    sem_t *sem;
    sem = sem_open("sem1",O_CREAT,0600,0); 
    
    if(sem == SEM_FAILED)   {     
        perror("WRITER failure in sem_open()");     
        exit(4);   
    }

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
        }
 
        t++;
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