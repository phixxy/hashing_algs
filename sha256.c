#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

uint32_t BIT_32_MASK = 0xFFFFFFFF;

uint32_t right_rotate(uint32_t word, uint32_t amount){
    amount = amount % 32;
    return ((word >> amount) | (word << (32 - amount))) & BIT_32_MASK;
}

int main()
{
    char message[151] = "";
    scanf("%[^\n]", message);

    // init hash values
    uint32_t h0 = 0x6a09e667;
    uint32_t h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372;
    uint32_t h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f;
    uint32_t h5 = 0x9b05688c;
    uint32_t h6 = 0x1f83d9ab;
    uint32_t h7 = 0x5be0cd19;


    uint32_t k[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    

    // Convert message to bytes and append "1 bit"
    int input_len = strlen(message);
    int base = input_len + 1 + 8;
    int zero_bytes_needed = 64 - ( base % 64);
    int total_bytes_msg_size = base + zero_bytes_needed;
    uint8_t byte_msg[total_bytes_msg_size];
    uint64_t L = strlen(message) * 8;
    for (int i = 0; i < total_bytes_msg_size; i++){
        if (i < input_len){
            byte_msg[i] = message[i];
        } else if (i == input_len){
            byte_msg[i] = 0b10000000;
        } else {
            byte_msg[i] = 0;
        }
    }

    // append L to end of byte_msg
    uint8_t L_bytes[8];
    for (int i = 0; i < 8; i++){
        int shift = 64 - ((i+1)*8);
        L_bytes[i] = (L >> shift) & 0xFF;
    }
    int start = total_bytes_msg_size - 8;
    for (int i = 0; i < 8; i++){
        byte_msg[start + i] = L_bytes[i];
    }


    // build chunks
    int NUM_CHUNKS = sizeof(byte_msg)/64;
    uint32_t chunks[NUM_CHUNKS][64];
    for (int chunk_num = 0; chunk_num < NUM_CHUNKS; chunk_num++){
        for (int i = 0; i < 16; i++){
            //every 4 bytes is 1 word
            int j = i + chunk_num * 16;
            uint32_t word = (byte_msg[j*4] << 24) | (byte_msg[j*4+1] << 16) | (byte_msg[j*4+2] << 8) | byte_msg[j*4+3];
            chunks[chunk_num][i] = word;

        }
    }
    // process hash
    for(int chunk_num = 0; chunk_num < NUM_CHUNKS; chunk_num++){
        for(int i = 16; i < 64; i++){

            uint32_t s0 = (right_rotate(chunks[chunk_num][i-15],
                (uint32_t)7) ^ right_rotate(chunks[chunk_num][i-15], 
                (uint32_t)18) ^ (chunks[chunk_num][i-15] >> (uint32_t)3)) & BIT_32_MASK;
            uint32_t s1 = ((right_rotate(chunks[chunk_num][i-2], 
                (uint32_t)17) ^ right_rotate(chunks[chunk_num][i-2], 
                (uint32_t)19) ^ chunks[chunk_num][i-2] >> (uint32_t)10)) & BIT_32_MASK;
            chunks[chunk_num][i] = (chunks[chunk_num][i-16] + s0 + chunks[chunk_num][i-7] + s1) & BIT_32_MASK;
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;
        uint32_t f = h5;
        uint32_t g = h6;
        uint32_t h = h7;

        for (int i = 0; i < 64; i++){
            uint32_t s1 = (right_rotate(e,6) ^ right_rotate(e,11) ^ right_rotate(e,25)) & BIT_32_MASK;
            uint32_t ch = ((e & f) ^ ((~e) & g)) & BIT_32_MASK;
            uint32_t temp1 = (h + s1 + ch + k[i] + chunks[chunk_num][i]) & BIT_32_MASK;
            uint32_t s0 = (right_rotate(a,2) ^ right_rotate(a,13) ^ right_rotate(a,22)) & BIT_32_MASK;
            uint32_t maj = ((a & b) ^ (a & c) ^ (b & c)) & BIT_32_MASK;
            uint32_t temp2 = (s0 + maj) & BIT_32_MASK;
            h = g;
            g = f;
            f = e;
            e = (d + temp1) & BIT_32_MASK;
            d = c;
            c = b;
            b = a;
            a = (temp1 + temp2) & BIT_32_MASK;
        }
        h0 = (h0 + a) & BIT_32_MASK;
        h1 = (h1 + b) & BIT_32_MASK;
        h2 = (h2 + c) & BIT_32_MASK;
        h3 = (h3 + d) & BIT_32_MASK;
        h4 = (h4 + e) & BIT_32_MASK;
        h5 = (h5 + f) & BIT_32_MASK;
        h6 = (h6 + g) & BIT_32_MASK;
        h7 = (h7 + h) & BIT_32_MASK;
    }
    // print digest
    printf("%08x%08x%08x%08x%08x%08x%08x%08x", h0,h1,h2,h3,h4,h5,h6,h7);
    return 0;
}
    