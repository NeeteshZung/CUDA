#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*Compile
  gcc Q.N.1.c -o Q.N.1 pthread
  ./task1 no of thread


*/


struct thareadargument{
	int start;
	int finish;

};

// Creating Global variables
pthread_mutex_t mutex;
double **Amatrix, **Bmatrix, **Cmatrix;
double sum=0.0;
int arows, acols, brows, bcols;


// Thread funtion for applying multithreading in matrix multiplication
void* matrixMul(void* args){
pthread_mutex_lock(&mutex);
struct thareadargument* p=(struct thareadargument*) args;
pthread_t tid = pthread_self();
int st=p->start;
int fi=p->finish;
for(int i=st; i<=fi; i++){
	for(int j=0; j<bcols; j++){
		for(int k=0; k<brows; k++){	
		sum = sum +	Amatrix[i][k]* Bmatrix[k][j];
		}
		Cmatrix[i][j]=sum;
		sum=0.0;
		}
	}

pthread_mutex_unlock(&mutex);

}



int main(int argc, char *argv[]){
 
 int defaultThread = 1;
   if(argc>1){
       defaultThread = atoi(argv[1]);
    }

 
  if(defaultThread<=0 || defaultThread>1000){
        printf("no of thread should be in rang 0< and >10001\n");
        exit(0);

    }
    

  FILE *fptr1 = NULL;
  FILE *fptr2 =NULL;
  FILE *fptr3 =NULL;
  int i, j;
  int k;

  fptr1 = fopen("SampleMatricesWithErrors3.txt", "r");
  fptr2 = fopen("Error", "w");
  fptr3 = fopen("matrixresults2048494.txt","w");
  while(!feof(fptr1)){

  	// for matrix A
    fscanf(fptr1,"%d,%d",&arows, &acols);
    printf("aRows: %d, aCols: %d\n", arows, acols);
    
    Amatrix=(double**)malloc(arows * sizeof(double*));
	for(i=0; i<arows; i++){
		Amatrix[i] = malloc(acols* sizeof(double));
		
	}
    
    for(i = 0; i < arows; i++){
      for(j = 0; j < acols-1; j++){
        fscanf(fptr1,"%lf,",&Amatrix[i][j]);
      }
      fscanf(fptr1,"%lf\n",&Amatrix[i][j]);
    }
    

    // for matrix B
    fscanf(fptr1,"%d,%d",&brows, &bcols);
    printf("bRows: %d, bCols: %d\n", brows, bcols);
    
    Bmatrix=(double**)malloc(brows * sizeof(double*));
	for(i=0; i<brows; i++){
		Bmatrix[i] = malloc(bcols* sizeof(double));
		
	}
    
    for(i = 0; i < brows; i++){
      for(j = 0; j < bcols-1; j++){
        fscanf(fptr1,"%lf,",&Bmatrix[i][j]);
      }
      fscanf(fptr1,"%lf\n",&Bmatrix[i][j]);
    }

    
    if (acols!=brows){
        
        fprintf(fptr2,"Size of matrix is not valid.\n which is arows= %d, acols= %d and brows= %d, bcols= %d\n",arows,acols,brows,bcols);
        continue;


    }

    
    // Allocating 2D Dyanamically Array for  Cmatrix
    Cmatrix=(double**)malloc(arows  * sizeof(double*));
	for(i=0; i<arows; i++){
		Cmatrix[i] = malloc(bcols* sizeof(double));
		
	}


   // sliciing
    int numofthreads = 1;
   if(argc>1){
       numofthreads = defaultThread;
    }

   
   int iteration = arows; 

      if(numofthreads>iteration){
        numofthreads = iteration;
    }

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

  printf("After Sclicing dynamically in equal thread.\n");

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
		pthread_create(&threadId[i],NULL,matrixMul,&s[i]);
	}
     
     for (int i = 0; i <numofthreads; i++){

     	pthread_join(threadId[i],NULL);
     }
     pthread_mutex_destroy(&mutex);
     

b

    fprintf(fptr3,"Multiplication of matrix \n");
    for(i=0; i<arows; i++){
		for(j=0; j<bcols; j++){
			fprintf(fptr3,"%lf\t",Cmatrix[i][j]);
		}
		fprintf(fptr3,"\n");
	}
    fprintf(fptr3,"\n");
     
    
    	
	
	
    
	
	
    // deallocating 2D dynamically array for a
   for(i=0; i<arows; i++){
		free(Amatrix[i]);
		
	}
	free(Amatrix);
	
	// deallocating 2D dynamically array for b
	for(i=0; i<brows; i++){
		free(Bmatrix[i]);
		
	}
	free(Bmatrix);
	
	// deallocating 2D dynamically array for c
	for(i=0; i<arows; i++){
		free(Cmatrix[i]);
		
	}
	free(Cmatrix);

    
    
    
  }//while loop end


  fclose(fptr1);

  return 0;
}

