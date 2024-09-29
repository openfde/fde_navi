cmake_minimum_required(VERSION 3.14)

project(fde_navi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Qt5_DIR "/opt/qt5_15_0/lib/cmake/Qt5")
# 添加qt模块
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

# 添加源文件
set(SOURCE_FILES qrc_fde_navi.cpp draggable_button.cpp main.cpp)
set(HEADER_FILES draggable_button.h)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})


# 链接qt库
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui )

