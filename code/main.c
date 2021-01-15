/*
linux
g++ -c des.c -o des.o
g++ -c main.c -o main.o
g++ des.o main.o -o main
./main
*/

#include "des.h"
#include "stdio.h"
#include "string.h"

void getMainKey(unsigned char *main_key) {
    int mid;
    unsigned char key[10];

    // 得到key
    printf("key: \n");
    printf("enter 1 for random input\nenter 2 for manual input\n");
    scanf("%d", &mid);
    if (mid == 1) {
        generateKey(main_key);
        printf("random key: ");
        for (int i = 0; i < 8; i++) {
            printf("%c", main_key[i]);
        }
        printf("\n");
    } else {
        printf("enter the key(8char):");
        scanf("%s", key);

        for (int i = 0; i < 8; i++) {
            main_key[i] = key[i];
        }
    }
}

int main(int argc, char const *argv[]) {
    int mid;
    unsigned char main_key[8];
    getMainKey(main_key);
    generateSubKey(main_key);

    FILE *E, *C, *D;
    unsigned char message[10];

    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%c", message[i]);
    // }
    // printf("\n");
    unsigned char cip[10];
    unsigned char dec[10];

    E = fopen("enc.txt", "r");
    C = fopen("ciphertext.txt", "w");
    D = fopen("dec.txt", "w");

    printf("Before encryption:\n");
    int need = 1;
    while (mid = fread(message, sizeof(message[0]), 8, E)) {
        for (int i = mid; i < 8; i++) {
            message[i] = 8 - mid;
            need = 0;
        }
        memset(cip, 0, sizeof(cip));
        encryptionMsg(message, cip);
        fwrite(cip, sizeof(cip[0]), 8, C);

        for (int i = 0; i < 8; i++) {
            printf("%c", message[i]);
        }
    }
    if (need) {
        for (int i = 0; i < 8; i++) message[i] = 8 - mid;
        memset(cip, 0, sizeof(cip));
        encryptionMsg(message, cip);
        fwrite(cip, sizeof(cip[0]), 8, C);

        for (int i = 0; i < 8; i++) {
            printf("%c", message[i]);
        }
    }
    printf("\n");

    fclose(C);
    C = fopen("ciphertext.txt", "r");

    printf("Has been encrypted:\n");
    while (mid = fread(cip, sizeof(cip[0]), 8, C)) {
        // for(int i = mid;i < 8;i++)
        //     cip[i] = 0;
        memset(dec, 0, sizeof(dec));
        decryptionMsg(cip, dec);
        fwrite(dec, sizeof(dec[0]), 8, D);

        for (int i = 0; i < 8; i++) {
            printf("%02x", cip[i]);
        }
    }
    printf("\n");

    fclose(D);
    D = fopen("dec.txt", "r");

    printf("Decrypted messages:\n");
    while (mid = fread(dec, sizeof(dec[0]), 8, D)) {
        for (int i = 0; i < 8; i++) {
            printf("%c", dec[i]);
        }
    }
    printf("\n");

    fclose(E);
    fclose(C);
    fclose(D);
    return 0;
}
