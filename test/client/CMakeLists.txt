cmake_minimum_required(VERSION 3.10)
project(client)

set(CMAKE_CXX_STANDARD 14)

include_directories(/root/Github/incubator-brpc/output/include)
link_directories(/root/Github/incubator-brpc/output/lib)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(Threads)

add_executable(client client.cpp
        ../../shardkv/Engine.h ../../shardkv/Engine.cc
        ../../shardkv/TransactionContext.h
        ../../shardkv/TransactionContext.cc
        ../../shardkv/KvServer.h ../../shardkv/KvServer.cc
        ../../proto/KvServer.pb.h ../../proto/KvServer.pb.cc
        ../../shardkv/TransactionController.h ../../shardkv/TransactionController.cc
        ../../client/KvClient.h ../../client/KvClient.cc)

target_link_libraries(client Threads::Threads)
target_link_libraries(client brpc)

target_link_libraries(client protobuf)
target_link_libraries(client protoc)


