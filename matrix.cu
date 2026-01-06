#include <stdio.h>

#define N 4   // rows
#define M 4   // cols

__global__ void matrixAdd(float *A, float *B, float *C, int rows, int cols) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rows && col < cols) {
        int idx = row * cols + col;
        C[idx] = A[idx] + B[idx];
    }
}

int main() {
    int size = N * M * sizeof(float);

    float h_A[N*M], h_B[N*M], h_C[N*M];

    for (int i = 0; i < N*M; i++) {
        h_A[i] = i;
        h_B[i] = i * 2;
    }

    float *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_B, size);
    cudaMalloc(&d_C, size);

    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid(
        (M + threadsPerBlock.x - 1) / threadsPerBlock.x,
        (N + threadsPerBlock.y - 1) / threadsPerBlock.y
    );

    matrixAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N, M);

    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    printf("Result Matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            printf("%5.1f ", h_C[i*M + j]);
        printf("\n");
    }

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}
