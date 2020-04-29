#include <time.h>
#include <sys/types.h>
#include <semaphore.h>

#include "flagsU.h"
#include "log.h"

#include "utils.h"

flagsU* flags;


static int public_fd;
static sem_t *sem;

void * send_request(void * arg) { 
    
    //open semaphore
    char sem_name[50];
    sprintf(sem_name , "/sem%d", *(int*) arg);
    sem = sem_open(sem_name,O_CREAT,0600,0); 

    if(sem == SEM_FAILED)   {     
        perror("WRITER failure in sem_open()");     
        exit(4);   
    }

    
    
    char msg[BUF_SIZE];
    //[ i, pid, tid, dur, pl] 
    sprintf(msg, "%d, %d, %lu, %d", * (int *)arg, getpid(), pthread_self(), rand() % 200 + 50);
    //printf("\n%s\n", msg);
    
    //Name of privateFIFO
    char privateFIFO[MAX_STR];
    sprintf(privateFIFO, "/tmp/%d.%lu", getpid(), pthread_self());
    
    int private_fd = open(privateFIFO, O_RDONLY | O_CREAT, 0666);

    if (private_fd == -1 ) {
        perror("Couldn't create private FIFO");
        pthread_exit(NULL);
    }

    //CLOSE FIFO FOR READING

    //send request
    write(public_fd, msg, strlen(msg));
    
sem_init(sem, 1, 0);
    //UNLOCK FIFO FOR READING

    //WAIT FOR RESPONSE
    
    char buffer[BUF_SIZE];

    int asf;
    sem_getvalue(sem,&asf);

    printf("before %d\n", asf);

    sem_wait(sem);

    while (read(private_fd, buffer, BUF_SIZE) <= 0)
        continue;
    
    printf("%s\n", buffer);
    if(close(private_fd) == -1) {
        perror("Couldn't close private FIFO");
        pthread_exit(NULL);
    }

    // enum Operation op = IWANT;
    // print_log(msg, op);
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

        if(usleep(100)){
            perror("Failed sleeping");
            exit(1);
        }
        
        t++;
    }
    
    for(int i = 0; i < t; i++){
        pthread_join(tid[i], NULL);
    }

    if(close(public_fd) == -1){
        perror("Failed closing fifo");
        exit(1);
    }

    return 0;

}