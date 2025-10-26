cmake_minimum_required(VERSION 4.0)
project(testMoba)

set(CMAKE_CXX_STANDARD 20)
include_directories(C:/projects/C++/NetFrameWork)
add_definitions(-D_WIN32_WINNT=0x0601)

add_executable(testMoba main.cpp protos/test.pb.cc
        internal/server.cpp
        internal/server.h
        internal/containers/containers.h
        internal/objects/player.h
        internal/containers/error.h
        internal/objects/timer.h
        internal/serverSync.cpp
        internal/serverUpdate.cpp
        internal/serverTerrain.cpp
        internal/serverInput.cpp
        internal/core/core.h
        internal/core/components/movingComponent.cpp
        internal/core/components/attackComponent.cpp
        internal/core/components/attributeComponent.cpp
        internal/utils/syncer.h
        internal/objects/bullet.h
        internal/core/components/components.h
        internal/core/components/hitComponent.cpp
        internal/core/components/skillComponent.cpp
        internal/core/skills/skill.h
        internal/core/skills/ezQ.cpp
        internal/objects/worldManager.h
)
target_link_directories(testMoba PRIVATE C:/projects/TestMoba/libs)

target_link_libraries(testMoba PRIVATE NetFrameWork)
target_link_libraries(testMoba PRIVATE ws2_32 Mswsock)


set(protobuf_DIR "C:/dev/vcpkg/installed/x64-mingw-static/share/protobuf")
set(absl_DIR "C:/dev/vcpkg/installed/x64-mingw-static/share/absl")
set(utf8_range_DIR "C:/dev/vcpkg/installed/x64-mingw-static/share/utf8_range")

find_package(protobuf CONFIG REQUIRED)
find_package(absl REQUIRED)
target_link_libraries(testMoba PRIVATE protobuf::libprotobuf)
set(asio_DIR "C:/dev/vcpkg/installed/x64-mingw-dynamic/share/asio")
find_package(asio REQUIRED)
target_link_libraries(testMoba PRIVATE asio::asio)

set(box2d_DIR  "C:/dev/vcpkg/installed/x64-mingw-static/share/box2d")
find_package(box2d CONFIG REQUIRED)
target_link_libraries(testMoba PRIVATE box2d::box2d)