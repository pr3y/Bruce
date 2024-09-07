
#include <Arduino.h>
#include <MD5Builder.h>


String xorEncryptDecrypt(const String &input, const String &password) {
  uint8_t md5Hash[16];
  
  MD5Builder md5;
  md5.begin();
  md5.add(password);
  md5.calculate();
  md5.getBytes(md5Hash);  // Store MD5 hash in the output array
  
  String output = input;  // Copy input to output for modification
  for (size_t i = 0; i < input.length(); i++) {
    output[i] = input[i] ^ md5Hash[i % 16];  // XOR each byte with the MD5 hash
  }

  return output;
}



String encryptString(String& plaintext, const String& password_str) {
  // TODO: add "XOR" header
  return xorEncryptDecrypt(plaintext, password_str);
}

String decryptString(String& cypertext, const String& password_str) {
  return xorEncryptDecrypt(cypertext, password_str);
}
