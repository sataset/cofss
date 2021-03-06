set grid
set datafile separator ", "
file = "logs/time_logs.csv"
stats file
N = STATS_blocks - 2

set title 'Time domain'
#set xrange [45:55]

do for [j=0:N] {
plot file index j u 1:($2*$2 + $3*$3) w lines title 'Right Polarization' , \
file index j u 1:($4*$4 + $5*$5) w lines title 'Left Polarization'
pause -1
}
