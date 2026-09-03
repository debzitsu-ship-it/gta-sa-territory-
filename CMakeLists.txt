cmake_minimum_required(VERSION 3.18)
project(GTA5CityUnlock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT AML_ROOT)
    message(FATAL_ERROR "AML_ROOT is required")
endif()

add_library(GTA5CityUnlock SHARED src/GTA5CityUnlock.cpp)

target_include_directories(GTA5CityUnlock PRIVATE
    "${AML_ROOT}/mod"
)

target_compile_options(GTA5CityUnlock PRIVATE
    -fPIC
    -fvisibility=hidden
    -O2
    -ffunction-sections
    -fdata-sections
)

target_link_options(GTA5CityUnlock PRIVATE
    -Wl,--gc-sections
    -Wl,-z,relro
    -Wl,-z,now
)

target_link_libraries(GTA5CityUnlock PRIVATE
    log
    android
)
