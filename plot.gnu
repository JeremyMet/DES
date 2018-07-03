set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set key left top 
set xtics font ", 11"
set xlabel "#Iterations"
set ylabel "seconds (s)"
set term png
set output "benchmark.png"
plot "benchmark.txt" using 2:xticlabels(1) ti "Python", "benchmark.txt" using 3:xticlabels(1) ti "C"
