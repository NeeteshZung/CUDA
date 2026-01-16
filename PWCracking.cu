#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime_api.h>
/***************************************************************************************
Demonstrates how to crack an encrypted password using a CUDA. Works on passwords that consist only of 2 uppercase
letters and a 2 digit integer.

Compile with:
 nvcc Q.N.3.cu -o Q.N.3
 ./Q.N.3
 
//Password Cracking using  
*****************************************************************************************/

//__global__ --> GPU function which can be launched by many blocks and threads
//__device__ --> GPU function or variables
//__host__ --> CPU function or variables


int calculate_time(struct timespec *start, struct timespec *end,
                   long long int *diff)
{
  long long int in_sec = end->tv_sec - start->tv_sec;
  long long int in_nano = end->tv_nsec - start->tv_nsec;
  if (in_nano < 0)
  {
    in_sec--;
    in_nano += 1000000000;
  }
  *diff = in_sec * 1000000000 + in_nano;
  return !(*diff > 0);
}


// where r = rawPassword and n = newPassword generated password
__device__ char* encryptDecrypt(char* r){

	char * n = (char *) malloc(sizeof(char) * 11);

	n[0] = r[0] + 2;
	n[1] = r[0] - 2;
	n[2] = r[0] + 1;
	n[3] = r[1] + 3;
	n[4] = r[1] - 3;
	n[5] = r[1] - 1;
	n[6] = r[2] + 2;
	n[7] = r[2] - 2;
	n[8] = r[3] + 4;
	n[9] = r[3] - 4;
	n[10] = '\0';

	for(int i =0; i<10; i++){
		if(i >= 0 && i < 6){ //checking all upper case letter limits
			if(n[i] > 122){
				n[i] = (n[i] - 122) + 97;
			}else if(n[i] < 97){
				n[i] = (97 - n[i]) + 97;
			}
		}else{ //checking number section
			if(n[i] > 57){
				n[i] = (n[i] - 57) + 48;
			}else if(n[i] < 48){
				n[i] = (48 - n[i]) + 48;
			}
		}
	}
	return n;
}

__global__ void crack(char * alphabet, char * numbers, char * password){

  char matchedPassword[4];

  matchedPassword[0] = alphabet[blockIdx.x];
  matchedPassword[1] = alphabet[blockIdx.y];

  matchedPassword[2] = numbers[threadIdx.x];
  matchedPassword[3] = numbers[threadIdx.y];

//firstLetter - 'A' - 'Z' (26 characters)
//secondLetter - 'A' - 'Z' (26 characters)
//firstNum - '0' - '9' (10 characters)
//secondNum - '0' - '9' (10 characters)

//Idx --> gives current index of the block or thread



const char* encryptedPassword = "koleki6253"; //UZ41
const char* search = encryptDecrypt(matchedPassword);
int Pass_match = 0;
while (*encryptedPassword != '\0' || *search != '\0') {
	if (*encryptedPassword == *search) {
		encryptedPassword++;
		search++;
	} else if ((*encryptedPassword == '\0' && *search != '\0') || (*encryptedPassword != '\0' && *search == '\0') || *encryptedPassword != *search) {
		Pass_match = 1;
	
		break;
	}
}
if (Pass_match == 0) {
	password = matchedPassword;
	printf("Password Found: %c%c%c%c \n", matchedPassword[0],matchedPassword[1],matchedPassword[2],matchedPassword[3]);
}


}

int main(int argc, char ** argv){

	cudaError_t error;
	char * h_passwordfound;

	char h_Letters[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	char h_Digits[26] = {'0','1','2','3','4','5','6','7','8','9'};

	char * d_Letters;
	error = cudaMalloc( (void**) &d_Letters, sizeof(char) * 26); 
	if(error){
		fprintf(stderr, "cudaMalloc on d_Letter return %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}
	error = cudaMemcpy(d_Letters, h_Letters, sizeof(char) * 26, cudaMemcpyHostToDevice);
	if(error){
		fprintf(stderr, "cudaMemcpy to d_Letter return %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}

	char * d_Digits;
	error = cudaMalloc( (void**) &d_Digits, sizeof(char) * 26); 
	if(error){
		fprintf(stderr, "cudaMalloc on d_Digits return %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}
	error = cudaMemcpy(d_Digits, h_Digits, sizeof(char) * 26, cudaMemcpyHostToDevice);
	if(error){
		fprintf(stderr, "cudaMemcpy to d_Digits return %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}

	char* d_password;
	error = cudaMalloc( (void**) &d_password, sizeof(char) * 4); 
	if(error){
		fprintf(stderr, "cudaMalloc on d_password return %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}
	
	struct timespec start, end;
    long long int time_used;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    

    crack<<< dim3(26,26,1), dim3(10,10,1) >>>( d_Letters, d_Digits ,d_password);
	error = cudaGetLastError();
	if(error){
		fprintf(stderr, "Kernal launch returned %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}
    cudaDeviceSynchronize();
    error = cudaMemcpy(h_passwordfound, d_password, sizeof(char) * 4, cudaMemcpyDeviceToHost);
	if(error){
		fprintf(stderr, "cudaMemcpy to d_Digits return %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    calculate_time(&start, &end, &time_used);
    printf("Time taken: %f seconds OR %lld Nano Seconds\n", (time_used / 1.0e9), (time_used));


	error = cudaFree(d_Letters);
	if(error){
		fprintf(stderr, "cudaFree on d_Letters returned %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}
	error = cudaFree(d_Digits);
	if(error){
		fprintf(stderr, "cudaFree on d_Digits returned %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}
	error = cudaFree(d_password);
	if(error){
		fprintf(stderr, "cudaFree on d_password returned %d %s\n",error,cudaGetErrorString(error));
		exit(1);
	}

	printf("Password found %s\n",h_passwordfound);


	return 0;
}
