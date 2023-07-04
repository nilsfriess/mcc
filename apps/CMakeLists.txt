add_executable(engine engine.cc)
target_link_libraries(engine PRIVATE mcc)

add_executable(perft perft.cc)
target_link_libraries(perft PRIVATE mcc)
