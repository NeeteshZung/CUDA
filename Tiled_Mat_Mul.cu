template<int TS>
__global__ void gemm_tiled_shared_opt(const float* __restrict__ A,
                                      const float* __restrict__ B,
                                      float* __restrict__ C,
                                      int N)
{
    __shared__ float As[TS][TS + 1];
    __shared__ float Bs[TS][TS + 1];

    const int tx  = threadIdx.x;
    const int ty  = threadIdx.y;
    const int row = blockIdx.y * TS + ty;
    const int col = blockIdx.x * TS + tx;

    float sum = 0.0f;
    const int numTiles = (N + TS - 1) / TS;

    for (int t = 0; t < numTiles; ++t) {
        const int Acol = t * TS + tx;
        const int Brow = t * TS + ty;

        As[ty][tx] = (row < N && Acol < N) ? A[row * N + Acol] : 0.0f;
        Bs[ty][tx] = (Brow < N && col < N) ? B[Brow * N + col] : 0.0f;

        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TS; k++) {
            sum = fmaf(As[ty][k], Bs[k][tx], sum);
        }

        __syncthreads();
    }

    if (row < N && col < N) {
        C[row * N + col] = sum;
    }
}
