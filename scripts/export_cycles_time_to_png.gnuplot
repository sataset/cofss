c = 299792.458
lambda = 1885.0
shift = c / lambda
#shift = 159.0410917772
pi = 3.1415926536

set term pngcairo transparent font 'Fira Sans' fontscale 4 size 3300, 1080
set key top left invert

set datafile separator ", "

file_time = "logs/time_logs.csv"

################################

stats file_time

N = STATS_blocks - 2
command = 'mkdir logs/plot_png'
system(command)

################################

set title 'Time domain'
set xrange [*:*]
set xlabel 'Time [ps]'
set ylabel 'Power [W]'
do for [j=0:N] {
	set output 'logs/plot_png/cycle_time_'. j .'.png'
	plot \
	file_time index j u 1:($2*$2 + $3*$3) w lines lw 4 lc 'orange' title 'Right Polarization' ,\
	file_time index j u 1:($4*$4 + $5*$5) w lines lw 4 lc 'skyblue' title 'Left Polarization'
}
