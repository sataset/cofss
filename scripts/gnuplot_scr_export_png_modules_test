c = 299792.458
lambda = 1885.0
shift = c / lambda
#shift = 159.0410917772
pi = 3.1415926536

set term png
set datafile separator ", "

file_freq = "logs/freq_logs.csv"
file_time = "logs/time_logs.csv"

stats file_freq
set title 'Frequency domain'

N = STATS_blocks - 2
command = 'mkdir plot_module_test'
system(command)

do for [j=0:N] {
set output 'plot_module_test/plot_freq_cycle_' . j . '.png'
plot file_freq index j u (c / ($1/2/pi + shift)):2 w lines title 'Right Polarization' , \
file_freq index j u (c / ($1/2/pi + shift)):3 w lines title 'Left Polarization'
}

stats file_time
set title 'Time domain'

do for [j=0:N] {
set output 'plot_module_test/plot_time_cycle_' . j . '.png'
plot file_time index j u 1:(sqrt($2*$2 + $3*$3)) w lines title 'Right Polarization' , \
file_time index j u 1:(sqrt($4*$4 + $5*$5)) w lines title 'Left Polarization'
}


#set terminal gif animate delay 100
#set output 'foobar.gif'