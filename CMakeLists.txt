#
# Project Definition
#
cmake_minimum_required(VERSION 3.8.2)
project(ElectionDataAnal)

#
# Find dependencies
#
find_package(FixedJSON 0.1 REQUIRED)
find_package(DevToolsLog 0.1 REQUIRED)
find_package(OSDevTools 0.1 REQUIRED)
find_package(UtilTime 0.1 REQUIRED)
find_package(Boost 0.1 REQUIRED COMPONENTS system thread)


add_library(AdParser STATIC
    include/FacebookParser.h
    include/FacebookAd.h
    include/Index.h
    internal_includes/FacebookAdJSON.h
    internal_includes/SummmaryJSON.h
    src/FacebookParser.cpp
    src/FacebookAd.cpp
)
target_link_libraries(AdParser PUBLIC
    FixedJSON::FixedJSON
    DevToolsLog::Log
)
target_include_directories(AdParser PUBLIC
    $<BUILD_INTERFACE:${ElectionDataAnal_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(AdParser PRIVATE cxx_std_17)
set_property(TARGET AdParser PROPERTY PUBLIC_HEADER
    ${ElectionDataAnal_SOURCE_DIR}/include/FacebookAd.h
)

add_library(Db STATIC
    include/AdDb.h
    internal_includes/DbUtils.h
    internal_includes/ConfigParser.h
    internal_includes/StoredItemJSON.h
    internal_includes/StoreJSON.h
    internal_includes/IndexJSON.h
    internal_includes/DbJSON.h
    internal_includes/TimeSeriesConfigJSON.h
    include/FacebookAdKey.h
    include/FacebookAdStore.h
    include/Index.h
    include/Index.hpp
    include/IndexConfig.h
    include/Reports.h
    src/AdDb.cpp
    src/DbUtils.cpp
    src/FacebookAdKey.cpp
    src/FacebookAdStore.cpp
    src/IndexConfig.cpp
    src/Reports.cpp
)
target_link_libraries(Db PUBLIC
    AdParser
    FixedJSON::FixedJSON
    DevToolsLog::Log
)
target_include_directories(Db PUBLIC
    $<BUILD_INTERFACE:${ElectionDataAnal_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(Db PRIVATE cxx_std_17)
set_property(TARGET Db PROPERTY PUBLIC_HEADER
)

add_library(ConfigUtils STATIC
        internal_includes/ConfigPurify.h
        src/ConfigPurify.cpp
)
target_link_libraries(ConfigUtils PUBLIC
        Db
        AdParser
        FixedJSON::FixedJSON
        DevToolsLog::Log
)
target_include_directories(ConfigUtils PUBLIC
        $<BUILD_INTERFACE:${ElectionDataAnal_SOURCE_DIR}/include>
)

target_compile_features(ConfigUtils PRIVATE cxx_std_17)

add_executable(processAds src/processAds.cpp)
target_compile_features(processAds PRIVATE cxx_std_17)
target_link_libraries(processAds PUBLIC AdParser Db)

add_executable(generateAWSExport src/generateAWSExport.cpp)
target_compile_features(generateAWSExport PRIVATE cxx_std_17)
target_link_libraries(generateAWSExport PUBLIC AdParser Db)

add_executable(processDiff src/processDiff.cpp)
target_compile_features(processDiff PRIVATE cxx_std_17)
target_link_libraries(processDiff PUBLIC AdParser Db)

add_executable(generateTimeSeries src/generateTimeSeries.cpp)
target_compile_features(generateTimeSeries PRIVATE cxx_std_17)
target_link_libraries(generateTimeSeries PUBLIC AdParser Db)

add_executable(extractId src/ExtractIdDates.cpp)
target_compile_features(extractId PRIVATE cxx_std_17)
target_link_libraries(extractId PUBLIC AdParser Db)

add_executable(cfgClean src/cfgClean.cpp internal_includes/test_utils.h)
target_compile_features(cfgClean PRIVATE cxx_std_17)
target_link_libraries(cfgClean PUBLIC ConfigUtils Db AdParser GTest::GTest GTest::Main)


#
# Test Configuration
#
find_package(GTest REQUIRED)

add_executable(ad_parser test/ad_parser.cpp internal_includes/test_utils.h)
add_executable(index test/index.cpp internal_includes/test_utils.h)
add_executable(store test/store.cpp internal_includes/test_utils.h)
add_executable(adDb test/dbTests.cpp internal_includes/test_utils.h
                                     internal_includes/SummmaryJSON.h
                                     internal_includes/ReportAdsJSON.h
                                     internal_includes/AWSExport.h)
add_executable(configPurify test/testPurify.cpp internal_includes/test_utils.h internal_includes/SummmaryJSON.h internal_includes/ReportAdsJSON.h)
add_executable(testReports test/testReports.cpp internal_includes/test_utils.h)

#
# NOTE: Valgrind must be configured *before* testing is imported
#
set(MEMORYCHECK_COMMAND_OPTIONS "--gen-suppressions=all --track-origins=yes --freelist-vol=2000000000 --error-exitcode=1 --track-fds=yes --num-callers=50 --fullpath-after= --trace-children=yes --leak-check=full" )
find_program(MEMORYCHECK_COMMAND valgrind )
include (CTest)

enable_testing()
add_test(ad_parser ad_parser)
add_test(index index)
add_test(store store)
add_test(adDb adDb)
add_test(configPurify configPurify)
add_test(testReports testReports)

target_compile_features(ad_parser PRIVATE cxx_std_17)
target_link_libraries(ad_parser PUBLIC AdParser GTest::GTest GTest::Main)

target_compile_features(index PRIVATE cxx_std_17)
target_link_libraries(index PUBLIC AdParser Db GTest::GTest GTest::Main)

target_compile_features(store PRIVATE cxx_std_17)
target_link_libraries(store PUBLIC AdParser Db GTest::GTest GTest::Main)

target_compile_features(adDb PRIVATE cxx_std_17)
target_link_libraries(adDb PUBLIC AdParser Db GTest::GTest GTest::Main)

target_compile_features(configPurify PRIVATE cxx_std_17)
target_link_libraries(configPurify PUBLIC ConfigUtils Db AdParser GTest::GTest GTest::Main)

target_compile_features(testReports PRIVATE cxx_std_17)
target_link_libraries(testReports PUBLIC AdParser Db GTest::GTest GTest::Main)

#
# Configure Coverage Build
#
SET(CMAKE_CXX_FLAGS_COVERAGE
    "${GCC_DEBUG_FLAGS} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE )
SET(CMAKE_C_FLAGS_COVERAGE
    "${GCC_DEBUG_FLAGS} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE)
