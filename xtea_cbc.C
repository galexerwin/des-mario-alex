/* Author: Alex Erwin
 * Purpose: Assignment 6a
 * Date: 10/30/2017
 */
// includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// hex to string
void hexdump_to_string(const void *data, int size, char *str) {
    const unsigned char *byte = (unsigned char *)data;
    while (size > 0) {
		size--;
		sprintf(str, "%.2x ", *byte);
		byte++;
		str+=2;
    }
}
// XTEA encipher
void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}
// XTEA decipher
void decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]) {
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}
// encrypt w/ CBC
uint32_t* encryptWithCBC(unsigned int num_rounds, uint32_t* v, unsigned int v_len, uint32_t const key[4], uint32_t iv[2]) {
	// variables
	int i, j;
	uint32_t *retData = malloc(sizeof(uint32_t) * v_len);
	unsigned char ivec[8];
	unsigned char data[8];
	// iv into a buffer
	memcpy(ivec, (unsigned char *)iv, 8);
	// iterate over data
	for (i = 0; i < v_len/4; i += 2) {
		// copy chunk in
		memcpy(data, (unsigned char *)(v + i), 8);
		// xor chunk with iv or previous
		for (j = 0; j < 8; j++)
			data[j] = (unsigned char)(data[j] ^ ivec[j]);
		// encrypt the data
		encipher(num_rounds, (uint32_t *)data, key);
		// store for next run
		memcpy(ivec, (unsigned char *)data, 8);
		// copy into out buffer
		memcpy(retData + i, (uint32_t *)data, 8);
		// reset data
		memset(data, 0, 8);
	}
	// return encrypted bytes
	return retData;
}
// decrypt w/ CBC
uint32_t* decryptWithCBC(unsigned int num_rounds, uint32_t* v, unsigned int v_len, uint32_t const key[4], uint32_t iv[2]) {
	// variables
	int i, j;
	uint32_t *retData = malloc(sizeof(uint32_t) * v_len);
	unsigned char ivec[8];
	unsigned char temp[8];
	unsigned char data[8];
	// iv into a buffer
	memcpy(ivec, (unsigned char *)iv, 8); 
	// iterate over data
	for (i = 0; i < v_len/4; i += 2) {
		// copy chunk in
		memcpy(data, (unsigned char *)(v + i), 8);
		// save chunk
		memcpy(temp, data, 8);
		// decrypt the data
		decipher(num_rounds, data, key);
		// xor chunk with iv or previous
		for (j = 0; j < 8; j++)
			data[j] = (unsigned char)(data[j] ^ ivec[j]);
		// store for next run
		memcpy(ivec, temp, 8);
		// copy into out buffer
		memcpy(retData + i, (uint32_t *)data, 8);
		// reset data
		memset(data, 0, 8);
	}
	// return decrypted bytes
	return retData;
}
// pad the message
void padMessage(char *msg, unsigned int msg_len, uint32_t** padded_msg, unsigned int *padded_msg_len) {
	// variables
	int i, n, unaligned = 8 - (msg_len % 8);
	// reset unaligned if necessary
	if (unaligned == 0)
		unaligned = 8;
	// set length
	*padded_msg_len = msg_len + unaligned;
	// set length
	n = msg_len + unaligned;
	// padded array
	unsigned char *padded = malloc(n * sizeof(char));
	// copy data into new
	memcpy(padded, msg, msg_len);
	// set all to zeros except final
	memset(padded + (msg_len - 1), 0, unaligned - 1);
        // set final to unaligned number
	memset(padded + (n - 1), unaligned, 1);
	// return padded message
	*padded_msg = (uint32_t *) padded;
}
// unpad the message
void unpadMessage(char **msg, unsigned int *msg_len, uint32_t* padded_msg, unsigned int padded_msg_len) {
	// temp space
	char *temp = malloc(padded_msg_len * sizeof(char));
	// set all to null
	memset(temp, 0, padded_msg_len);
	// copy message as char
	memcpy(temp, (char *)padded_msg, padded_msg_len);
	// get alignment boundary
	int aligned = (int)temp[padded_msg_len - 1];
	// temp variable
	int n = padded_msg_len - aligned;
	// more temp space
	char *temp2 = malloc(n * sizeof(char));
	// set all to null
	memset(temp2, 0, n);
	// copy message as char
	memcpy(temp2, (char *)padded_msg, n); 
	// set length
	*msg_len = n;
	// return unpadded
	*msg = (char *)temp2;
}

int main(int argc, char **argv) {
	// variables
	// files
    FILE *msg_fp;
    FILE *key_fp;
    FILE *encrypted_msg_fp;
    FILE *decrypted_msg_fp;
    // rounds
    int num_rounds;
    unsigned char iv[] = {1,4,0,9,2,0,6,1};
	char *dmsg;
	int dlen;
    // key conversion
    char key[16];
	uint32_t ckey[4];
	uint32_t *pkey = &ckey;
	// padding
	uint32_t *pmsg;
	int plen;
	// encrypted
	uint32_t *emsg;
	// other
	int fileSize;
    // check input
    if (argc == 2) {
		num_rounds = (unsigned int)atoi(argv[1]);
    } else {
		printf("Program needs 1 input - number of rounds\n");
		return 0;
    }
    num_rounds = 64;
    // open for reading
    msg_fp = fopen("message.txt", "r");
    key_fp = fopen("key.txt", "r");
    // write out response
    encrypted_msg_fp = fopen("encrypted_msg.bin", "wb");
    decrypted_msg_fp = fopen("decrypted_msg.txt", "w");
    // read key from key file
    fread(key, 16, 1, key_fp);
    // set data length 
    fseek(msg_fp, 0, SEEK_END);
    fileSize = ftell(msg_fp);
    rewind(msg_fp);
    // get the message
    char *msg = (char *)malloc((fileSize + 1) * sizeof(char));
    fread(msg, sizeof(char), fileSize, msg_fp);
    // pad message
    padMessage(msg, fileSize, &pmsg, &plen);
    // encryptWithCBC
    emsg = encryptWithCBC(num_rounds, pmsg, plen, (uint32_t *)key, (uint32_t *)iv);
    // string out space
    char *out = (char *)malloc(plen * sizeof(char));
    // convert to string data
    hexdump_to_string(emsg, plen, out); 
    // write to file
    fwrite((char *)out, sizeof(char), plen * 2, encrypted_msg_fp);
    // decryptWithCBC
    emsg = decryptWithCBC(num_rounds, emsg, plen, (uint32_t *)key, (uint32_t *)iv);		
    // unpad message
    unpadMessage(&dmsg, &dlen, emsg, plen);
    // write message to file
    fwrite((char *)dmsg, sizeof(char), dlen, decrypted_msg_fp);
    // close resources
    fclose(msg_fp);
    fclose(key_fp);
    fclose(encrypted_msg_fp);
    fclose(decrypted_msg_fp);
    // return
    return 0;
}