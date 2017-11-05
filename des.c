/* Author: Alex Erwin and Mario Aguilar
 * Purpose: Assignment 6b
 * Date: 11/2/2017
 * 
 * https://www.tutorialspoint.com/cprogramming/c_bitwise_operators.htm
 * https://stackoverflow.com/questions/14528233/bit-masking-in-c-how-to-get-first-bit-of-a-byte/14528319
 * https://stackoverflow.com/questions/7863499/conversion-of-char-to-binary-in-c
 * http://cse29-iiith.virtual-labs.ac.in/exp7/media/DES1.pdf
 */
// includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bithacks.h"

// parity drop table
static const int parityDrop[56] = {
   57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
   10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
   63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
   14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};
// key compression table
static const int keyCompression[48] = {
   14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
   23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
   41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
   44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
// initial permutation table
static const int initialPermutation[64] = {
   58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
   62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
   57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
   61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
};
// final permutation table
static const int finalPermutation[64] = {
   40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
   38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
   36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
   34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
};




// get bit at pos
int getBit(uint64_t src, int pos) {
	// set mask
    uint8_t mask = 1 << (7 - pos % 8);
    // return bit at position
    return (src[pos/8] & mask) ? 1 : 0;
}
// set bit at pos
void setBit(uint64_t *dst, int pos, int value) {
	if (value)
		*dst += (0x8000000000000000 >> pos);
}
// perform permutation according to the table
void doPermutation(uint64_t *data, const int *pArray, int count) {
	// variables
	uint64_t temp;
	int i;
	// initialize the memory segment
	memset(temp, 0, 32);
	// iterate over the bits and set according to pArray
	for (i = 0; i < count; i++)
	   setBit(temp, i, getBit(data, pArray[i] - 1));
}
// expand & permute
uint8_t * expandPBox (unsigned int v[1]) {
	// memory storage for returning the expanded p box
	unsigned int pbox[2];

	pbox[0] = v[0] & 15;
	pbox[0] = pbox[0] << 1;
	if (B_IS_SET(pbox[0], 1))
		B_TOGGLE(pbox[0], 0);
	if (B_IS_SET(pbox[0], 4))
		B_TOGGLE(pbox[0], 5);
printf("%d\n", pbox[0]);

	pbox[1] = v[0] & 240;
	pbox[1] = pbox[1] >> 3;
	if (B_IS_SET(pbox[1], 1))
		B_TOGGLE(pbox[1], 0);
	if (B_IS_SET(pbox[1], 4))
		B_TOGGLE(pbox[1], 5);
printf("%d\n", pbox[1]);
	return (uint8_t *)pbox;
}





void hexdump_to_string(const void *data, int size, char *str) {
    const unsigned char *byte = (unsigned char *)data;
    while (size > 0) {
 size--;
 sprintf(str, "%.2x ", *byte);
 byte++;
 str+=2;
    }
}

// des encryption
void des_enc(uint32_t v[2], uint32_t const key[2]) {
	
}

// des decryption
void des_dec(uint32_t v[2], uint32_t const key[2]) {
	
}
/*
 * 1011
 * 010110
 * 010111
 * 
 * 1010
 * 010100
 * 010100
 */
// convert hex to string
unsigned char * unHexifyKey(unsigned char *key) {
	// variables
	int i, j;
	unsigned char temp[2];
	unsigned char *rret;
	// make space
	rret = malloc(sizeof(char) * 8);
    // set memory to zero
	memset(rret, 0, 8);
	// iterate over the two digit hex
	for (i = 0, j = 0; i < 16; i += 2, j++) {
		// store each digit
		temp[0] = key[i];
		temp[1] = key[i + 1];
		// convert to ascii and store
		rret[j] = (int)strtol(temp, NULL, 16);
	}
	// return
	return rret;
}
// print binary
void printfB(unsigned char *text) {
	// variables
	int i, j;
	// iterate over text
	for(i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
	      		printf("%d", !!((text[i] << j) & 0x80));
	printf("\n");
}

int main(int argc, char **argv) {
    //FILE *msg_fp = fopen("message.txt", "r");
    //FILE *key_fp = fopen("key.txt", "r");
    //FILE *encrypted_msg_fp = fopen("encrypted_msg.bin", "wb");
    //FILE *decrypted_msg_fp = fopen("decrypted_msg.txt", "w");    
	unsigned char key[8] = "secret";
	//unsigned char temp[8];

	uint64_t temp = (uint64_t *)key;

	//memcpy(temp, key, 8);

	doPermutation(&temp, parityDrop, 56);

	int i;
	for (i = 64; i > 0; i--) {
		printf("%d", getBit(temp, i - 1));
	}
	printf("\n");

	//uint8_t *pbox = expandPBox((unsigned int *)in);
	
	//printf("%u, %u\n", (&pbox)[0], (&pbox)[1]);
    // read key from key file
    // read msg from msg file (msg size will exactly be size of 1 cipher block)
    // convert key as string to uint32_t
    // encrypt msg with des using �des_enc� and write encrypted message using �hexdump_to_string� to �encrypted_msg.bin�
    // decrypt encrypted msg with des using �des_dec� and write decrypted message to �decrypted_msg.txt�
    return 0;
} 
