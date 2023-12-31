cmake_minimum_required(VERSION 3.14)
SITE_NAME(HOSTNAME)
if(${HOSTNAME} STREQUAL "j63c09243.sqa.eu95")
set(CMAKE_CUDA_COMPILER /usr/local/cuda-10.1/bin/nvcc)
elseif(${HOSTNAME} STREQUAL "meepo4")
set(CMAKE_CUDA_COMPILER /usr/local/cuda-10.2/bin/nvcc)
elseif(${HOSTNAME} STREQUAL "meepo5")
set(CMAKE_CUDA_COMPILER /usr/local/cuda-11/bin/nvcc)
elseif(${HOSTNAME} STREQUAL "SH-IDC1-10-142-40-63")
set(CMAKE_CUDA_COMPILER /usr/local/cuda-11.7/bin/nvcc)
else()
message( STATUS "host is $ENV{HOSTNAME}")
message( WARNING "unknown host" )
endif()

project(samgraph_cpp_version CXX CUDA)

# set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(COMMON_FLAGS " -O0 -g -DSXN_REVISED ")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(COMMON_FLAGS " -O3 -g -DSXN_REVISED ")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}  ${COMMON_FLAGS} -fPIC -Wall -fopenmp -march=native -D_GLIBCXX_USE_CXX11_ABI=0 -DUNIT_TEST")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} ${COMMON_FLAGS} --compiler-options '-fPIC -D_GLIBCXX_USE_CXX11_ABI=0' -gencode=arch=compute_35,code=sm_35 -gencode=arch=compute_70,code=sm_70 -gencode=arch=compute_80,code=sm_80")
message(DEBUG "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(DEBUG "CMAKE_CUDA_FLAGS: ${CMAKE_CUDA_FLAGS}")
include_directories(
  ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
  # ../3rdparty/cub
  # ../3rdparty/parallel-hashmap
  ../../../CLI11/include
  # $ENV{CONDA_PREFIX}/lib/python3.8/site-packages/torch/include
  # $ENV{CONDA_PREFIX}/lib/python3.8/site-packages/torch/include/torch/csrc/api/include
  # $ENV{CONDA_PREFIX}/include/python3.8
  )
link_directories(
  # $ENV{CONDA_PREFIX}/lib
  # $ENV{CONDA_PREFIX}/lib/python3.8/site-packages/torch/lib
  /usr/local/cuda/lib64
)


file(GLOB_RECURSE SAMGRAPH_SRC
  ./*.cc
  ./*.cu)

# build samgraph library
add_library(samgraph_lib
  ${SAMGRAPH_SRC}
)
target_link_libraries(samgraph_lib
  nvToolsExt)

add_executable(
  test_hashtable
  main.cc
)

target_link_libraries(
  test_hashtable
  samgraph_lib
)