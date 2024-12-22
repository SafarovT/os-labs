# CMakeList.txt: проект CMake для snowfall; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
find_package(X11 REQUIRED)

if (X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES: ${X11_LIBRARIES}")
else ()
    message(FATAL_ERROR "X11 library not found")
endif ()

add_executable (snowfall "snowfall.cpp" "snowfall.h")
target_link_libraries(snowfall ${X11_LIBRARIES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET snowfall PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
