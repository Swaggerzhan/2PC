cmake_minimum_required(VERSION 3.10)
project(CoordinatorServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(/root/Github/incubator-brpc/output/include)
link_directories(/root/Github/incubator-brpc/output/lib)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(Threads)


set(TransactionCoordinator
        ../../coordinator/CoordinatorServer.h ../../coordinator/CoordinatorServer.cc
        ../../coordinator/TransactionCoordinator.h ../../coordinator/TransactionCoordinator.cc
        ../../util/MurmurHash2.h ../../util/MurmurHash2.cc
        )

set(proto
        ../../proto/transaction.pb.h ../../proto/transaction.pb.cc
        ../../proto/shardkv.pb.h ../../proto/shardkv.pb.cc
        )

set(ShardKvClient
        ../../shardkv/ShardKvClient.h ../../shardkv/ShardKvClient.cc
        ../../util/global.h ../../util/global.cc
        )

set(ShardKvTransaction
        ../../shardkv/TransactionController.h ../../shardkv/TransactionController.cc
        ../../shardkv/TransactionContext.h ../../shardkv/TransactionContext.cc
        )

set(Engine
        ../../shardkv/Engine.h ../../shardkv/Engine.cc
        )


add_executable( CoordinatorServer coordinatorServer.cc
        ${TransactionCoordinator}
        ${proto}
        ${ShardKvClient}
        ${ShardKvTransaction}
        ${Engine}
        )

target_link_libraries(CoordinatorServer Threads::Threads)
target_link_libraries(CoordinatorServer brpc)

target_link_libraries(CoordinatorServer protobuf)
target_link_libraries(CoordinatorServer protoc)


