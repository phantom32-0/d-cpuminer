#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main(int argn, char *argv[]) {

    int i = 0;
    unsigned char temp[SHA_DIGEST_LENGTH];
    char buf[SHA_DIGEST_LENGTH*2];

    if ( argn != 2 ) {
        printf("Usage: %s string\n", argv[0]);
        return -1;
    }

    memset(buf, 0x0, SHA_DIGEST_LENGTH*2);
    memset(temp, 0x0, SHA_DIGEST_LENGTH);

    SHA1((unsigned char *)argv[1], strlen(argv[1]), temp);

    for (i=0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buf[i*2]), "%02x", temp[i]);
    }

    printf("SHA1 of %s is %s\n", argv[1], buf);

    return 0;

}
