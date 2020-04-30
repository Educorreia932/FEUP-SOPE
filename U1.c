#include <time.h>
#include <sys/types.h>

#include "flagsU.h"
#include "log.h"

#include "utils.h"

flagsU* flags;


static int public_fd;

void * send_request(void * arg) { 
    
    //PRIVATEFIFO

    char privateFIFO[BUF_SIZE];
    int private_fd;
    sprintf(privateFIFO, "/tmp/%d.%lu", getpid(), pthread_self());
    
    //PREPARE MESSAGE
    char msg[BUF_SIZE];
    //[ i, pid, tid, dur, pl] 
    sprintf(msg, "%d, %d, %lu, %d", * (int *)arg, getpid(), pthread_self(), rand() % 200 + 50);

    free(arg);


    //SEND REQUEST
    int n = 0;
    while ((n = write(public_fd, msg, strlen(msg)) == 0)){
        continue;
    }

    if (n < 0) {
        perror("[CLIENT] Couldn't write to public FIFO");
        pthread_exit(NULL);
    }
    
    if(mkfifo(privateFIFO, 0660) < 0){
        perror("[Client]Failed to create fifo");
        exit(1);
    }

    if ((private_fd = open(privateFIFO, O_RDONLY)) == -1) {
        perror("[Client]Couldn't open FIFO.\n");
        exit(1);
    }
    enum Operation op = IWANT; //LOG
    print_log(msg, op);



    // READ
    char buffer[BUF_SIZE];
    n = 0;
    while ((n = read(private_fd, buffer, BUF_SIZE)) == 0){
        continue;
    }

    if (n < 0){
        perror("[CLIENT] Couldn´t read private FIFO");
        pthread_exit(NULL);
    }  

    printf("%s\n", buffer);
    
    //UNLINK & CLOSE PRIVATE FIFO

    if(close(private_fd) == -1) {
        perror("[Client]Couldn't close private FIFO");
        pthread_exit(NULL);
    }

    if(unlink(privateFIFO) == -1){
        perror("[Client]Failed to delete FIFO");
        exit(1);
    }

    pthread_exit(NULL);
}

int main(int argc, char * argv[]){
    
    //Begin Time count
    time_t begin = time(NULL);


    // Check Flags
    flags = flagsU_constructor(); 
    
    parse_flagsU(argc, argv, flags);

    if (argc != 4 || flags->fifoname == NULL || flags->nsecs == 0) {
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
        perror("Failed to open FIFO");
        exit(1);
    } 

    //Thread creating
    pthread_t tid[MAX_THREADS];
    int *thrArg, t = 0;
    
    srand(time(NULL));

    while( (time(NULL) - begin) < flags->nsecs && t < MAX_THREADS){
        
        thrArg = (int *) malloc(sizeof(t));
        *thrArg = t + 1; //request number

       
        if (pthread_create(&tid[t], NULL, send_request, thrArg)){
            perror("Failed to create thread");
            exit(1);
        }

        pthread_detach(tid[t]);
        if(usleep(100)){
            perror("Failed sleeping");
            exit(1);
        }
        
        
        t++;
    }


    if(close(public_fd) == -1){
        perror("Failed closing fifo");
        exit(1);
    }

    pthread_exit(NULL);

}