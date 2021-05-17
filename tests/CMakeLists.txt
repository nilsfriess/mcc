find_package(Catch2 REQUIRED)


add_executable(tests tests.cc board_t.cc)
target_link_libraries(tests PRIVATE Catch2::Catch2)
target_link_libraries(tests PUBLIC mcc)

include(CTest)
include(Catch)

catch_discover_tests(tests)