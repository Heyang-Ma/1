# CMakeLists.txt - 简易战旗游戏项目配置
cmake_minimum_required(VERSION 3.16)
project(SimpleTacticalGame VERSION 1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找SFML库
find_package(SFML 3 COMPONENTS Graphics Window System QUIET)

# 如果没找到SFML 3，尝试SFML 2
if(NOT SFML_FOUND)
    find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
endif()

# 添加可执行文件
add_executable(TacticalGame
    main.cpp
    Unit.cpp
    GameMap.cpp
    Game.cpp
    FSM_AI.cpp
    BehaviorTree_AI.cpp
)

# 链接SFML库
if(SFML_FOUND)
    target_link_libraries(TacticalGame PRIVATE sfml-graphics sfml-window sfml-system)
else()
    target_link_libraries(TacticalGame PRIVATE sfml-graphics sfml-window sfml-system)
endif()

# 复制资源文件到构建目录
file(COPY ${CMAKE_SOURCE_DIR}/../TBSGame/Assets DESTINATION ${CMAKE_BINARY_DIR})

# 设置输出目录
set_target_properties(TacticalGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
