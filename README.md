(transfer to /private)

# DES

## Python Implementation

DES core program has been written in a C-like fashion with a lot of bit-level operators (as shifts <<, >> and so on).
For now, one can only perform ECB encryption/decryption.
The git includes a bash script that allows the user to encrypt/decrypt the content of a given folder recursively
(Yesh, I know, it would have been more clever to compress the directory beforehand but goal of the project was also to learn some bash).

To run the bash script, type the following
```bash
./src/encrypt_folder.sh "encrypt" "./test/" "./test_des/
```
where "encrypt" can be replaced by "decrypt" depending on the purpose. First argument is the input (i.e. the folder to be encrypted) and second is the output folder path (that should not already exist).

Encryption is performed with the *0xDEADBEEF* default key and can be changed by editing encrypt_folder.sh. Moreover, one also has to define the python scripts path (in the *DES_path* variables). 

Encryption/Decryption is not that fast (85 seconds to encrypt a 1.5 MB file on a 3.6 GHz machine).

If you meet any bug, don't hesitate to contact me ;-)

PS : Please note the DES constants part has been copied/pasted from Robin David's git (https://github.com/RobinDavid/pydes).

## C Implementation

The C implementation was written in order to compare performances (compiled language versus interpreted language). The implementation was thought for 8-bit architecture, i.e. all operations are performed with byte granularity (no use of 16-bit or 32-bit integers or even bigger). Thus, this code could be utilized on small Arduino (as the Nano for instance). This is no surprise to observe a "gap". We observed (at least) a 10 speedup factor in favor of C. See below: 

<img align="center" src=https://raw.githubusercontent.com/JeremyMet/DES/master/benchmark.png alt="benchmark">


A DES is called #iterations times (from 10,000 to 100,000 with a 10,000 steps). Benchmark consists in computing DES(DES(...(DES(0x0))) where DES is called #iterations times. Of course, we obtain a linear behaviour depending on #iterations.
Overall, a single DES computation time is reported in the following table.
<p align="center">
  
|                          | Python (Pentium @ 3.6 GHz) | C (Pentium @ 3.6 GHz)     | Arduino Nano (atmega328p @ 16 MHz)
|--------------------------|--------|-------|-------|
|  DES Execution Time (ms) |  &asymp; 0.268 | &asymp; 0.009 | &asymp; 63,8 |

</p>

Benchmarks were run on a 3.6 GHz machine/Arduino Nano @ 16 MHz.
I was quite amazed by the Nano performance !

I don't really know how to "interpret" the results. This "gap" was actually expected. However, it is trully substantial. Maybe, I did not code in a "pythonic" way, plus I'm quite sure Python does not "like" bit-level operations that much.

PS : Valgrind approved :-)

~~## What about saving the round keys once for all (in case DES is called several times with the same key) ?~~

~~Yes, it needs to be done (for both implementations).~~

## Makefile

Run make in the main folder to compile the C code (gcc).
Type make benchmark to run the benchmark. Note the bench requires gnuplot for graph generation.
