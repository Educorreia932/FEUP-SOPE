#include <stdbool.h>

#include "flagsQ.h"
#include "utils.h"
#include "log.h"

static int public_fd;
static bool wc_open;


void * handle_request(void* arg) { 
    //Save request
    message_t* message = (message_t*) arg;

    //OPEN Private Fifo

    char privateFIFO[BUF_SIZE];
    int private_fd;
    sprintf(privateFIFO, "/tmp/%d.%lu", message->pid, message->tid);
    
    if ((private_fd = open(privateFIFO, O_WRONLY)) == -1) {
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
        perror("[SERV] Couldn't write to private FIFO");
        pthread_exit(NULL);
    }
    else if(n > 0){
        print_log(message, op);
    }

    //USE BATHROOM

    if(op == ENTER){
        usleep(message->dur);
        print_log(message, TIMUP);
    }
    
    // CLOSE FIFO 
    if(close(private_fd)){
        perror("Failed to close private fifo");
        exit(1);
    }

    free(arg);

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
    message_t* msg; 
    int n;
    bool processing = true;

    while ( (wc_open = ((time(NULL) - begin) < c->nsecs)) && processing) {
        msg = (message_t*) malloc(sizeof(message_t));

        n = read(public_fd, msg, sizeof(message_t));

        if (n > 0) {
            print_log(msg, RECVD);

            if (pthread_create(&tid, NULL, handle_request, (void*) msg)){
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
            exit(1); // TODO: Close FIFO
        }

    }

    //Close and Delete FIFO
    if(close(public_fd) == -1){
        perror("[SERVER] Failed closing fifo");
        exit(1);
    }

    if(unlink(c->fifoname) == -1){
        perror("[SERVER] Failed to delete FIFO");
        exit(1);
    }

    free(flags);
    pthread_exit(NULL);
}