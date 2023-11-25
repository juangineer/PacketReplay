cmake_minimum_required (VERSION 3.12)
project("PacketReplayTest" CXX)

# Add Boost C++
set(Boost_NO_WARN_NEW_VERSIONS 1)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.66.0 COMPONENTS system unit_test_framework)

if (${Boost_unit_test_framework_FOUND} STREQUAL "ON")
    # Add unit test sources
    set(SOURCE_DIR ".")
    # get all *.cpp files recursively
    file(GLOB_RECURSE TEST_FILES ${PROJECT_SOURCE_DIR}/${SOURCE_DIR}/*.cpp)
    # Add test executable
    add_executable (${PROJECT_NAME} ${TEST_FILES} ${TEST_SRC_FILES})
    message("Test Executable: ${PROJECT_NAME}")

    # Add linked libraries
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::unit_test_framework)
    target_link_libraries(${PROJECT_NAME} PUBLIC PcapPlusPlus::Common++)
    target_link_libraries(${PROJECT_NAME} PUBLIC PcapPlusPlus::Packet++)
    target_link_libraries(${PROJECT_NAME} PUBLIC PcapPlusPlus::Pcap++)

    # testing command
    enable_testing()
    add_test(${PROJECT_NAME} ${PROJECT_NAME})
else()
    message("Skipping Test Executable")
endif()
