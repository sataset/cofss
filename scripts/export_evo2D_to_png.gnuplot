c = 299792.458
lambda = 1885.0
shift = c / lambda
#shift = 159.0410917772
pi = 3.1415926536

set term pngcairo transparent font 'Fira Sans' fontscale 2 size 540, 1080
set datafile separator ", "
set key top left invert

file_time = "logs/time_logs.csv"

################################

#stats file_time
#N = STATS_blocks - 2
command = 'mkdir logs/plot_png'
#system(command)
#system('echo '.N)

#EB811B
#set palette model RGB defined ( 0 '#589BDD', 1 '#F1F23B', 2 'red')

################################
set title 'Time domain'
set ylabel 'Cycle'
set xlabel 'Time [ps]'
set zlabel 'Power [W]'
set output "logs/plot_png/evo.png"
set xrange[0:100] noreverse nowriteback
set yrange[0:2000] noreverse nowriteback
set logscale zcb
unset cbtics

#set cbrange[1E-2:1000]
set palette defined ( 0 "blue", 3 "cyan", 4 "yellow", 5 "red" )

#set pm3d
#set hidden3d
#set view map
#splot for [j=0:N] file_time index j u 1:($0):($2*$2 + $3*$3) w pm3d palette lw 19 notitle

plot file_time u 1:(50*floor($0/8192)):($2*$2 + $3*$3) w image notitle
