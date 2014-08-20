#ifndef _RND_MT_H
#define _RND_MT_H


/* This is a simple implementation of pseudorandom numbers generator based on Mersenne-Twister algorithm.
 *
 */

#define LAST_32_BITS(x) ( (x) & 0xffffffff)

const uint32_t RND_MT_MAX = (uint32_t)(((uint64_t)1 << 32) - 1);

typedef struct random_mt_state_t{
    uint32_t MT[624];
    uint32_t index;
} random_mt_state_t;




/* Initialize the generator from a seed */
void initialize_mt_generator(int seed, random_mt_state_t *state) {
    state->index = 0;
    state->MT[0] = seed;
    for (int i = 1;  i < 624; i++) { // loop over each other element
        state->MT[i] = LAST_32_BITS(1812433253 * (state->MT[i-1] ^ ((state->MT[i-1]) >> 30)) + i); // 0x6c078965
    }
}

/* Generate an array of 624 untempered numbers */
void generate_numbers(random_mt_state_t *state)
{
    for (int i = 0; i<624; i++) {
        uint32_t y = (state->MT[i] & 0x80000000)                      // bit 31 (32nd bit) of MT[i]
            + (state->MT[(i+1) % 624] & 0x7fffffff);   // bits 0-30 (first 31 bits) of MT[...]
        state->MT[i] = state->MT[(i + 397) % 624] ^ (y >> 1);
        if ((y % 2) != 0) { // y is odd
            state->MT[i] = state->MT[i] ^ 2567483615; // 0x9908b0df
        }
    }
}

/* Get next random number */
uint32_t rnd_mt_next(random_mt_state_t *state) {
    if (state->index == 0) {
        generate_numbers(state);
    }

    uint32_t y = state->MT[state->index];
    y = y ^ (y >> 11);
    y = y ^ ((y << 7) & 2636928640); // 0x9d2c5680
    y = y ^ ((y << 15) & 4022730752); // 0xefc60000
    y = y ^ ((y>>18));

    state->index = (state->index + 1) % 624;
    return y;
}

#endif /*_RND_MT_H */
