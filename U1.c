#include <time.h>
#include <sys/types.h>

#include "flagsU.h"
#include "log.h"

#include "utils.h"

flagsU* flags;

static int public_fd;

void* send_request(void * arg) { 
    //PRIVATEFIFO

    char privateFIFO[BUF_SIZE];
    int private_fd;
    sprintf(privateFIFO, "/tmp/%d.%lu", getpid(), pthread_self());

    if(mkfifo(privateFIFO, 0660) < 0){
        //free(arg);
        perror("[CLIENT] Failed to create fifo");
        exit(1);
    }
    
    // PREPARE MESSAGE
    message_t* message = message_constructor(*(int*) arg);

    //free(arg);

    //SEND REQUEST
    int n = write(public_fd, message, sizeof(message_t));
  
    if (n < 0) {
        //free(message);
        perror("[CLIENT] Couldn't write to public FIFO");
        pthread_exit(NULL);
    }
    
    else if (n > 0){
        print_log(message, IWANT);
    }

    //Open FIFO
    if ((private_fd = open(privateFIFO, O_RDONLY)) == -1) {
        //free(message);
        perror("[CLIENT] Couldn't open FIFO.\n");
        exit(1);
    }

    // READ
    n = read(private_fd, message, sizeof(message_t));

    if (n < 0){
        //free(message);
        perror("[CLIENT] Couldn´t read private FIFO");
        pthread_exit(NULL);
    }
    else if (n > 0){
        if(message->pl != -1)
            print_log(message,IAMIN);
        else
            print_log(message,CLOSD);
    }
    else { //n=0
        print_log(message, FAILD);
    } 

    
    // UNLINK & CLOSE PRIVATE FIFO
    if (close(private_fd) == -1) {
        //free(message);
        perror("[CLIENT] Couldn't close private FIFO");
        pthread_exit(NULL);
    }

    if (unlink(privateFIFO) == -1){
        //free(message);
        perror("[CLIENT] Failed to delete FIFO");
        exit(1);
    }

    //free(message);
    pthread_exit(NULL);
}

int main(int argc, char * argv[]){
    srand(time(0));

    // Check Flags
    flags = flagsU_constructor(); 
    
    parse_flagsU(argc, argv, flags);

    //Begin Time count
    time_t begin = time(NULL);

    if (argc != 4 || flags->fifoname == NULL || flags->nsecs == 0) {
        //free(flags);
        perror("Usage: U1 <-t nsecs> fifoname");
        exit(1);
    }

    //Open public FIFO
    int timeout = 0;     

    do {
        public_fd = open(flags->fifoname, O_WRONLY);

        if (public_fd == -1){
            timeout++;
            sleep(1);
        } 
            
    } while (public_fd == -1 && timeout < 3);

    if(timeout == 3){ //Took too long to open 
        //free(flags);
        perror("Failed to open FIFO");
        exit(1);
    } 

    // Thread creating
    pthread_t tid;
    int *thrArg, t = 0;

    while ((time(NULL) - begin) < flags->nsecs){
        thrArg = (int *) malloc(sizeof(t));
        *thrArg = t + 1; // Request number
       
        if (pthread_create(&tid, NULL, send_request, thrArg)){
            //free(thrArg);
            //free(flags);
            perror("Failed to create thread");
            exit(1);
        }

        pthread_detach(tid);

        if (usleep(5000)) {
            //free(flags);
            //free(thrArg);
            perror("Failed sleeping");
            exit(1);
        }
        
        t++;
    }

    if (close(public_fd) == -1){
        //free(flags);
        perror("Failed closing fifo");
        exit(1);
    }

    //free(flags);
    pthread_exit(NULL);
}