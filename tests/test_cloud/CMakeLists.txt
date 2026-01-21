find_package(Qt6 REQUIRED COMPONENTS Test Core Gui Network Sql Widgets)

# 包含必要的头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/utils
)

# 定义测试可执行文件
add_executable(test_cloud
    test_cloud.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/CloudFileNetWork.cpp
)

# 链接必要的库
target_link_libraries(test_cloud
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Sql
    Qt6::Widgets
)

# 启用测试宏
set_target_properties(test_cloud PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

# 注册测试
add_test(NAME test_cloud COMMAND test_cloud)