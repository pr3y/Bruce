#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "mbedtls/pkcs5.h"
#include <Arduino.h>


String aes_decrypt(uint8_t* inputData, size_t fileSize, const String& password_str) {
    // generate key, iv, salt
    const char *password = password_str.c_str();
    unsigned char key[32];
    unsigned char iv[16];
    unsigned char salt[8] = { /* The salt used by OpenSSL during encryption */ };

    mbedtls_md_context_t md_ctx;
    mbedtls_md_init(&md_ctx);
    mbedtls_md_setup(&md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);

    // Derive key and IV using PBKDF2
    mbedtls_pkcs5_pbkdf2_hmac(&md_ctx, (unsigned char*)password, strlen(password), salt, 8, 10000, 32, key);
    mbedtls_pkcs5_pbkdf2_hmac(&md_ctx, (unsigned char*)password, strlen(password), salt, 8, 10000, 16, iv);

    mbedtls_md_free(&md_ctx);
    
    unsigned char *outputData = new unsigned char[fileSize];
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, key, 256);

    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, fileSize, iv, inputData, outputData);

    mbedtls_aes_free(&aes);
    
    // Convert the decrypted data to a string
    String decryptedText = String(outputData, fileSize);
    free(outputData);

    return decryptedText;
}
