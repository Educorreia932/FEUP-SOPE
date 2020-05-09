#include <stdbool.h>

#include "flagsQ.h"
#include "utils.h"
#include "log.h"
#include <semaphore.h>

static int public_fd;
static bool wc_open;

static sem_t *mutex;

void * handle_request(void* arg) { 

    //Save request
    message_t* message = (message_t*) arg;


    //OPEN Private Fifo

    char privateFIFO[BUF_SIZE];
    int private_fd;
    sprintf(privateFIFO, "/tmp/%d.%lu", message->pid, message->tid);
    
    if ((private_fd = open(privateFIFO, O_WRONLY)) == -1) {
        free(arg);
        perror("[SERVER] Couldn't open private FIFO.\n");
        exit(1);
    }

    // WRITE TO FIFO

    enum Operation op;

    if(wc_open){
        op = ENTER;
        message->pl = message->i;
    }
    
    else {
        op = LATE;
        message->dur = -1;
    }

    int n = write(private_fd, message, sizeof(message_t));
    
    if (n < 0){
        free(arg);
        perror("[SERV] Couldn't write to private FIFO");
        pthread_exit(NULL);
    }
    
    else if(n > 0){
        print_log(message, op);
    }
    
    else { // n = 0
        op = GAVUP;
        print_log(message, op);
    }

    //USE BATHROOM

    if(op == ENTER){
        usleep(message->dur);
        print_log(message, TIMUP);
    }
    
    // CLOSE FIFO 
    if(close(private_fd)){
        free(arg);
        perror("Failed to close private fifo");
        exit(1);
    }

    free(arg);

    if (sem_post(mutex) == -1) {
        perror("[SERVER] Failed to increment semaphore");
        exit(1);
    }

    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    // Check Flags
    flagsQ* c = flagsQ_constructor();

    mutex = (sem_t*) malloc(sizeof(sem_t));

    parse_flagsQ(argc, argv, c);

    //print_flagsQ(c);
    if (sem_init(mutex, NOT_SHARED, c->nthreads) == -1){
        perror("Couldn't initiate semaphore.");
        exit(1);
    }

    //BEGIN TIME COUNT
    time_t begin = time(NULL);

    if (argc != 8  || c->fifoname == NULL || c->nsecs == 0) {
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
        free(c);
        perror("Couldn't open FIFO.\n");
        exit(1);
    }

    // Thread creating
    pthread_t tid;
    message_t* msg;
    int n;
    bool processing = true;

    while (processing) {
        
        msg = (message_t*) malloc(sizeof(message_t));

        wc_open = ((time(NULL) - begin) < c->nsecs);

            n = read(public_fd, msg, sizeof(message_t));

            if (n > 0) {

                print_log(msg, RECVD);

                if (sem_wait(mutex) == -1){ //wait for available threads
                    perror("[SERVER] Failed to decrement semaphore");
                    exit(1);
                } 

                if (pthread_create(&tid, NULL, handle_request, (void*) msg)){
                    free(msg);
                    free(c);
                    perror("[SERVER] Failed to create thread");
                    exit(1);
                }

                c->nthreads--;

                pthread_detach(tid);
            }
            else if (n == 0) {
                free(msg);
                processing = false;
            }
            else {
                perror("[SERVER] Failed to read public FIFO.\n");            
                free(msg);
                free(c);
                close(public_fd);
                exit(1); // TODO: Close FIFO
            }
        
   

    }

    //Close and Delete FIFO
    if(close(public_fd) == -1){
        free(c);
        perror("[SERVER] Failed closing fifo");
        exit(1);
    }

    if(unlink(c->fifoname) == -1){
        free(c);
        perror("[SERVER] Failed to delete FIFO");
        exit(1);
    }

    free(c);
    free(mutex);

    pthread_exit(NULL);
}