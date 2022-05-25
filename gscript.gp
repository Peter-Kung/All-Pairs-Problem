set terminal postscript color noenhanced

set title "Differenct core statistics"
set xlabel "Ordinal"
set ylabel "time spend"

set output "result.ps"

set key right top 

plot [1:100][0:3] \
	for [i = 2:4] "file_".i.".dat" \
	pointsize 1.3 linecolor i+4 \
	title "core\_".i \
	with linespoint
