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
// expansion permutation table
static const int expansionPermutation[48] = {
   32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
    8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
   16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
   24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};
// final permutation table
static const int finalPermutation[64] = {
   40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
   38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
   36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
   34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
};
// sbox table
static const int sbox[8][4][16] = {
   {{14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
   { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
   { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
   {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}},

   {{15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
   { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
   { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
   {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}},

   {{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
   {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
   {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
   { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}},

   {{ 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
   {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
   {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
   { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}},

   {{ 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
   {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
   { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
   {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}},

   {{12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
   {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
   { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
   { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}},

   {{ 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
   {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
   { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
   { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}},

   {{13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
   { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
   { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
   { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}}
};
// straight pbox
static const int pbox[32] = {
   16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
    2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
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
// xor bits
void xorBit(const unsigned char *plainText, const unsigned char *subKeyText, unsigned char *xorText, int size) {
	// variables
	int i;
	// xor the plaintext and subKeyText to yield the xorText
	for (i = 0; i < size; i++) {
	   if (getBit(plainText, i) != getBit(subKeyText, i))
		  setBit(xorText, i, 1);
	   else
		  setBit(xorText, i, 0);
	}
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
/*// expand & permute
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
}*/


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

// round key generation (scrubbed because of indirection issues)
void generateKeys(unsigned char *key, unsigned char ****subKeys) {

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
	//
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
		// calculate the permutation for the key and expand into a 48 bit key
		doPermutation(subKeys[k][2], keyPermutations, 48);

		//printfB(subKeys[k][2]);
	}


	unsigned char plainText[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
	unsigned char encryptedText[8];




    int row, col, pos = 0;
	unsigned char leftText[6], rightText[6], funcText[6], xorText[6], subChar;

	doPermutation(plainText, initialPermutation, 64);
	// split source into two blocks
	memcpy(leftText, &plainText[0], 4);
	memcpy(rightText, &plainText[4], 4);
	// iterate over the subKeys we created earlier
	for (i = 1; i < 17; i++) {
		// copy right portion into our function text
		memcpy(funcText, rightText, 4);
		// calculate the permutation for the text
		doPermutation(funcText, expansionPermutation, 48);
		// apply encryption
		xorBit(funcText, subKeys[i][3], xorText, 48);
		// copy back 6 bytes/48 bits
		memcpy(xorText, funcText, 6);
		// reset pos
		pos = 0;
		// perform the 8 sbox substitutions
		for (j = 0; j < 8; j++) {
			  // get rows and colums
		      row = (getBit(funcText, (j * 6) + 0) * 2) + (getBit(funcText, (j * 6) + 5) * 1);
		      col = (getBit(funcText, (j * 6) + 1) * 8) + (getBit(funcText, (j * 6) + 2) * 4) +
		            (getBit(funcText, (j * 6) + 3) * 2) + (getBit(funcText, (j * 6) + 4) * 1);
		      // get the substitution for the 6-bit output
		      subChar = (unsigned char)sbox[j][row][col];
		      // iterate and substitute the bits in funcText with substitutes
		      for (k = 4; k < 8; k++) {
		    	 // set bits for position p using k bits in subChar
		         setBit(funcText, pos, getBit(&subChar, k));
		         // increment pos
		         pos++;
		      }
		}
		// permute the function text with the straight pbox
		doPermutation(funcText, pbox, 32);
		// xor the leftText and funcText
		xorBit(leftText, funcText, xorText, 32);
		// copy right to left for next round
		memcpy(leftText, rightText, 4);
		// set the right for the next round
		memcpy(rightText, xorText, 4);
	}
	// reassemble text pairs
	memcpy(&encryptedText[0], leftText, 4);
	memcpy(&encryptedText[1], rightText, 4);
	// permute the final result
	doPermutation(encryptedText, finalPermutation, 64);
    // string out space
    char *out = (char *)malloc(64 * sizeof(char));
    // convert to string data
    hexdump_to_string(encryptedText, 64, out);
	printf("%s", out);
    // exit
    return 0;
} 
