c = 299792.458
lambda = 1885.0
shift = c / lambda
#shift = 159.0410917772
pi = 3.1415926536

set term pngcairo transparent font 'Fira Sans' fontscale 2 size 1080, 1080
set datafile separator ", "
set key top left invert

file_time = "logs/time_logs.csv"

################################

stats file_time
N = STATS_blocks - 2

command = 'mkdir logs/plot_png'
system(command)
system('echo '.N)

set palette model RGB defined ( 0 'blue', 1 '#EB811B', 2 'red')

################################
set title 'Time domain'
set ylabel 'Cycle'
set xlabel 'Time [ps]'
set zlabel 'Power [W]'
set output "logs/plot_png/evo.png"
set yrange [100:2000]
set zrange [0:*]
set view 45,,

splot for [j=1:N] file_time index j u 1:(j*100):($2*$2 + $3*$3) w lines lw 4 lc palette notitle
