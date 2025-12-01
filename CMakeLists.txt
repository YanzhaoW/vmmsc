cmake_minimum_required(VERSION 3.30)

project(vmm)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(Qt6 6.9.2 REQUIRED COMPONENTS Core Gui Xml Network Widgets
                                           PrintSupport)

qt_standard_project_setup()
qt_add_executable(vmmdcs)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(images)
add_subdirectory(alglib)

target_link_libraries(vmmdcs PRIVATE Qt6::Core Qt6::Gui Qt6::Xml Qt6::Network
                                     Qt6::Widgets Qt6::PrintSupport)
target_compile_definitions(vmmdcs PRIVATE Q_MOC_RUN)
