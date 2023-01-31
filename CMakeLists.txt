cmake_minimum_required (VERSION 3.12)
# find_package(Python3) need version >= 3.12

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project (asyncflow)
set(PLATFORM x64)

option (BUILD_PYTHON "build python version" OFF)
option (BUILD_PYTHON2 "build python2 version" OFF)
option (BUILD_WASM "build WASM version" OFF)
option (BUILD_LUAJIT "build luajit version" OFF)
option (BUILD_TEST   "build test project" OFF)

if(TARGET xlua)
    message(STATUS ">>> finx xlua target")
    set(BUILD_LUAJIT ON)
endif(TARGET xlua)

if(${BUILD_TEST})
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        # add gcov compile flags
        message(STATUS "build with gcov flags")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fPIC -fprofile-arcs -ftest-coverage -fsanitize=address") 
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -fprofile-arcs -ftest-coverage -fsanitize=address")
    endif()
endif(${BUILD_TEST})

# msgpack
add_library(msgpack INTERFACE)
target_include_directories(msgpack INTERFACE "thirdparty/msgpack-c-cpp-3.0.1/include")

# rapidjson
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE "thirdparty")

# python
if(${BUILD_PYTHON})
    find_package (Python3 COMPONENTS Development)    
    if(NOT ${Python3_FOUND})
        message(FATAL_ERROR "cannot find python3")
    endif(NOT ${Python3_FOUND})
endif(${BUILD_PYTHON})

if(${BUILD_PYTHON2})
    find_package (Python2 COMPONENTS Development)    
    if(NOT ${Python2_FOUND})
        message(FATAL_ERROR "cannot find python2")
    endif(NOT ${Python2_FOUND})
endif(${BUILD_PYTHON2})


if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_compile_options(-fPIC)
else()
    add_definitions(-D_WIN32_WINNT=0x0601)
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")



# source files
aux_source_directory("src/core" CORE_SRCS)
aux_source_directory("src/export" EXPORT_SRCS)
aux_source_directory("src/debug" DEBUG_SRCS)
aux_source_directory("src/util" UTIL_SRCS)

# set source folder in visual studio project
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#source_group(include FILES ${HEADERS})
source_group(core FILES ${CORE_SRCS})
source_group(export FILES ${EXPORT_SRCS})
source_group(debug FILES ${DEBUG_SRCS})
source_group(util FILES ${UTIL_SRCS})

# header files
file(GLOB HEADERS "include/*.h")
source_group(include ${HEADERS})

file(GLOB CORE_HEADERS "include/core/*.h")
source_group(include\\core FILES ${CORE_HEADERS})
list(APPEND HEADERS ${CORE_HEADERS})

file(GLOB UTIL_HEADERS "include/util/*.h")
source_group(include\\util FILES ${UTIL_HEADERS})
list(APPEND HEADERS ${UTIL_HEADERS})

file(GLOB DEBUG_HEADERS "include/debug/*.h")
source_group(include\\debug FILES ${DEBUG_HEADERS})
list(APPEND HEADERS ${DEBUG_HEADERS})

# asyncflow source files
add_library (asyncflow STATIC ${CORE_SRCS} ${DEBUG_SRCS} ${EXPORT_SRCS} ${UTIL_SRCS}  ${HEADERS})
target_include_directories(asyncflow PUBLIC "include")
target_compile_features(asyncflow PUBLIC cxx_std_11)
set_target_properties(asyncflow PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(asyncflow PUBLIC msgpack rapidjson)

if(${BUILD_WASM})
    target_compile_definitions(asyncflow PUBLIC BUILD_WASM)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(asyncflow PUBLIC Threads::Threads)
endif(${BUILD_WASM})

if(FLOWCHART_DEBUG)
    # asio
    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE "thirdparty/asio-1.12.1/include")
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

    # websocketpp
    add_library(websocketpp INTERFACE)
    target_include_directories(websocketpp INTERFACE "thirdparty")
    target_compile_definitions (websocketpp INTERFACE _WEBSOCKETPP_CPP11_STRICT_)
    if(MSVC)
        target_compile_options(websocketpp INTERFACE /wd4996 /wd4995 /wd4355)
    else()
        target_link_libraries(asio INTERFACE pthread)
    endif(MSVC)

    # build debug
    add_definitions(-DFLOWCHART_DEBUG -D_SINGLE_THREAD_MODE)
    target_link_libraries(asyncflow PUBLIC asio websocketpp)
endif(FLOWCHART_DEBUG)

if(${BUILD_WASM})
    aux_source_directory("src/js" JS_SRCS)
    add_executable (asyncflow-js ${JS_SRCS})
    target_include_directories(asyncflow-js PUBLIC "include")
    target_compile_features(asyncflow-js PUBLIC cxx_std_11)
    set_target_properties(asyncflow-js PROPERTIES CXX_EXTENSIONS OFF)
    if(FLOWCHART_DEBUG)
        target_link_libraries(asyncflow-js asyncflow msgpack rapidjson)
    else(FLOWCHART_DEBUG)
        target_link_libraries(asyncflow-js asyncflow msgpack rapidjson)
    endif(FLOWCHART_DEBUG)

    message(STATUS "Setting compilation target to WASM")
    #set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")
    set_target_properties(asyncflow-js PROPERTIES LINK_FLAGS "-s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='[ccall, cwrap]' -s --js-library ${CMAKE_SOURCE_DIR}/src/js/pkg.js")
endif(${BUILD_WASM})

if(${BUILD_PYTHON})
    aux_source_directory("src/py" PY_SRCS)
    add_library (asyncflow-py SHARED ${PY_SRCS})
    target_compile_features(asyncflow-py PUBLIC cxx_std_11)
    set_target_properties(asyncflow-py PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(asyncflow-py asyncflow Python3::Python)
    target_compile_definitions(asyncflow-py PUBLIC USING_PYTHON)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")   # cmake > 3.15 => MSVC_RUNTIME_LIBRARY
    endif (MSVC)  
endif(${BUILD_PYTHON})

if(${BUILD_PYTHON2})
    aux_source_directory("src/py" PY_SRCS)
    add_library (asyncflow-py2 SHARED ${PY_SRCS})
    target_compile_features(asyncflow-py2 PUBLIC cxx_std_11)
    set_target_properties(asyncflow-py2 PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(asyncflow-py2 asyncflow Python2::Python)
    target_compile_definitions(asyncflow-py2 PUBLIC USING_PYTHON USING_PYTHON2)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")   # cmake > 3.15 => MSVC_RUNTIME_LIBRARY
    endif (MSVC)  
endif(${BUILD_PYTHON2})

if(${BUILD_LUAJIT})
    if(TARGET xlua)
        add_library(luajit ALIAS xlua)
    else()
    set(LUAJIT_INCLUDE_PATH CACHE PATH "luajit header files folder")
    set(LUAJIT_LIB CACHE FILEPATH "fullpath of luajit lib")    

    # add luajit
    add_library(luajit STATIC IMPORTED)
    set_target_properties(luajit PROPERTIES IMPORTED_LOCATION "${LUAJIT_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${LUAJIT_INCLUDE_PATH}")    
    endif(TARGET xlua)

    aux_source_directory("src/lua" LUA_SRCS)
    add_library (asyncflow-lua SHARED ${LUA_SRCS})
    target_compile_features(asyncflow-lua PUBLIC cxx_std_11)
    set_target_properties(asyncflow-lua PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(asyncflow-lua asyncflow luajit)

    if(${BUILD_TEST})
        aux_source_directory("src/test" LUA_TEST_SRCS)    
        add_executable(lua_test ${LUA_TEST_SRCS} ${LUA_SRCS} "thirdparty/catch.cpp")
        target_compile_features(lua_test PUBLIC cxx_std_11)
        target_include_directories(lua_test PUBLIC "src/lua")
        set_target_properties(lua_test PROPERTIES CXX_EXTENSIONS OFF)
        target_link_options(lua_test PRIVATE "-Wl,-rpath=${LUAJIT_INCLUDE_PATH}")
        target_compile_definitions(lua_test PRIVATE ASIO_HAS_THREADS)
        target_link_options(lua_test PRIVATE "-fsanitize=address")
        target_link_libraries(lua_test asyncflow luajit msgpack rapidjson ${CMAKE_DL_LIBS})
    endif(${BUILD_TEST})
endif(${BUILD_LUAJIT})

add_custom_target("distclean"
        COMMAND make clean
        COMMAND rm CMakeCache.txt Makefile cmake_install.cmake
        COMMAND rm -rf CMakeFiles)
