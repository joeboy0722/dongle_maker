cmake_minimum_required(VERSION 3.12)
project(DongleSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 在 Windows 下設定 Unicode 支援
add_compile_definitions(UNICODE _UNICODE)

# 包含標頭檔目錄
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

# 加入子目錄
add_subdirectory(maker)
add_subdirectory(reader)
