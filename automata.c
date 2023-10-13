#include "samples/prototypes.h"
#include <errno.h>
#include <error.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define INVERTED_BOX "\e[7m \e[0m"

/* uncomment below to swap in "+" as definition of LIVE_STR if your
 * terminal doesn't play nice with inverted box
 */
#define LIVE_STR INVERTED_BOX
//#define LIVE_STR "+"
#define EMPTY_STR " "

#define NBR_MASK 7UL    //bitmask to get the 3 least significant bits (0111)
#define INDX_MASK 1UL    //bitmask for single LSB (0001)
#define LEN_GEN 63    //represents the largest index of a 64-length generation

unsigned long advance(unsigned long cur_gen, unsigned char ruleset) {
    unsigned long neighborhood;    //holds the bit configuration of any given neighborhood
    unsigned long index;    //strips all leading bits off of neighborhood
    unsigned long value;    //the value of the bit at the indexth index of ruleset
    unsigned long new_gen = 0UL;    //the long to hold the new_generation

    //loop over cur_gen
    for (int i = LEN_GEN; i >= 0; i--) {
        //if we haven't reached the very rightmost neighborhood (0 isnt the center cell)
        if (i) {
            //shift the neighborhood in question to the 3 least significant bits
            neighborhood = cur_gen >> (i - 1);
        } else {
            //shift zero to be at index 1 to pull a zero out of thin air
            neighborhood = cur_gen << 1;
        }
        //get the value of that neighborhood that represents the index of ruleset to be searched
        index = neighborhood & NBR_MASK;
        //extract the value of the bit at the indexth index of ruleset
        value = (ruleset >> index) & INDX_MASK;

        //shift new_gen over 1 bit and put in the value we just extracted at the LSB
        new_gen <<= 1;
        new_gen |= value;
    }

    //i don't have to explain this to you
    return new_gen;
}

void draw_generation(unsigned long gen) {
    // each cell in gen is drawn as LIVE_STR or EMPTY_STR depending on whether cell is on or off
    for (unsigned long b = 1UL << 63; b > 0; b = b >> 1) {
        if (gen & b) {
            printf("%s", LIVE_STR);
        } else {
            printf("%s", EMPTY_STR);
        }
    }
    //i forgot this for soooo long and it was killing me, obviously we need a linefeed at the end!
    printf("\n");

    //good job!
}




// ------- DO NOT EDIT ANY CODE BELOW THIS LINE (but do add comments!)  -------


unsigned long convert_arg(const char *str, unsigned long low, unsigned long high, const char *argname) {
    char *end;
    unsigned long n = strtoul(str, &end, 0);
    if (*end != '\0') {
        error(1, 0, "Invalid number '%s' for %s", str, argname);
    }
    if (n < low || n > high || errno == ERANGE) {
        error(1, 0, "%s %s is not in range [%#lx, %#lx]", argname, str, low, high);
    }
    return n;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        error(1, 0, "Missing argument. Please supply ruleset and optional initial generation.");
    }

    unsigned long gen_0 = 1L << 32;
    unsigned char ruleset = convert_arg(argv[1], 0, UCHAR_MAX, "ruleset");
    if (argc > 2) {
        gen_0 = convert_arg(argv[2], 0, ULONG_MAX, "initial generation");
    }

    if (ruleset == 0) {
        draw_generation(gen_0);
    } else {
        unsigned long cur_gen = gen_0;
        for (int i = 0; i < 32; i++) {
            draw_generation(cur_gen);
            unsigned long next_gen = advance(cur_gen, ruleset);
            if (next_gen == cur_gen) {
                break;
            }
            cur_gen = next_gen;
        }
    }
    return 0;
}
