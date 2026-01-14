#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <pthread.h>
#include <unistd.h>

/******************************************************************************
  Nitesh Bishwakarma 
  This programme found the encrypted password using mulitheading
  Compile with:
    gcc Q.N.2.c -o Q.N.2 -lcrypt -pthread
    ./task2 (no of thread)   where no of thread= 1 to 26
******************************************************************************/

struct thareadargument{
	int start;
	int finish;

};

pthread_mutex_t mutex;

int count=0;     // A counter used to track the number of combinations explored so far
char *salt_and_encrypted = "$6$AS$JfsX2qXD68V5//9isNuHZkjPBxO.M9DtVN4Cxy1oRjr5QAWrKdqguFto2CB4CoR3ppGd.l715T9L35oXvJZK4.";//AK37

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

//Thread function to apply multithreading in password cracking
void* crack(void* args ){
  pthread_mutex_lock(&mutex);
  struct thareadargument* p=(struct thareadargument*) args;
  pthread_t tid = pthread_self();
  int st=p->start;
  int fi=p->finish;
  int x, y, z;     // Loop counters
  char salt[7];    // String used in hashing the password. Need space for \0 // incase you have modified the salt value, then should modifiy the number accordingly
  char plain[7];   // The combination of letters currently being checked // Please modifiy the number when you enlarge the encrypted password.
  char *enc;       // Pointer to the encrypted password

  substr(salt, salt_and_encrypted, 0, 6);

  for(x=st+65; x<=fi+65; x++){
    for(y='A'; y<='Z'; y++){
      for(z=0; z<=99; z++){
        sprintf(plain, "%c%c%02d", x, y, z); 
        enc = (char *) crypt(plain, salt);
        count++;
      if(strcmp(salt_and_encrypted, enc) == 0){
      printf("passwoord found = %s\n",plain);
      printf("Thread %ld found the password \n",pthread_self());
      printf("%d solutions explored\n", count);
      pthread_cancel(pthread_self());
      exit(0);
        } 
      }
    }
  }
  pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[]){
  int numofthreads = 1;
   int iteration = 26;
    if(argc>1){
       numofthreads = atoi(argv[1]);
    }
    if(numofthreads>iteration){
        numofthreads = iteration;
    }
 printf("Programme in process.\n");
 pthread_mutex_init(&mutex,NULL);
 pthread_t threadId[numofthreads];
 struct thareadargument s[numofthreads];
	int slicelist[numofthreads];
	int i;
	for(i=0; i<numofthreads; i++) {
		slicelist[i]=iteration/numofthreads;
	}
	for(i=0; i<numofthreads; i++) {
		printf("slicelist=%d\n",slicelist[i]);
	}

	int remainder = iteration % numofthreads;
	printf("remainder=%d\n",remainder );

	for (int i = 0; i < remainder; i++){

         slicelist[i]=slicelist[i]+1;
	}
  printf("After dynamically slicing\n");
for(i=0; i<numofthreads; i++) {
		printf("slicelist=%d\n",slicelist[i]);
	}

	for (int i = 0; i < numofthreads; i++){
		if(i==0){
			s[i].start=0;
		}
		else{
			s[i].start=s[i - 1].finish + 1;
		}
		s[i].finish=s[i].start + slicelist[i] -1;
	}


     for (int i = 0; i <numofthreads; i++){
		pthread_create(&threadId[i],NULL,crack,&s[i]);
	}
     
     for (int i = 0; i <numofthreads; i++){

     	pthread_join(threadId[i],NULL);
     }
     pthread_mutex_destroy(&mutex);
     
  return 0;
}

