#include "flagsQ.h"
#include "utils.h"
#include <stdbool.h>

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

    while ((n = write(private_fd, "-", strlen("-")+1)) == 0)
        continue;

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
    // Check Flags
    flagsQ* c = flagsQ_constructor();

    parse_flagsQ(argc, argv, c);

    //BEGIN TIME COUNT
    time_t begin = time(NULL);

    if (argc != 4 /*8 in Part2*/ || c->fifoname == NULL || c->nsecs == 0) {
        //perror("Usage: Q1 <-t nsecs> [-l nplaces] [-n nthreads] fifoname");
        perror("Usage: Q1 <-t nsecs> fifoname");
        exit(1);
    }

    // Create and open public FIFO
    if (mkfifo(c->fifoname, 0660) == -1){
        perror("Failed to create fifo");
        exit(1);
    }

    if ((public_fd = open(c->fifoname,O_RDONLY)) == -1) {
        perror("Couldn't open FIFO.\n");
        exit(1);
    }

    // Thread creating

    pthread_t tid;
    char* line; 
    bool processing = true;
    time_t time_took;
    int n;

    while (((time_took = time(NULL) - begin) < c->nsecs) && processing) {
        line = (char*) malloc(BUF_SIZE);
        
        n = read(public_fd, line, BUF_SIZE);

        if (n > 0) {

            if (pthread_create(&tid, NULL, handle_request, (void *) line)){
                perror("[SERVER] Failed to create thread");
                exit(1);
            }

            pthread_detach(tid);
        }

        else if (time_took && n == 0) {
            free(line);
            processing = false;
        }

        else {
            perror("[SERVER] Failed to read public FIFO.\n");
            free(line);
            exit(1); // TODO: Close FIFO
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