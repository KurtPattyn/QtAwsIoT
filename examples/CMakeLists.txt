find_package(Qt5Mqtt)

add_executable(pubsub pubsub.cpp)

target_link_libraries(pubsub Qt5::Mqtt ${TARGET_NAME})
target_include_directories(pubsub PRIVATE ${CMAKE_SOURCE_DIR}/src)

