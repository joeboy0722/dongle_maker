#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace CryptoHelper {
    // 依據 USB 唯一晶片序號與內置鹽值 (Salt)，藉由 SHA-256 衍生出 32 字節的密鑰
    bool DeriveKey(const std::string& serial, const std::string& salt, std::vector<uint8_t>& keyOut);

    // 使用 AES-256-CBC 進行對稱加密，IV 使用固定的全 0 以簡化並保證硬體防拷
    bool EncryptAES256(const std::string& plaintext, const std::vector<uint8_t>& key, std::vector<uint8_t>& ciphertextOut);

    // 使用 AES-256-CBC 進行對稱解密
    bool DecryptAES256(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& key, std::string& plaintextOut);
}
