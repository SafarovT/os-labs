# CMakeList.txt: проект CMake для sys-info-crossplatform; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.

project(SecretFunction)

add_library(IsPrime STATIC "IsPrime.cpp")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (SecretFunction "SecretFunction.cpp")

target_link_libraries(SecretFunction IsPrime)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SecretFunction PROPERTY CXX_STANDARD 20)
endif()
