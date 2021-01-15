#include "des.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "table.h"

// 16组子密钥
unsigned char k[17][8];
unsigned char c[17][4];
unsigned char d[17][4];

// 初始置换
void do_initial_msg_permutation(unsigned char* msg, unsigned char* initial) {
    int shift;
    unsigned char mid_byte;

    for (int i = 0; i < 64; i++) {
        shift = initial_msg_permutation[i];
        mid_byte = 0x80 >> ((shift - 1) % 8);
        mid_byte &= msg[(shift - 1) / 8];
        mid_byte <<= ((shift - 1) % 8);

        initial[i / 8] |= (mid_byte >> i % 8);
    }
}

// PC-1置换
void do_initial_key_permutation(unsigned char* main_key) {
    int shift;
    unsigned char mid_byte;
    for (int i = 0; i < 8; i++) {
        k[0][i] = 0;
    }

    // PC-1置换
    for (int i = 0; i < 56; i++) {
        shift = initial_key_permutation[i];
        mid_byte = 0x80 >> ((shift - 1) % 8);
        mid_byte &= main_key[(shift - 1) / 8];
        mid_byte <<= ((shift - 1) % 8);

        k[0][i / 8] |= (mid_byte >> i % 8);
    }
}

// P-置换
void do_right_sub_msg_permutation(unsigned char* r, unsigned char* ser) {
    int shift;
    unsigned char mid_byte;

    for (int i = 0; i < 4; i++) {
        r[i] = 0;
    }

    for (int i = 0; i < 32; i++) {
        shift = right_sub_msg_permutation[i];
        mid_byte = 0x80 >> ((shift - 1) % 8);
        mid_byte &= ser[(shift - 1) / 8];
        mid_byte <<= ((shift - 1) % 8);

        r[i / 8] |= (mid_byte >> i % 8);
    }
}

// 循环左移
void do_shift_left(unsigned char* half, unsigned char mid_byte, int shift,
                   int index) {
    unsigned char first, second, third, fourth;
    first = mid_byte & half[0];
    second = mid_byte & half[1];
    third = mid_byte & half[2];
    fourth = mid_byte & half[3];

    half[0] <<= shift;
    half[0] |= (second >> (8 - shift));
    half[1] <<= shift;
    half[1] |= (third >> (8 - shift));
    half[2] <<= shift;
    half[2] |= (fourth >> (8 - shift));
    half[3] <<= shift;
    half[3] |= (first >> (4 - shift));
}

// 迭代
void do_iteration(unsigned char* L, unsigned char* R, int index, int mode) {
    int key_index;
    int shift;
    unsigned char mid_byte;
    unsigned char l[4], r[4], er[6], ser[4];
    memcpy(l, R, 4);
    memset(er, 0, 6);

    // 将长度为32位的串 R_i-1 作 E-扩展，得到一个48位的串 E(R_i-1)
    for (int i = 0; i < 48; i++) {
        shift = msg_expansion[i];
        mid_byte = 0x80 >> ((shift - 1) % 8);
        mid_byte &= R[(shift - 1) / 8];
        mid_byte <<= ((shift - 1) % 8);

        er[i / 8] |= (mid_byte >> i % 8);
    }

    if (mode == 1) {
        key_index = 17 - index;
    } else {
        key_index = index;
    }

    // 将 E(R_i-1) 和长度为48位的子密钥 K_i 作48位二进制串按位异或运算
    for (int i = 0; i < 6; i++) {
        er[i] ^= k[key_index][i];
    }

    unsigned char row, col;

    for (int i = 0; i < 4; i++) {
        ser[i] = 0;
    }

    // S-盒6-4转换
    col = 0;
    col |= ((er[0] & 0x78) >> 3);
    row = 0;
    row |= ((er[0] & 0x80) >> 6);
    row |= ((er[0] & 0x04) >> 2);

    ser[0] |= ((unsigned char)sBox[0][row * 16 + col] << 4);

    col = 0;
    col |= ((er[0] & 0x01) << 3);
    col |= ((er[1] & 0xE0) >> 5);
    row = 0;
    row |= (er[0] & 0x02);
    row |= ((er[1] & 0x10) >> 4);

    ser[0] |= (unsigned char)sBox[1][row * 16 + col];

    col = 0;
    col |= ((er[1] & 0x07) << 1);
    col |= ((er[2] & 0x80) >> 7);
    row = 0;
    row |= ((er[1] & 0x08) >> 2);
    row |= ((er[2] & 0x40) >> 6);

    ser[1] |= ((unsigned char)sBox[2][row * 16 + col] << 4);

    col = 0;
    col |= ((er[2] & 0x1E) >> 1);
    row = 0;
    row |= ((er[2] & 0x20) >> 4);
    row |= (er[2] & 0x01);

    ser[1] |= (unsigned char)sBox[3][row * 16 + col];

    col = 0;
    col |= ((er[3] & 0x78) >> 3);
    row = 0;
    row |= ((er[3] & 0x80) >> 6);
    row |= ((er[3] & 0x04) >> 2);

    ser[2] |= ((unsigned char)sBox[4][row * 16 + col] << 4);

    col = 0;
    col |= ((er[3] & 0x01) << 3);
    col |= ((er[4] & 0xE0) >> 5);
    row = 0;
    row |= (er[3] & 0x02);
    row |= ((er[4] & 0x10) >> 4);

    ser[2] |= (unsigned char)sBox[5][row * 16 + col];

    col = 0;
    col |= ((er[4] & 0x07) << 1);
    col |= ((er[5] & 0x80) >> 7);
    row = 0;
    row |= ((er[4] & 0x08) >> 2);
    row |= ((er[5] & 0x40) >> 6);

    ser[3] |= ((unsigned char)sBox[6][row * 16 + col] << 4);

    col = 0;
    col |= ((er[5] & 0x1E) >> 1);
    row = 0;
    row |= ((er[5] & 0x20) >> 4);
    row |= (er[5] & 0x01);

    ser[3] |= (unsigned char)sBox[7][row * 16 + col];

    do_right_sub_msg_permutation(r, ser);

    for (int i = 0; i < 4; i++) {
        r[i] ^= L[i];
    }

    for (int i = 0; i < 4; i++) {
        L[i] = l[i];
        R[i] = r[i];
    }
}

// 逆置换
void do_inverse_msg_permutation(unsigned char* msg,
                                unsigned char* processed_piece) {
    int shift;
    unsigned char mid_byte;
    for (int i = 0; i < 64; i++) {
        shift = inverse_msg_permutation[i];
        mid_byte = 0x80 >> ((shift - 1) % 8);
        mid_byte &= msg[(shift - 1) / 8];
        mid_byte <<= ((shift - 1) % 8);

        processed_piece[i / 8] |= (mid_byte >> i % 8);
    }
}

// 随机生成密钥
void generateKey(unsigned char* key) {
    for (int i = 0; i < 8; i++) {
        key[i] = rand() % 255;
    }
}

// 根据密钥生成子密钥
void generateSubKey(unsigned char* main_key) {
    int shift;
    unsigned char mid_byte;

    do_initial_key_permutation(main_key);

    // 赋值c0和d0
    for (int i = 0; i < 3; i++) {
        c[0][i] = k[0][i];
    }

    c[0][3] = k[0][3] & 0xF0;

    for (int i = 0; i < 3; i++) {
        d[0][i] = (k[0][i + 3] & 0x0F) << 4;
        d[0][i] |= (k[0][i + 4] & 0xF0) >> 4;
    }

    d[0][3] = (k[0][6] & 0x0F) << 4;

    // 循环左移一个或两个位置
    for (int i = 1; i < 17; i++) {
        for (int j = 0; j < 4; j++) {
            c[i][j] = c[i - 1][j];
            d[i][j] = d[i - 1][j];
        }

        shift = key_shift[i];
        if (shift == 1) {
            mid_byte = 0x80;
        } else {
            mid_byte = 0xC0;
        }

        do_shift_left(c[i], mid_byte, shift, i);
        do_shift_left(d[i], mid_byte, shift, i);

        // PC-2置换
        for (int j = 0; j < 48; j++) {
            shift = sub_key_permutation[j];
            if (shift <= 28) {
                mid_byte = 0x80 >> ((shift - 1) % 8);
                mid_byte &= c[i][(shift - 1) / 8];
                mid_byte <<= ((shift - 1) % 8);
            } else {
                mid_byte = 0x80 >> ((shift - 29) % 8);
                mid_byte &= d[i][(shift - 29) / 8];
                mid_byte <<= ((shift - 29) % 8);
            }

            k[i][j / 8] |= (mid_byte >> j % 8);
        }
    }
}

// 加密主过程
void encryptionMsg(unsigned char* message_piece,
                   unsigned char* processed_piece) {
    int shift;
    unsigned char mid_byte;

    unsigned char msg[8];
    memset(msg, 0, 8);
    memset(processed_piece, 0, 8);

    do_initial_msg_permutation(message_piece, msg);

    // 赋值L和R
    unsigned char L[4], R[4];
    for (int i = 0; i < 4; i++) {
        L[i] = msg[i];
        R[i] = msg[i + 4];
    }

    // 16次迭代
    int key_index;
    for (int j = 1; j <= 16; j++) {
        do_iteration(L, R, j, 0);
    }

    for (int i = 0; i < 4; i++) {
        msg[i] = R[i];
        msg[4 + i] = L[i];
    }

    // 逆置换
    do_inverse_msg_permutation(msg, processed_piece);

    return;
}

// 解密主过程
void decryptionMsg(unsigned char* message_piece,
                   unsigned char* processed_piece) {
    int shift;
    unsigned char mid_byte;

    unsigned char msg[8];
    memset(msg, 0, 8);
    memset(processed_piece, 0, 8);

    do_initial_msg_permutation(message_piece, msg);

    // 赋值L和R
    unsigned char L[4], R[4];
    for (int i = 0; i < 4; i++) {
        L[i] = msg[i];
        R[i] = msg[i + 4];
    }

    // 16次迭代
    int key_index;
    for (int j = 1; j <= 16; j++) {
        do_iteration(L, R, j, 1);
    }

    for (int i = 0; i < 4; i++) {
        msg[i] = R[i];
        msg[4 + i] = L[i];
    }

    // 逆置换
    do_inverse_msg_permutation(msg, processed_piece);

    return;
}