all: des

des: des.o
	gcc -o ./c_src/des.exe ./c_src/des.o 

des.o: ./c_src/des.h
	gcc -c ./c_src/des.c -o ./c_src/des.o

clean:
	rm ./c_src/des.exe ./c_src/*.o

benchmark:
	bash bench.sh
