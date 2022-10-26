cmake_minimum_required(VERSION 3.4.1)

project("adirfhook")
set(CMAKE_CXX_STANDARD 11)

find_library(log-lib log)

set(SOURCES
  src/main.cpp
)

# frida-gum requires, ref: build/frida-android-arm64/lib/pkgconfig/frida-gum-1.0.pc
if (NOT DEFINED ENV{FRIDA_BUILD_PATH})
    message(FATAL_ERROR "not defined env var: FRIDA_BUILD_PATH") # export FRIDA_SDK_PATH=/home/lds/project/python/frida/build
endif()
set(FRIDA_SDK_DIR $ENV{FRIDA_BUILD_PATH}/sdk-android-arm64) 
message("FRIDA_SDK_DIR: " ${FRIDA_SDK_DIR})
include_directories(${FRIDA_SDK_DIR}/include)
include_directories(${FRIDA_SDK_DIR}/include/glib-2.0)
include_directories(${FRIDA_SDK_DIR}/include/gio-unix-2.0)
include_directories(${FRIDA_SDK_DIR}/lib/glib-2.0/include)
include_directories(${FRIDA_SDK_DIR}/include/capstone)
link_directories(${FRIDA_SDK_DIR}/lib)
set(FRIDA_SDK_LIBS 
  libglib-2.0.a
  libcapstone.a 
  libgobject-2.0.a 
  libffi.a 
  liblzma.a 
  libz.a 
  libunwind.a 
  libelf.a 
  libdwarf.a 
  libiconv.a 
  libminizip.a
  libgmodule-2.0.a
  libgio-2.0.a
)

# frida-gum
set(FRIDA_DIR $ENV{FRIDA_BUILD_PATH}/frida-android-arm64) 
message("FRIDA_DIR: " ${FRIDA_DIR})
include_directories(${FRIDA_DIR}/include/frida-1.0)
set(FRIDA_GUM_LIB ${FRIDA_DIR}/lib/libfrida-gum-1.0.a)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${log-lib} ${FRIDA_SDK_LIBS} ${FRIDA_GUM_LIB})
