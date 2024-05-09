# 设置cmake最小版本
cmake_minimum_required(VERSION 3.20)

# 设置项目名称
project(network_program)

# 设置C++标准17
set(CMAKE_CXX_STANDARD 17)

# 设置输出compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_subdirectory(src)

# ctest 测试
# enable_testing()
# add_subdirectory(test)