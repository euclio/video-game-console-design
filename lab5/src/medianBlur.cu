#include "medianBlur.hpp"

const size_t BLOCK_ROWS = 32;
const size_t BLOCK_COLS = 16;
const size_t MEDIAN_LENGTH = 9;

/*
 * Graciously based on
 * http://stackoverflow.com/questions/22315903/cuda-median-filter-implementation-does-not-produce-desired-results
 */
__global__ void blurKernel (
        unsigned char *d_input_img, unsigned char *d_output_img,
        int d_iRows, int d_iCols) {
    unsigned int row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned int col = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned char window[MEDIAN_LENGTH];

    if (col > d_iCols || row >= d_iRows) {
        return;
    }

    window[0] = (row==0||col==0) ? 0 :                 d_input_img[(row-1)*d_iCols+(col-1)];
    window[1] = (row==0) ? 0 :                         d_input_img[(row-1)*d_iCols+col];
    window[2] = (row==0||col==d_iCols-1) ? 0 :         d_input_img[(row-1)*d_iCols+(col+1)];
    window[3] = (col==0) ? 0 :                         d_input_img[row*d_iCols+(col-1)];
    window[4] =                                        d_input_img[row*d_iCols+col];
    window[5] = (col==d_iCols-1) ? 0 :                 d_input_img[row*d_iCols+(col+1)];
    window[6] = (row==d_iRows-1||col==0) ? 0 :         d_input_img[(row+1)*d_iCols+(col-1)];
    window[7] = (row==d_iRows-1) ? 0 :                 d_input_img[(row+1)*d_iCols+col];
    window[8] = (row==d_iRows-1||col==d_iCols-1) ? 0 : d_input_img[(row+1)*d_iCols+(col+1)];

    // Order elements
    for (unsigned int j = 0; j < 5; ++j) {
        // Find position of minimum element
        unsigned char temp = window[j];
        unsigned int  idx  = j;
        for (unsigned int l = j + 1; l < 9; ++l) {
            if (window[l] < temp) {
                idx = l;
                temp = window[l];
            }
        }

        // Put found minimum element in its place
        window[idx] = window[j];
        window[j] = temp;
    }

    d_output_img[row*d_iCols + col] = window[4];
}

void gpuMedianBlur(const cv::Mat& input, const cv::Mat& output) {
    unsigned char *device_input, *device_output;

    size_t d_ipimgSize = input.step * input.rows;
    size_t d_opimgSize = output.step * output.rows;

    cudaMalloc((void**) &device_input, d_ipimgSize);
    cudaMalloc((void**) &device_output, d_opimgSize);

    cudaMemcpy(device_input, input.data, d_ipimgSize, cudaMemcpyHostToDevice);

    dim3 Threads(BLOCK_ROWS, BLOCK_COLS);
    dim3 Blocks(
            (input.cols + Threads.x - 1) / Threads.x,
            (input.rows + Threads.y - 1) / Threads.y);

    blurKernel<<< Blocks, Threads>>>(
            device_input, device_output, input.rows, input.cols);

    cudaDeviceSynchronize();

    cudaMemcpy(output.data, device_output, d_opimgSize, cudaMemcpyDeviceToHost);

    cudaFree(device_input);
    cudaFree(device_output);
}
