#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "des.h"

void print_unsigned_char_array(unsigned char* array, unsigned int len_array) {
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

char apply_SBOX(unsigned char S[4][16], unsigned char val) {
	unsigned char row = (((1 << 5) & val) >> 4)^(val & 1) ;
	unsigned char column = ((val & 0x01E) >> 1) ; 
	return S[row][column] ; 
}

// len_P, len_val are given in bit (should be a multiple of 8)
unsigned char* apply_Permutation(unsigned char P[], unsigned char* val, unsigned int len_P, unsigned int len_val) {
	unsigned char* ret = (unsigned char*) malloc(sizeof(unsigned char)*(len_P >> 3)) ; 
	int i; 
	unsigned int input_char_index ; 
	unsigned int input_bit_index ;
	unsigned int output_char_index ; 
	unsigned int output_bit_index ;  
	unsigned int len_val_minus_one = len_val-1 ; 
	unsigned int input_tmp_index ; 
	for(i=0 ; i < len_P ; i++)
	{
		input_tmp_index = P[i]-1 ;
		input_char_index = (input_tmp_index >> 3) ; 
		input_bit_index = 7-(input_tmp_index & 0x7) ; 
		output_char_index = (i >> 3) ; 
		output_bit_index =  7-(i & 0x7) ; 
		ret[output_char_index] ^= ((val[input_char_index] >> (input_bit_index)) & 1) << (output_bit_index) ;
	}
	return ret ; 	
}

unsigned char* apply_Feistel(unsigned char* half_block, unsigned char* subkey) {
	// Apply the E permutation
	unsigned char* tmp = apply_Permutation(E, half_block, 48, 32) ;  
	// XOR with subkey
	int i ; 
	for(i=0 ; i < 6 ; i++)
	{
		tmp[i] ^= subkey[i] ; 
	}
	print_unsigned_char_array(tmp, 48) ;
	// Iterate over all SBOX. 
	unsigned char* ret = malloc(sizeof(unsigned char)*4) ; 
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
			ret[i >> 1] = apply_SBOX(SBOX[0], current_bits) << 4 ; 
		}
		else {
			ret[i >> 1] ^= apply_SBOX(SBOX[0], current_bits) ; 
		} 
	}
	// Finale Permutation
	free(tmp) ; 
	tmp = apply_Permutation(P, ret, 32, 32) ;
	free(ret) ; 
	ret = tmp ; 
	return ret ; 
}

// len_array is given in bits (should be a multiple of 8). 
void left_shift_array(unsigned char* array, unsigned int len_array, unsigned int shift) {

	unsigned int byte_len_array = (len_array >> 3) ; 
	unsigned int block_shift = (shift >> 3) ; 
	unsigned int bit_shift = (shift & 0x7) ; 
	unsigned char tmp ; 
	int i ; int j ; 
	// Byte granularity.
	for(j=0 ; j < block_shift ; j++) {
		tmp = array[0] ; 
		for(i=0 ; i < byte_len_array ; i++)
		{
			array[i] = array[i+1] ; 
		}
		array[byte_len_array-1] = tmp ; 
	} 
	// Bit granularity.
	for(j=0 ; j < bit_shift ; j++) {
		tmp = (array[0] >> 7) ;
		for(i=0 ; i < byte_len_array-1 ; i++)
		{
			array[i] = (array[i] << 1) ^ (array[i+1] >> 7) ;
		}
		array[byte_len_array-1] = (array[byte_len_array-1] << 1 ^ tmp) ;
	} 	
}

void main() {
	printf("SBox %i \n", apply_SBOX(SBOX[0], 0)) ; 
	char* key = malloc(6*sizeof(unsigned char)) ;
	char* half_block = malloc(4*sizeof(unsigned char)) ;  
	int i ; 
	for(i=0 ; i<6 ; i++)
	{
		key[i] = 0 ; 
	}
	for(i=0 ; i<4 ; i++)
	{
		half_block[i] = 23*i ; 
	}
	char* ret = apply_Feistel(IP, key) ; 
	print_unsigned_char_array(ret, 32) ;
	left_shift_array(ret, 32, 9) ;
	print_unsigned_char_array(ret, 32) ; 
	free(key) ; 
	free(half_block) ; 
	free(ret) ; 
	return ; 
}
