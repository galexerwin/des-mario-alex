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
// key shifts table
static const int keyShifts[16] = {
   1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
// key permuations table
static const int keyPermutations[48] = {
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
int getBit(unsigned char *src, int pos) {
	// set mask
    uint8_t mask = 1 << (7 - pos % 8);
    // return bit at position
    return (src[pos/8] & mask) ? 1 : 0;
}
// set bit at pos
void setBit(unsigned char *dst, int pos, int val) {
	// variables
	unsigned char mask = 0x80;
	int	i;
	// iterate over mask and shift
	for (i = 0; i < (pos % 8); i++)
	   mask = mask >> 1;
	// set based on value
	if (val)
	   dst[pos / 8] = dst[pos / 8] | mask;
	else
	   dst[pos / 8] = dst[pos / 8] & (~mask);
}
// push and set bits in rotations
void pushBit(unsigned char *data, int size, int count) {
	// variables
	int  i, j, msBit, lsBit;
	// iterate over the bits
	for (i = 0; i < count; i++) {
		for (j = 0; j <= ((size - 1) / 8); j++) {
			// last bit
			lsBit = getBit(&data[i], 0);
			// set msb if at zero
			if (i == 0) {
				msBit = lsBit;
			} else {
				setBit(&data[i - 1], 7, lsBit);
			}
			// shift left by 1
			data[i] = data[i] << 1;
		}
		// set lsb
		setBit(data, size - 1, msBit);
	}
}







// perform permutation according to the table
void doPermutation(unsigned char *data, const int *pArray, int count) {
	// variables
	unsigned char temp[8];
	int i;
	// wipe memory
	memset(temp, 0, 8);
	// iterate over the bits and set according to pArray
	for (i = 0; i < count; i++)
	   setBit(&temp, i, getBit(data, pArray[i] - 1));
	// set return
	memcpy(data, temp, 8);
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
	// variables
	int i, j, k;
	unsigned char key[16] = "133457799BBCDFF1";
	unsigned char subKeys[17][3][7];
	unsigned char leftKey[4], rightKey[4];
	unsigned char *temp;
	// unhex the key
	temp = unHexifyKey(key);
	//printfB(temp);
	// perform a parity drop
	doPermutation(temp, parityDrop, 56);
	//printfB(temp);
	// set aside memory for the left and right keys
	memset(leftKey, 0, 4);
	memset(rightKey, 0, 4);
	// set the bits for each 28 bit piece
	for (i = 0; i < 28; i++) {
		setBit(leftKey, i, getBit(temp, i));
		setBit(rightKey, i, getBit(temp, i + 28));
	}
	//printfB(leftKey);
	//printfB(rightKey);
	// copy subkeys into temporary storage
	memcpy(subKeys[0][0], leftKey, 7);
	memcpy(subKeys[0][1], rightKey, 7);
	// iterate to perform rotations
	for (k = 1; k < 17; k++) {
		// rotate the keys
		pushBit(subKeys[k - 1][0], 28, keyShifts[k]);
		pushBit(subKeys[k - 1][1], 28, keyShifts[k]);
	}
	// concatenate and permutate the keys
	for (k = 1; k < 17; k++) {
		//  concatenate the keys
		for (j = 0; j < 28; j++) {
			setBit(subKeys[k][2], j, getBit(subKeys[k][0], j));
			setBit(subKeys[k][2], j + 28, getBit(subKeys[k][1], j));
		}
		// permutate the key into a 48 bit key
		doPermutation(subKeys[k][2], keyPermutations, 48);

		printfB(subKeys[k][2]);
	}
    // exit
    return 0;
} 
