cmake_minimum_required(VERSION 3.20)

project(cuda-memory-demo LANGUAGES CXX CUDA)

set(CMAKE_CUDA_STANDARD 17)

add_executable(bandwidth-latency-test bandwidth-latency.cu utility.cu)

# target_compile_features(bandwidth-latency-test PUBLIC cuda_std_17)

add_executable(simulate simulate.cu utility.cu)

add_executable(simple-test simple-test.cu utility.cu)

add_executable(latency latency.cu utility.cu)

