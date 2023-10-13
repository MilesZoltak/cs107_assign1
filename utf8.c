#include "samples/prototypes.h"
#include <error.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define HIGH_ORDER_1 0    //high order bits for 1 byte #
#define HIGH_ORDER_2 49280    //high order bits for 2 byte #
#define HIGH_ORDER_3 14712960    //high order bits for 3 byte #
#define BITS_4 15    //equal to ...1111 for getting  first 4 bits4
#define BITS_5 31    //equal to ...0001 1111 etc.^
#define BITS_6 63    //equal to ...0011 1111 etc.^
#define BITS_7 127    //equal to ...0111 1111 etc.^
#define BITS_8 255    //equal to ...1111 1111 etc.^
#define BYTE_MASK 255 << 16

int to_utf8(unsigned short code_point, unsigned char utf8_bytes[]) {
    int checkBytes = ~0 << 7;
    int byte_count = 1;
    //check how many bytes we will need to encode the character
    while (code_point & checkBytes) {
        checkBytes = checkBytes << (byte_count + 3);    //just used byte_count's incrementation for convenience here... hopefully that's good style? it felt good to me...
        byte_count++;
    }

    //fill lowOrder and make utf8_rep (which holds the actual utf8 value of the code_point
    int lowOrder = 0;
    int utf8_rep;
    if (byte_count == 1) {
        lowOrder = BITS_7 & code_point;
        
        utf8_rep = HIGH_ORDER_1 | lowOrder;
    } else if (byte_count == 2) {
        lowOrder = BITS_5 & (code_point >> 6);
        lowOrder <<= 8;    //shift the bits over
        lowOrder |= BITS_6 & code_point;
        
        utf8_rep = HIGH_ORDER_2 | lowOrder;
    } else {
        lowOrder = BITS_4 & (code_point >> 12);   //lowOrder now = first 4 bits of code_point
        lowOrder <<= 8;    //move over for new low order bits
        lowOrder |= BITS_6 & (code_point >> 6);
        lowOrder <<= 8;    //move over for the new low order bits again
        lowOrder |= BITS_6 & code_point;

        utf8_rep = HIGH_ORDER_3 | lowOrder;
    }

    //put the bytes in the right places
    for (int i = byte_count - 1; i >= 0; i--) {
        utf8_bytes[i] = (utf8_rep >> ((byte_count - 1) - i) * 8) & BITS_8;
    }
    return byte_count;
}




// ------- DO NOT EDIT ANY CODE BELOW THIS LINE (but do add comments!)  -------

void print_utf8(unsigned short code_point, unsigned char utf8_bytes[], int utf8_bytes_length) {
    int i;
    printf("U+%04X   UTF-8 Hex:", code_point);
    for (i = 0; i < utf8_bytes_length; i++) {
       printf(" %02x", utf8_bytes[i]);
    }
    for ( ; i < 4; i++) {
        printf("   ");
    }
    utf8_bytes[utf8_bytes_length] = '\0';
    printf("Character: %s  \n", utf8_bytes);  
}

unsigned short convert_arg(const char *str, unsigned short low, unsigned short high) {
    char *end;
    long n = strtol(str, &end, 0);
    if (*end != '\0') {
        error(1, 0, "Invalid code point '%s'", str);
    }
    if (n < low || n > high) {
        error(1, 0, "Code point %s not in range [%u, %u]", str, low, high);
    }
    return n;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        error(1, 0, "Missing argument. Please supply one or more unicode code points in decimal or hex.");
    }
    
    for (int i = 1; i < argc; i++) {
        unsigned short code_point = convert_arg(argv[i], 0, USHRT_MAX);
        unsigned char utf8_bytes[4];
        int num_utf8_bytes = to_utf8(code_point, utf8_bytes);
        print_utf8(code_point, utf8_bytes, num_utf8_bytes);  
    }
    return 0;
}
