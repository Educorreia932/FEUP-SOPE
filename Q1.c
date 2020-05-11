#include <stdbool.h>

#include "flagsQ.h"
#include "utils.h"
#include "log.h"
#include <semaphore.h>

static int public_fd;
static bool wc_open;

static sem_t *num_threads, *avail_places, *can_check;
static bool *occupied;

static flagsQ *c;

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

    
    int place;
    if(wc_open){
        op = ENTER;
    
        sem_wait(avail_places); 

        sem_wait(can_check);

        
        for (int i = 0; i < c->nplaces; i++) {
                if (!occupied[i]) {
                    occupied[i] = true;
                    place = i;
                    break;
                }
        }
        sem_post(can_check);

        message->pl = place;
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

        

        if (sem_post(avail_places) == -1) {
            perror("[SERVER] Failed to increment semaphore");
            exit(1);
        }

        sem_wait(can_check);
        occupied[place] = false;
        sem_post(can_check);
    }
    
    // CLOSE FIFO 
    if(close(private_fd)){
        free(arg);
        perror("Failed to close private fifo");
        exit(1);
    }

    free(arg);

    if (sem_post(num_threads) == -1) {
        perror("[SERVER] Failed to increment semaphore");
        exit(1);
    }

 
   

    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    // Check Flags
    c = flagsQ_constructor();

    num_threads = (sem_t*) malloc(sizeof(sem_t));
    avail_places = (sem_t*) malloc(sizeof(sem_t));
    can_check = (sem_t*) malloc(sizeof(sem_t));

    parse_flagsQ(argc, argv, c);

    //print_flagsQ(c);
    if (sem_init(num_threads, NOT_SHARED, c->nthreads) == -1){
        perror("Couldn't initiate semaphore.");
        exit(1);
    }

    if (sem_init(avail_places, NOT_SHARED, c->nplaces) == -1){
        perror("Couldn't initiate semaphore.");
        exit(1);
    }

    if (sem_init(can_check, NOT_SHARED, 1) == -1){
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

    occupied = (bool*) calloc(c->nplaces, sizeof(bool));

    while (processing) {

        msg = (message_t*) malloc(sizeof(message_t));

        wc_open = ((time(NULL) - begin) < c->nsecs);

        n = read(public_fd, msg, sizeof(message_t));

        if (n > 0) {

            print_log(msg, RECVD);

            if (sem_wait(num_threads) == -1){ //wait for available threads
                perror("[SERVER] Failed to decrement semaphore");
                exit(1);
            } 

            if (pthread_create(&tid, NULL, handle_request, (void*) msg)){
                free(msg);
                free(c);
                perror("[SERVER] Failed to create thread");
                exit(1);
            }


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
    free(num_threads);
    free(avail_places);
    free(can_check);

    pthread_exit(NULL);
}