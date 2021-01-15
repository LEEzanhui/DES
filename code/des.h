#include "stdlib.h"

void generateKey(unsigned char *key);
// 子密钥的生成
void generateSubKey(unsigned char *main_key);
void encryptionMsg(unsigned char *message_piece,
                   unsigned char *processed_piece);
void decryptionMsg(unsigned char *message_piece,
                   unsigned char *processed_piece);
