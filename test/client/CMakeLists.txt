cmake_minimum_required(VERSION 3.10)
project(ShardKvClient)

set(CMAKE_CXX_STANDARD 14)

include_directories(/root/Github/incubator-brpc/output/include)
link_directories(/root/Github/incubator-brpc/output/lib)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(Threads)



set(ShardKv
        ../../shardkv/ShardKvClient.h ../../shardkv/ShardKvClient.cc
        ../../proto/shardkv.pb.h ../../proto/shardkv.pb.cc

        ../../util/global.h ../../util/global.cc
        )

set(ShardKvTransaction
        ../../shardkv/TransactionController.h ../../shardkv/TransactionController.cc
        ../../shardkv/TransactionContext.h ../../shardkv/TransactionContext.cc
        )

set(Engine
        ../../shardkv/Engine.h ../../shardkv/Engine.cc
        )


add_executable( ShardKvClient client.cc
        ${ShardKv}
        ${ShardKvTransaction}
        ${Engine}
        )

target_link_libraries(ShardKvClient Threads::Threads)
target_link_libraries(ShardKvClient brpc)

target_link_libraries(ShardKvClient protobuf)
target_link_libraries(ShardKvClient protoc)


