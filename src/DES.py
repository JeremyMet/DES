
from DES_Constants import * ;
import random ;
import time ;

class DES(object):

    @staticmethod
    def internal_encrypt_decrypt(input, key, encrypt = True):
        LSH = lambda x, shf: ((x << shf) ^ (x >> (28-shf))) & 0xfffffff ;
        key = DES.apply_permutation(CP1, key, 64)
        ret = DES.apply_permutation(IP, input, 64) ;
        C = (key >> 28) & 0xfffffff ;
        D = key & 0xfffffff ;
        ## Run the 16 rounds of DES
        L = (ret >> 32) & 0xffffffff ;
        R = ret & 0xffffffff ;
        for i in range(16):
            if encrypt:
                shift = SHIFT[i] ;
            else:
                shift = 28-SHIFT[16-i];
            C = LSH(C, shift);
            D = LSH(D, shift);
            sub_key = DES.apply_permutation(CP2, (C << 28)^D, 56) ;
            tmp_R = R ;
            R = DES.Feistel_Function(R, sub_key)^L ;
            L = tmp_R;
        L, R = R, L ;
        ret = DES.apply_permutation(IP_1, L << 32 ^ R, 64) ;
        return ret ;

    @staticmethod
    def encrypt(plain, key):
        return DES.internal_encrypt_decrypt(plain, key, encrypt = True) ;

    @staticmethod
    def decrypt(cipher, key):
        return DES.internal_encrypt_decrypt(cipher, key, encrypt = False);

    @staticmethod
    def internal_encrypt_decrypt_file_ECB(input_file, output_file, key, encrypt = True):
        fid_input = open(input_file, "rb") ;
        fid_output = open(output_file, "wb") ;
        block = [0] ;
        while(block):
            block = fid_input.read(8) ;
            block_to_write = DES.convert_byte_to_block(block) ;
            block_to_write = DES.internal_encrypt_decrypt(block_to_write, key, encrypt) ;
            block_to_write = block_to_write.to_bytes(8, "big") ;
            fid_output.write(block_to_write) ;
        fid_input.close() ;
        fid_output.close() ;


    @staticmethod
    def encrypt_file_ECB(input_file, output_file, key):
        DES.internal_encrypt_decrypt_file_ECB(input_file, output_file, key, True) ;

    @staticmethod
    def decrypt_file_ECB(input_file, output_file, key):
        DES.internal_encrypt_decrypt_file_ECB(input_file, output_file, key, False);





    @staticmethod
    def convert_byte_to_block(byte):
        ret = 0 ;
        cpt = 0 ;
        for b in byte:
            ret = ret << 8 ^ b ;
            cpt+=1 ;
        pad_len = 8-cpt ;
        for _ in range(pad_len):
            ret = ret << 8 ^ pad_len ;
        return ret ;

    @staticmethod
    def apply_permutation(P, val, len_val):
        ret = 0 ;
        for p in P:
            ret = (ret << 1) ^ ((val >> (len_val-p)) & 1) ;
        return ret ;

    @staticmethod
    def apply_SBOX(SBox, val):
        row = (((1 << 5) & val) >> 4) ^ (val & 1) ;
        column = (val & 0x1E) >> 1 ;
        return SBox[row][column] ;

    @staticmethod
    def Feistel_Function(half_block, sub_key):
        tmp = DES.apply_permutation(E, half_block, 32) ;
        tmp = tmp^sub_key ;
        ret = 0 ;
        # Iterate over all SBoxes (6-bit input to 4-bit output),
        for i in range(8):
            ret = ret << 4 ;
            sbox_input = (tmp >> ((7-i)*6)) & 0x3F ;
            sbox_output = DES.apply_SBOX(SBOX[i], sbox_input) ;
            ret = ret^sbox_output ;
        ret = DES.apply_permutation(P, ret, 32) ;
        return ret ;

if __name__ == "__main__":
    key = int("8000000000000000", 16) ;
    plain = int("DEADBEEF", 16) ;
    iteration = 1 ;

    start = time.time() ;
    cipher = plain ;
    for _ in range(iteration):
        cipher = DES.encrypt(cipher, key) ;
    end = time.time()-start ;
    print("Nb of Iter: "+str(iteration))
    print("Plain: "+str(hex(plain))) ,
    print("Key: "+str(hex(key))) ;
    print("Cipher: "+str(hex(cipher))) ;
    print("Computed in "+str(end)+" ms") ;
    print(hex(DES.decrypt(cipher, key))) ;


    DES.encrypt_file_ECB("/home/jmetairie/encrypted_files/bretagne.jpg", "/home/jmetairie/encrypted_files/bretagne_enc.jpg", key)
    DES.decrypt_file_ECB("/home/jmetairie/encrypted_files/bretagne_enc.jpg",
                         "/home/jmetairie/encrypted_files/bretagne_dec.jpg", key)




