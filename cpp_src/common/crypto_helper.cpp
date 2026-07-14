#include "crypto_helper.h"
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

namespace CryptoHelper {

    // 依據 USB 唯一晶片序號與內置鹽值 (Salt)，藉由 SHA-256 衍生出 32 字節的密鑰
    bool DeriveKey(const std::string& serial, const std::string& salt, std::vector<uint8_t>& keyOut) {
        BCRYPT_ALG_HANDLE hAlg = NULL;
        BCRYPT_HASH_HANDLE hHash = NULL;
        NTSTATUS status = 0;
        DWORD cbData = 0, cbHash = 0;
        std::string input = serial + salt;

        // 開啟 SHA256 演算法提供者
        status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
        if (!NT_SUCCESS(status)) return false;

        // 取得雜湊長度
        status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0);
        if (!NT_SUCCESS(status) || cbHash != 32) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        keyOut.resize(cbHash);

        // 建立雜湊物件
        status = BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);
        if (!NT_SUCCESS(status)) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 導入資料進行雜湊運算
        status = BCryptHashData(hHash, (PBYTE)input.data(), (ULONG)input.size(), 0);
        if (!NT_SUCCESS(status)) {
            BCryptDestroyHash(hHash);
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 輸出 SHA-256 結果
        status = BCryptFinishHash(hHash, keyOut.data(), (ULONG)keyOut.size(), 0);
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);

        return NT_SUCCESS(status);
    }

    // 使用 AES-256-CBC 進行對稱加密，IV 使用固定的全 0 以簡化並保證硬體防拷
    bool EncryptAES256(const std::string& plaintext, const std::vector<uint8_t>& key, std::vector<uint8_t>& ciphertextOut) {
        BCRYPT_ALG_HANDLE hAlg = NULL;
        BCRYPT_KEY_HANDLE hKey = NULL;
        NTSTATUS status = 0;
        DWORD cbData = 0, cbKeyObject = 0, cbBlockLen = 0, cbCiphertext = 0;

        // 開啟 AES 演算法提供者
        status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
        if (!NT_SUCCESS(status)) return false;

        // 設定 Chaining Mode 為 CBC
        status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
        if (!NT_SUCCESS(status)) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 取得 Key Object 長度需求
        status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbKeyObject, sizeof(DWORD), &cbData, 0);
        if (!NT_SUCCESS(status)) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 取得 Block 長度
        status = BCryptGetProperty(hAlg, BCRYPT_BLOCK_LENGTH, (PBYTE)&cbBlockLen, sizeof(DWORD), &cbData, 0);
        if (!NT_SUCCESS(status) || cbBlockLen != 16) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        std::vector<uint8_t> keyObject(cbKeyObject);
        // 生成對稱金鑰
        status = BCryptGenerateSymmetricKey(hAlg, &hKey, keyObject.data(), cbKeyObject, (PBYTE)key.data(), (ULONG)key.size(), 0);
        if (!NT_SUCCESS(status)) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 準備全 0 的 IV 向量
        std::vector<uint8_t> iv(cbBlockLen, 0);

        // 1. 第一次呼叫：估算加密後的密文緩衝區大小 (使用 BCRYPT_BLOCK_PADDING 做 PKCS7 填補)
        status = BCryptEncrypt(hKey, (PBYTE)plaintext.data(), (ULONG)plaintext.size(), NULL, iv.data(), (ULONG)iv.size(), NULL, 0, &cbCiphertext, BCRYPT_BLOCK_PADDING);
        if (!NT_SUCCESS(status)) {
            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        ciphertextOut.resize(cbCiphertext);
        // CNG 加密會更改傳入的 IV 內容，所以第二次加密前需重新將 IV 重設為全 0
        std::fill(iv.begin(), iv.end(), 0);

        // 2. 第二次呼叫：真正進行加密
        status = BCryptEncrypt(hKey, (PBYTE)plaintext.data(), (ULONG)plaintext.size(), NULL, iv.data(), (ULONG)iv.size(), ciphertextOut.data(), (ULONG)ciphertextOut.size(), &cbData, BCRYPT_BLOCK_PADDING);

        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);

        return NT_SUCCESS(status);
    }

    // 使用 AES-256-CBC 進行對稱解密
    bool DecryptAES256(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& key, std::string& plaintextOut) {
        BCRYPT_ALG_HANDLE hAlg = NULL;
        BCRYPT_KEY_HANDLE hKey = NULL;
        NTSTATUS status = 0;
        DWORD cbData = 0, cbKeyObject = 0, cbBlockLen = 0, cbPlaintext = 0;

        // 開啟 AES 演算法提供者
        status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
        if (!NT_SUCCESS(status)) return false;

        // 設定 Chaining Mode 為 CBC
        status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
        if (!NT_SUCCESS(status)) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 取得 Key Object 長度
        status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbKeyObject, sizeof(DWORD), &cbData, 0);
        if (!NT_SUCCESS(status)) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 取得 Block 長度
        status = BCryptGetProperty(hAlg, BCRYPT_BLOCK_LENGTH, (PBYTE)&cbBlockLen, sizeof(DWORD), &cbData, 0);
        if (!NT_SUCCESS(status) || cbBlockLen != 16) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        std::vector<uint8_t> keyObject(cbKeyObject);
        // 生成對稱金鑰
        status = BCryptGenerateSymmetricKey(hAlg, &hKey, keyObject.data(), cbKeyObject, (PBYTE)key.data(), (ULONG)key.size(), 0);
        if (!NT_SUCCESS(status)) {
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        // 準備全 0 的 IV 向量
        std::vector<uint8_t> iv(cbBlockLen, 0);

        // 1. 第一次呼叫：估算解密後的明文緩衝區大小
        status = BCryptDecrypt(hKey, (PBYTE)ciphertext.data(), (ULONG)ciphertext.size(), NULL, iv.data(), (ULONG)iv.size(), NULL, 0, &cbPlaintext, BCRYPT_BLOCK_PADDING);
        if (!NT_SUCCESS(status)) {
            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return false;
        }

        std::vector<uint8_t> plaintext(cbPlaintext);
        // 重新將 IV 重設為全 0
        std::fill(iv.begin(), iv.end(), 0);

        // 2. 第二次呼叫：真正進行解密
        status = BCryptDecrypt(hKey, (PBYTE)ciphertext.data(), (ULONG)ciphertext.size(), NULL, iv.data(), (ULONG)iv.size(), plaintext.data(), (ULONG)plaintext.size(), &cbData, BCRYPT_BLOCK_PADDING);

        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);

        if (NT_SUCCESS(status)) {
            plaintextOut.assign((char*)plaintext.data(), cbData);
            return true;
        }
        return false;
    }
}
