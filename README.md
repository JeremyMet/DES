# DES

Core DES program has been written in a C-like fashion with a lot of bit-level operators (as shifts <<, >> and so on).
For now, one can only perform ECB encryption/decryyption.
The git includes a bash script that allows the user to encrypt the whole content from a given folder recursively
(Yesh, I know, it would have been more clever to compress the directory beforehand but goal of the project was also to learn bash).

To run the bash script, type the following
```bash
./src/encrypt_folder.sh "encrypt" "./test/" "./test_des/
```
where "encrypt" can be replaced by "decrypt" depending on the purpose. First argument is the input (i.e. the folder to be encrypted) and second is the output folder path (that should not already exist).

Encryption is performed with the *0xDEADBEEF* default key and can be changed by editing encrypto_folder.sh. Moreover, one also has to define the python scripts path (in the *DES_path* variables). 

Encryption/Decryption is not that fast (85 seconds to encrypt a 1.5 MB file on a 3.6 GHz machine).

If you see any bug, don't hesitate to contact me ;-)

PS : Please not the DES constants part has been copy/paste from Robin David's git (https://github.com/RobinDavid/pydes).
