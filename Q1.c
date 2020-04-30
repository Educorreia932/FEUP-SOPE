#include "flagsQ.h"

#include "utils.h"

static int public_fd;


void * handle_request(void * arg) { 

    //Save request

    char buffer[BUF_SIZE];
    strcpy(buffer, (char *)arg);

    //Parse request
    char pid[25] , tid[25];
    char *array[3];
    parse_request(buffer, array, 3);

    strcpy(pid, array[1]);
    strcpy(tid, array[2]);
    free(arg);

    //OPEN Private Fifo

    char privateFIFO[BUF_SIZE];
    int private_fd;
    sprintf(privateFIFO, "/tmp/%s.%s", pid, tid);
    
    if ((private_fd = open(privateFIFO, O_WRONLY)) == -1) {
        perror("[SERV] Couldn't open private FIFO.\n");
        exit(1);
    }

    //WRITE TO FIFO
    int n = 0;
    while ((n = write(private_fd, "-", strlen("-")+1)) == 0){

    }
    if (n < 0){
        perror("[SERV] Couldn't write to ptivate FIFO");
        pthread_exit(NULL);
    }
    
    
    //CLOSE FIFO 
    if(close(private_fd)){
        perror("Failed to close private fifo");
        exit(1);
    }


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

            pthread_detach(tid[t]);
            t++;
        }
 
       
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

    pthread_exit(NULL);
}