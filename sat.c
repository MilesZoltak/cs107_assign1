#include "samples/prototypes.h"
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


long signed_max(int bitwidth) {
    //make turn all bits on
    //shift them so all less than bitwidth are 0
    //negate it so that only less than bitwidth are 0
    return ~(~0 << (bitwidth - 1));
}

long signed_min(int bitwidth) {
    //looking at the # wheel you'll see max is just ~min
    //so just remove the outside negation from the signed_max function
    return ~0 << (bitwidth - 1);
}

long sat_add(long operand1, long operand2, int bitwidth) {
    int mask = 1 << (bitwidth - 1);  //nth bit from right is on, so for 4-bit # it is 1000
    int mask2 = 3 << (bitwidth - 1);
    //int carrier = (operand1 & operand2) << 1;
    long sum = (operand1 + operand2);
    //THE TRIALS
    bool overflow = false;
    //two positives added should still be positive
    if (!(operand1 & mask) && !(operand2 & mask)) {
        overflow = (sum & mask);
    //two negatives added should still be negative
    } else if ((operand1 & mask) && (operand2 & mask)) {
        overflow = (sum & mask2);
    }

    //if we overflowed (overflew???) then figure out of sum is + or - then return max/min
    if (overflow) {
        if (operand1 & mask) {
            return signed_min(bitwidth);
        } else {
            return signed_max(bitwidth);
        }
    }
    //if we made it here we have no overflow and can just return the plain old sum
    return sum;
}




// ------- DO NOT EDIT ANY CODE BELOW THIS LINE (but do add comments!)  -------


long convert_arg(const char *str, long low, long high, const char *argname) {
    char *end;
    long n = strtol(str, &end, 0);
    if (*end != '\0') {
        error(1, 0, "Invalid number '%s'", str);
    }
    if (n < low || n > high || errno == ERANGE) {
        error(1, 0, "Illegal value %s. %s must be in range [%ld, %ld]", str, argname, low, high);
    }
    return n;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        error(1, 0, "Missing argument. Please specify the bitwidth.");
    }
    
    int bitwidth = convert_arg(argv[1], 4, sizeof(long) * 8, "Bitwidth");
    long min = signed_min(bitwidth);
    long max = signed_max(bitwidth);

    if (argc < 4)  {
        printf("%d-bit signed integer range\n", bitwidth);
        printf("min: %ld   %#018lx\n", min, min);
        printf("max:  %ld   %#018lx\n", max, max);
    } else {
        long sum = convert_arg(argv[2], min, max, "Operand");
        printf("%ld ", sum);
        for (int i = 3; i < argc; i++) {
            long next = convert_arg(argv[i], min, max, "Operand");
            printf("+ %ld ", next);
            sum = sat_add(sum, next, bitwidth);
        }
        printf("= %ld\n", sum);   
    }
    return 0;
}

