#include "flagsQ.h"
#include <semaphore.h>

#include "utils.h"

static int public_fd;

//static sem_t *sem;

void * handle_request(void * arg) { 

    //Save request

    char buffer[BUF_SIZE];
    strcpy(buffer, (char *)arg);

    //Parse request

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

    printf("Atendendo %d\n", i);

    //OPEN semaphore
/*
    char sem_name[50];
    sprintf(sem_name , "/sem%d", i);
    sem = sem_open(sem_name,O_WRONLY); 


    if(sem == SEM_FAILED)   {     
        perror("WRITER failure in sem_open()");    
        exit(1);   
    }
*/
    //OPEN Private Fifo

    char privateFIFO[BUF_SIZE];
    int private_fd;
    sprintf(privateFIFO, "/tmp/%s.%s", pid, tid);
    
    if ((private_fd = open(privateFIFO, O_WRONLY)) == -1) {
        perror("Couldn't open private FIFO.\n");
        exit(1);
    }

    //WRITE TO FIFO

    write(private_fd, "-", strlen("-"));
    printf("write %d\n", i);

 /*   
    if(sem_post(sem) < 0){
        perror("Failed to post sem");
        exit(1);
    }
    
    //WAIT CLIENT TO READ
    if(sem_wait(sem) < 0){
        perror("Failed to wait sem");
        exit(1);
    }
*/
    //CLOSE FIFO 
    if(close(private_fd)){
        perror("Failed to close private fifo");
        exit(1);
    }
/*
    //CLOSE SEM
    if(sem_close(sem)){
        perror("Failed to close sem");
        exit(1);
    }
*/

    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    //BEGIN TIME COUNT
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
    char * line; 

    while( (time(NULL) - begin) < c->nsecs && t < MAX_THREADS){
        line = (char * )malloc(BUF_SIZE);
        
        if (read(public_fd, line, BUF_SIZE) > 0){
            if (pthread_create(&tid[t], NULL, handle_request, (void *) line)){
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