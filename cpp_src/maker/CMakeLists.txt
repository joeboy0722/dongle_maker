add_library(DongleMaker SHARED
    dongle_maker.h
    dongle_maker.cpp
    ../common/crypto_helper.h
    ../common/crypto_helper.cpp
)

# 連結系統庫 Setupapi 與 bcrypt (CNG 加密)
target_link_libraries(DongleMaker PRIVATE Setupapi bcrypt)

# 定義導出標記
target_compile_definitions(DongleMaker PRIVATE DONGLE_EXPORTS)

# 測試用控制台程式
add_executable(MakerConsole maker_console.cpp)
target_link_libraries(MakerConsole PRIVATE DongleMaker)

# 建置後事件：自動複製 DLL 到 C# 專案 bin 目錄，利於直接執行
add_custom_command(TARGET DongleMaker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../../dongle_maker_gui/bin/Debug"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/../../dongle_maker_gui/bin/Release"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:DongleMaker>
        "${CMAKE_CURRENT_SOURCE_DIR}/../../dongle_maker_gui/bin/Debug/$<TARGET_FILE_NAME:DongleMaker>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:DongleMaker>
        "${CMAKE_CURRENT_SOURCE_DIR}/../../dongle_maker_gui/bin/Release/$<TARGET_FILE_NAME:DongleMaker>"
)
