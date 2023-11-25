cmake_minimum_required (VERSION 3.12)
project ("PacketReplay" CXX)

# Set C++ Language Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add additional CMakefiles
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Enable <PackageName>_ROOT env variables
if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

# Enable Hot Reload for MSVC compilers if supported
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Set _WIN32_WINNT_WIN10
if (WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif()

# Add Boost C++
set(Boost_NO_WARN_NEW_VERSIONS 1)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.66.0 COMPONENTS chrono filesystem program_options system thread REQUIRED)
if (Boost_FOUND)
    message("Using boost: ${Boost_INCLUDE_DIRS}")
    message("Boost libraries: ${Boost_LIBRARY_DIRS}")
else()
    if (WIN32)
        message(SEND_ERROR "Unable to find boost - set `BOOST_ROOT` env variable")
    elseif(UNIX)
        message(SEND_ERROR "Unable to find libboost")
    endif()
endif()

# Add Pcap++
find_package(PcapPlusPlus REQUIRED)
if (PcapPlusPlus_FOUND)
    message("Using PcapPlusPlus: ${PcapPlusPlus_INCLUDE_DIR}")
    message("PcapPlusPlus libraries: ${PcapPlusPlus_LIBRARY_DIRS}")
else()
    message(SEND_ERROR "Unable to find Pcap++ - set `PcapPlusPlus_ROOT` env variable")
    if (WIN32)
        message(SEND_ERROR "Unable to find WinPcap | Npcap - set `PCAP_ROOT` env variable")
        message(SEND_ERROR "Unable to find WinPcap | Npcap - set `Packet_ROOT` env variable")
    elseif(UNIX)
        message(SEND_ERROR "Unable to find libpcap")
    endif()
endif()


# Add includes
set(INCLUDE_DIR "include")
include_directories(${INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${PcapPlusPlus_INCLUDE_DIR})

# Add linker directories
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${PcapPlusPlus_LIBRARY_DIRS})

# Add source to this project's executable
set(SOURCE_DIR "src")
# get all *.cpp files recursively
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/${SOURCE_DIR}/*.cpp)
# get all *.h files recursively
file(GLOB_RECURSE INCLUDE_FILES ${PROJECT_SOURCE_DIR}/${INCLUDE_DIR}/*.h)
# Add executable
add_executable (${PROJECT_NAME} ${SRC_FILES} ${INCLUDE_FILES})
message("Executable: ${PROJECT_NAME}")

# Add linked libraries
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::chrono)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::program_options)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::system)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::thread)
target_link_libraries(${PROJECT_NAME} PUBLIC PcapPlusPlus::Common++)
target_link_libraries(${PROJECT_NAME} PUBLIC PcapPlusPlus::Packet++)
target_link_libraries(${PROJECT_NAME} PUBLIC PcapPlusPlus::Pcap++)

# Add project sources - excluding main.cpp
set(TEST_SRC_FILES ${SRC_FILES})
list(REMOVE_ITEM TEST_SRC_FILES "${PROJECT_SOURCE_DIR}/${SOURCE_DIR}/main.cpp")
message("${TEST_SRC_FILES}")
# Add tests target
add_subdirectory("test")

# TODO: Add install target if needed
