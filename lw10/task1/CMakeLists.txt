add_executable (task1 "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.20)
  set_property(TARGET task1 PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(task1 PRIVATE quadricsolver )
target_include_directories(task1 PUBLIC "${PROJECT_SOURCE_DIR}/quadricsolver ")