cmake_minimum_required(VERSION 3.1)
project(emulator LANGUAGES CXX)

add_executable(main main.cpp)

add_subdirectory(FTXUI)
target_link_libraries(main PRIVATE ftxui::screen ftxui::dom ftxui::component)
