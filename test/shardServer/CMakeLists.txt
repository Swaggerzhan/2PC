cmake_minimum_required(VERSION 3.10)
project(ShardKvServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(/root/Github/incubator-brpc/output/include)
link_directories(/root/Github/incubator-brpc/output/lib)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(Threads)



set(ShardKv
        ../../shardkv/ShardKvServer.h ../../shardkv/ShardKvServer.cc
        ../../shardkv/ShardKv.h ../../shardkv/ShardKv.cc

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


add_executable( ShardKvServer server.cc
        ${ShardKv}
        ${ShardKvTransaction}
        ${Engine}
        )

target_link_libraries(ShardKvServer Threads::Threads)
target_link_libraries(ShardKvServer brpc)

target_link_libraries(ShardKvServer protobuf)
target_link_libraries(ShardKvServer protoc)


