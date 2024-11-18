# CMakeList.txt: проект CMake для sys-info-crossplatform; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.

project(sys-info-crossplatform)

add_library(SysInfo STATIC "SysInfo.cpp")


# Добавьте источник в исполняемый файл этого проекта.
add_executable (sys-info-crossplatform "sys-info-crossplatform.cpp" "sys-info-crossplatform.h")

target_link_libraries(sys-info-crossplatform SysInfo)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sys-info-crossplatform PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
