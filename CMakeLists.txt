cmake_minimum_required(VERSION 3.10)
project(2PC)

set(CMAKE_CXX_STANDARD 14)

include_directories(/root/Github/incubator-brpc/output/include)
link_directories(/root/Github/incubator-brpc/output/lib)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(Threads)

add_executable(2PC main.cpp
        shardkv/Engine.h shardkv/Engine.cc
        shardkv/TransactionContext.h
        shardkv/TransactionContext.cc
        shardkv/TransactionController.h shardkv/TransactionController.cc
        proto/shardkv.pb.h proto/shardkv.pb.cc
        proto/transaction.pb.h proto/transaction.pb.cc


        client/KvClient.h client/KvClient.cc
        shardkv/ShardKv.h shardkv/ShardKv.cc
        shardkv/ShardKvServer.h shardkv/ShardKvServer.cc
        coordinator/TransactionCoordinator.h coordinator/TransactionCoordinator.cc
        shardkv/ShardKvClient.cc shardkv/ShardKvClient.h

        util/MurmurHash2.h util/MurmurHash2.cc
        util/global.h util/global.cc


        coordinator/CoordinatorServer.h coordinator/CoordinatorServer.cc coordinator/CoordinatorClient.h coordinator/CoordinatorClient.cc)

target_link_libraries(2PC Threads::Threads)
target_link_libraries(2PC brpc)

target_link_libraries(2PC protobuf)
target_link_libraries(2PC protoc)


