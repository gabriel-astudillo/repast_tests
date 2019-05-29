#!/usr/bin/env gnuplot


set encoding iso_8859_1
set term pngcairo dashed size 700,500

DATAFILE="out01.txt"

#-------------
#	SIR Graph
#------------- 
	 
set output "graph.png"	
set title "Personas"
set ylabel "Cantidad"
set xlabel "Tiempo"
#set key top right 
set key outside

set datafile separator ":"

#set xrange [8000:102000]
#set xtics 10000
#set ytics 0.1
#set yrange [1.5:2.8] 
#set format x "%2.0s"
#set format y "%2.2f"

#plot DATAFILE  using 1:2 title "Susceptibles"  with linespoints lt -1 pointtype 4 pointsize 1.5
plot DATAFILE  using 1:2 title "Susceptibles"  with lines lt -1 dt 2,\
	DATAFILE  using 1:3 title "Infectados"  with lines lt -1 dt 3,\
	DATAFILE  using 1:4 title "Recuperados"  with lines lt -1 dt 4



