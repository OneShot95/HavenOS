cmake_minimum_required(VERSION 3.16)

# Desktop UI prototype. This is not a flashable OS image.
project(haven-shell VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

add_executable(haven-shell
  src/main.cpp
  src/Profile.cpp
  src/Profile.h
  src/ShellWindow.cpp
  src/ShellWindow.h
)

target_link_libraries(haven-shell PRIVATE Qt6::Widgets)

enable_testing()
add_test(NAME smoke COMMAND haven-shell)
set_tests_properties(smoke PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen;HAVEN_SMOKE=1"
)
