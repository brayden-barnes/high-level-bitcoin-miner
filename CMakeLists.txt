cmake_minimum_required(VERSION 3.11)
project(miner C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_executable(miner main.c mempool.h mempool.c nonce.c nonce.h transactions.h transactions.c siggen.c siggen.h reader.c reader.h event_q.c event_q.h)
find_package(Threads REQUIRED)
target_link_libraries(miner PRIVATE Threads::Threads)
