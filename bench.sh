#/bin/bash 

output_file="./benchmark.txt" ;


if [ -f "$output_file" ] ; then
	rm -f $output_file
fi ;

for i in {1..10}
do
	start_p=`date +%s.%N`
	python3 ./src/DES.py $(($i*10000))
	end_p=`date +%s.%N`
	start_c=`date +%s.%N`
	./c_src/des.exe $(($i*10000))
	end_c=`date +%s.%N`
	time_p=$(echo "$end_p - $start_p" | bc) ; 
	time_c=$(echo "$end_c - $start_c" | bc) ; 
	print_tmp="$((10000*$i)) "$time_p" "$time_c
	echo -e $print_tmp >> $output_file
done ;

# Image Generation.
gnuplot plot.gnu
