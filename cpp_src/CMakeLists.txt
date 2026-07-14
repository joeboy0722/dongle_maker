cmake_minimum_required(VERSION 3.12)
project(DongleSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 針對 MinGW 編譯器，強制靜態連結 GCC 與 C++ 標準庫，避免在乾淨的 Windows / 虛擬機上因缺少 libstdc++-6.dll 等檔案而導致 HRESULT: 0x8007007E 載入失敗
if(MINGW)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

# 在 Windows 下設定 Unicode 支援
add_compile_definitions(UNICODE _UNICODE)

# 包含標頭檔目錄
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

# 加入子目錄
add_subdirectory(maker)
add_subdirectory(reader)
