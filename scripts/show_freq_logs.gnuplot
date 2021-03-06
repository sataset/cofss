set grid
set datafile separator ", "
file = "logs/freq_logs.csv"
stats file
N = STATS_blocks - 2

c = 299792.458
lambda = 1885.0
shift = c / lambda
#shift = 159.0410917772
pi = 3.1415926536

set title 'Frequency domain'

do for [j=0:N] {
plot file index j u (c / ($1/2/pi + shift)):2 w lines title 'Right Polarization' , \
file index j u (c / ($1/2/pi + shift)):3 w lines title 'Left Polarization'
pause -1
}
