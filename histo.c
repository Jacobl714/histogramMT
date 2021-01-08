#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

/*
 * Provided modular exponentiation function.
 */

 
 //each cell in locked array will have value and lock 
 typedef struct Data {
    pthread_mutex_t lock1;
    int cell;
} Data;


 typedef struct sendToThread {
     char* fd;
     int startIndex;
     int endIndex;
     Data* D;
 } sendThread;

//create array of locked cells 

 
 
long modPow(long base,long exp,long m)
{
   if (m == 1) return 0;
   long c = 1;
   for (long ep=0;ep <= exp - 1;ep++)
      c = (c * base) % m;
   return c;
}

long modPowNew(long base, long exp, long m) {
    long c = 1;
    //base case --> no modulus 
    if(m == 1){
        return 0;
    }
    //keep shifting down until its no longer >0
    while(exp > 0) {
        //if exponent is odd --> mod the base* current result to get new remainder 
        if(exp % 2 == 1){
            c = (c*base) % m;
        }
        //divide exponent by 2 then increment base by 
        exp >>= 1;
        //square the base and mod it 
        base = (base*base) % m;
    }
    //return modPow result 
    return c;
}


void* usingThreads(void* sending){
    sendThread* st2 = (sendThread*)sending;
    char* tArray = st2->fd;
    //start to end of sendThread list 
    for(int j = st2->startIndex; j < st2->endIndex; j++){
        //make soln the result of modpow 
        long soln = modPowNew(tArray[j], tArray[j+1], 256);
        //lock spot in array at this soln value 
        
        pthread_mutex_t* lock2 = &st2->D[soln].lock1 ;
        pthread_mutex_lock(lock2);
        //change from 0 to 1 
        st2->D[soln].cell++;
        pthread_mutex_unlock(lock2);
        
    }
    free(st2);
    
}



int main(int argc,char* argv[])
{
   int nbT = atoi(argv[2]);
   struct stat fs;
   char* fName = argv[1];
   stat(fName,&fs);
   size_t sz = fs.st_size;
   
   //file array put into as ints 
   //pthread_mutex_init(fName);
   int fd = open(argv[1], O_RDWR);
   
   
   //load file  --> lab8?
   size_t sizeOfMap = sz * sizeof(char);
   sizeOfMap = (sizeOfMap / 4096) + 1;
   sizeOfMap *= 4096;
   
   char* newPtr = mmap(NULL, sizeOfMap, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_FILE, fd, 0);
   
   if (newPtr == MAP_FAILED) {
        printf("mmap failure: %s\n",strerror(errno));
        exit(1);
  }
  
  close(fd);
  
  
  
  //Data lockedArray[256];
   
   //allocate space for locking array to be sz * size of ints 
   
   
   //making array w struct to hold locked values 
   Data* lockingArray = malloc(sizeof(Data) * 256);
   
   for(int i=0; i<256; i++){
       //init lock for cell in array from 0 to 255 
       pthread_mutex_init(&lockingArray[i].lock1, NULL);
       lockingArray[i].cell = 0;
   }
   
  
  
  
  //array of threads 
   pthread_t threads[nbT];
   
   for(int i= 0; i < nbT-1; i++){
       //allocate space 
        sendThread* st = malloc(sizeof(sendThread)); 
        //set char* to newPtr memmap
        st->fd = newPtr; 
        //set locking array to be data struct 
        //set start and end for all but last thread 
        st->D = lockingArray;
        //split up threads work depending on # of threads 
        
        st->startIndex = i *(sz / nbT);
        st->endIndex = st->startIndex + (sz/nbT) ;
        //create threads 
        pthread_create(&threads[i], NULL, usingThreads, (void*)st);
   }
   
  //for last thread 
    sendThread* st = malloc(sizeof(sendThread)); 
    //same as in for loop
    st->fd = newPtr; 
    st->D = lockingArray;
    //last thread starting point
    st->startIndex = (nbT-1) *(sz / nbT);
    //to end of file 
    st->endIndex = sz - 1;
    pthread_create(&threads[nbT-1], NULL, usingThreads, (void*)st);
   
   
   //join 
   for(int j =0; j< nbT; j++){
       pthread_join(threads[j], NULL);
   }
   
   
   //print histogram 
   for(int j = 0; j < 256; j++){
        printf("%i ", lockingArray[j].cell);
        }
    printf("\n");
   
   //fine grain lock int list 
   
   //pthread_mutex_unlock(fName);
   
   
   
   
   
   
   
   
   //load file into mem w mmap 
   //may need to make a strruct and then put this ina  function 
  
   
   
   /*
   
   want each thread to deal with 1 calc of modPow
   
   */
   
   
  
   
   
   /*
   fName = fopen(fName, "r");
   char *numArray;
   numArray = malloc(sizeof(int) * sz);
   for(int i = 0; i< sz; i++) {
       fscanf(fName, atoi("%c"), &numArray[i]);
   }
   */
   //create array of threads 
   //pthread_t *threads_array;
   //malloc enough space for all threads needed 
   //threads_array = malloc(sizeof(pthread_t)* nbT);
   
   
   //have each thread do diff modPow calc 
   //for(int i = 0; i< nbT; i++){
       
   //}
   //create array to hold modPow values 
   /*
   char* modArray;
   modArray = malloc(sizeof(int) * sz);
   for(int j = 0; j < sizeof(numArray)/sizeof(int); j++){
       modArray[j] = modPow(numArray[j], numArray[j+1], 256);
       
   }
   
   free(numArray);
   free(modArray);
   free(threads_array);
   */
   
   //for(int g =0; g<nbT; g++){
     //  pthread_create(&threads_array[i], NULL, modPow, & );
 //  }
   
  
   //for (i = 0; i < nbw; i++) {
   //     pthread_join(threads_array[i], NULL);
  //  }
   
   
   /* sz holds the size of the file in bytes */

   return 0;
}
