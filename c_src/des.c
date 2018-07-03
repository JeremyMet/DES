#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "des.h"

void print_unsigned_char_array_bin(unsigned char* array, unsigned int len_array) {
	int i ;
	int j ; 
	unsigned int byte_len_array = (len_array >> 3) ; 
	for(i=0 ; i < byte_len_array ; i++) {
		for (j=7 ; j >= 0 ; j--) {
			printf("%i", (array[i] >> j) &1) ; 
		}
	}
	printf("\n") ; 
}

void print_unsigned_char_array_hex(unsigned char* array, unsigned int len_array) {
	int i ;
	int j ; 
	unsigned int byte_len_array = (len_array >> 3) ; 
	printf("0x") ; 
	for(i=0 ; i < byte_len_array ; i++) {
		printf("%x", array[i]) ; 				
	}
	printf("\n") ; 
}

char apply_SBOX(unsigned char S[4][16], unsigned char val) {
	unsigned char row = (((1 << 5) & val) >> 4)^(val & 1) ;
	unsigned char column = ((val & 0x01E) >> 1) ; 
	return S[row][column] ; 
}

// len_P, len_val are given in bit (should be a multiple of 8)
void apply_Permutation(unsigned char P[], unsigned char* val, unsigned int len_P, unsigned int len_val, unsigned char* ret) {
	int i; 
	unsigned int input_char_index ; 
	unsigned int input_bit_index ;
	unsigned int output_char_index ; 
	unsigned int output_bit_index ;  
	unsigned int len_val_minus_one = len_val-1 ; 
	unsigned int input_tmp_index ; 
	// clean ret
	for(i=0;i<(len_P>>3);i++)
	{
		ret[i] = 0 ; 
	}
	// apply permutation
	for(i=0 ; i < len_P ; i++)
	{
		input_tmp_index = P[i]-1 ;
		input_char_index = (input_tmp_index >> 3) ; 
		input_bit_index = 7-(input_tmp_index & 0x7) ; 
		output_char_index = (i >> 3) ; 
		output_bit_index =  7-(i & 0x7) ; 
		ret[output_char_index] ^= ((val[input_char_index] >> (input_bit_index)) & 1) << (output_bit_index) ;
	}
}

void apply_Feistel(unsigned char* half_block, unsigned char* subkey, unsigned char* ret) {
	// Apply the E permutation
	unsigned char* tmp = malloc(sizeof(unsigned char)*6) ;
	unsigned char* tmp_ret = malloc(sizeof(unsigned char)*4) ;  
	apply_Permutation(E, half_block, 48, 32, tmp) ;  
	// XOR with subkey
	int i ; 
	for(i=0 ; i < 6 ; i++)
	{
		tmp[i] ^= subkey[i] ; 
	}
	// Iterate over all SBOX. 
	unsigned char current_bits ; 
	unsigned char start = 0 ; 
	unsigned char end = 5 ;
	unsigned char index_0 ; 
	unsigned char index_1 ; 
	for(i=0 ; i <= 7 ; i++) {
		index_0 = start >> 3 ; 
		index_1 = end >> 3 ; 
		if (index_0 == index_1)
		{
			current_bits = tmp[index_0] >> (7-(end&0x7)) & 0x3F ; 			 
		}
		else
		{
			current_bits = tmp[index_0] ;
			current_bits = current_bits << ((end&0x7)+1) & 0x3F ; 
			current_bits ^= (tmp[index_1] >> (7-(end&0x7)))  ; 
			current_bits = current_bits ; 
		}
		start += 6 ; 
		end += 6 ;  
		if ((i & 1) == 0) {
			tmp_ret[i >> 1] = apply_SBOX(SBOX[i], current_bits) << 4 ; 
		}
		else {
			tmp_ret[i >> 1] ^= apply_SBOX(SBOX[i], current_bits) ; 
		} 
	}
	// Finale Permutation
	apply_Permutation(P, tmp_ret, 32, 32, ret) ;
	free(tmp) ; 
	free(tmp_ret) ; 
}

// len_array is given in bits ; 
// key is 56 bits. 
void key_shift(unsigned char* key, unsigned int shift) {

	unsigned char tmp_0 ;
	unsigned char tmp_1 ;  
	int i ; int j ; 
	// Bit granularity.
	for(j=0 ; j < shift ; j++) {
		tmp_0 = (key[0] >> 7) & 1 ;
		tmp_1 = (key[3] >> 3) & 1 ;
		for(i=0 ; i < 7 ; i++)
		{
			key[i] = (key[i] << 1) ^ (key[i+1] >> 7) ;
		}
		key[3] = (key[3] & 0xEF)^(tmp_0 << 4) ; 
		key[6] ^= tmp_1 ; 
	} 	
}

void internal_encrypt_decrypt(unsigned char* input, unsigned char* key, unsigned char* ret)
{
	// Variables
	int i ; 
	unsigned char* permuted_key = malloc(sizeof(unsigned char)*7) ;
	unsigned char* permuted_input = malloc(sizeof(unsigned char)*8) ; 
	unsigned char* L = malloc(sizeof(unsigned char)*4) ; 
	unsigned char* R = malloc(sizeof(unsigned char)*4) ;
	unsigned char* tmp_R = malloc(sizeof(unsigned char)*4) ;  
	unsigned char* subkey = malloc(sizeof(unsigned char)*6) ; // 48 bits subkey
	// Initialisation. 
	apply_Permutation(CP1, key, 56, 64, permuted_key) ; 
	apply_Permutation(IP, input, 64, 64, permuted_input) ; 
	for(i=0;i<4;i++) {
		L[i] = permuted_input[i] ; 
		R[i] = permuted_input[i+4] ; 
	}
	// Iterating over the 16 rounds of the DES algorithm. 
	int round ; 
	for(round=0 ; round < 16 ; round++) {
  		key_shift(permuted_key, SHIFT[round]) ; 
		apply_Permutation(CP2, permuted_key, 48, 56, subkey) ; 
		// Apply Feistel
		apply_Feistel(R, subkey, tmp_R) ; 
		for(i=0;i<4;i++) {
			tmp_R[i] ^= L[i] ; 
			L[i] = R[i] ; 
			R[i] = tmp_R[i] ; 
		}
	}
	// L, R = R, L 
	// We use the permuted_input array to store R || L before applying the last permutation
	for(i=0;i<4;i++)
	{
		permuted_input[i] = R[i] ; 
		permuted_input[i+4] = L[i] ;  
	}
	apply_Permutation(IP_1, permuted_input, 64, 64, ret) ; // We are done. 
	// free tmp variables
	free(permuted_key) ; 
	free(permuted_input) ; 
	free(L) ; 
	free(R) ; 
	free(tmp_R) ; 
	free(subkey) ; 
	
}


void main() {

	unsigned char* key = malloc(8*sizeof(unsigned char)) ;
	unsigned char* input = malloc(8*sizeof(unsigned char)) ;  
	unsigned char* ret = malloc(8*sizeof(unsigned char)) ;  
	int i ;
	key[0] = 0x80 ;  
	for(i=1 ; i<8 ; i++)
	{
		key[i] = 0 ; 
	}
	for(i=0 ; i<8 ; i++)
	{
		input[i] = 0 ; 
	}
	internal_encrypt_decrypt(input, key, ret) ; 
	print_unsigned_char_array_hex(ret, 64) ; 
	free(key) ; 
	free(input) ; 
	free(ret) ; 

	return ; 
}
