add_library(DongleReader SHARED
    dongle_reader.h
    dongle_reader.cpp
    ../common/crypto_helper.h
    ../common/crypto_helper.cpp
)

# 連結系統庫 Setupapi 與 bcrypt (CNG 加密)
target_link_libraries(DongleReader PRIVATE Setupapi bcrypt)

# 定義導出標記
target_compile_definitions(DongleReader PRIVATE DONGLE_EXPORTS)

# 測試用控制台程式
add_executable(ReaderConsole reader_console.cpp)
target_link_libraries(ReaderConsole PRIVATE DongleReader Setupapi)
