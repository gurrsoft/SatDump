cmake_minimum_required(VERSION 3.0.0)
project(dscovr_support)

file(GLOB_RECURSE dscovr_support_CPPS *.cpp)
add_library(dscovr_support SHARED ${dscovr_support_CPPS})
target_link_libraries(dscovr_support PUBLIC satdump_core)
target_include_directories(dscovr_support PUBLIC src)

install(TARGETS dscovr_support DESTINATION lib/satdump/plugins)