outputfname = "figure.svg"
dat_file='data.dat'

# col numbers
col_cache_policy = 1
col_cache_percent = 2
col_batch_size = 3
col_unsup = 4
col_dataset = 5
col_app = 6
col_pipeline = 7
col_sample_time = 8
col_recv_time = 9
col_copy_time = 10
col_train_time = 11
col_local_time = 12
col_remote_time = 13
col_cpu_time = 14
col_local_weight = 15
col_remote_weight = 16
col_cpu_weight = 17

col_train_process_time=30

set datafile sep '\t'
set output outputfname

# set terminal svg "Helvetica,16" enhance color dl 2 background rgb "white"
set terminal svg size 1700,3400 font "Helvetica,16" enhanced background rgb "white" dl 2
set multiplot layout 18,7

set style fill solid border -2
set pointsize 1
set boxwidth 0.5 relative

set tics font ",12" scale 0.5

set rmargin 1.6
set lmargin 5.5
set tmargin 1.5
set bmargin 2

#### magic to filter expected data entry from dat file
format_str="<awk -F'\\t' '{ if(". \
                              "$".col_unsup."        ~ \"%s$\"     && ". \
                              "$".col_dataset."      ~ \"%s$\"     && ". \
                              "$".col_app."          ~ \"%s$\"     && ". \
                              "$".col_batch_size."   ~ \"%s$\"     && ". \
                              "$".col_cache_policy." ~ \"%s$\"     ". \
                              ") { print }}' ".dat_file." "
cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, policy)=sprintf(format_str, unsup, dataset, app, batch_size, policy)
##########################################################################################
step_plot_func(unsup, dataset, app, batch_size)=sprintf( \
"unsup=\"%s\"; dataset=\"%s\"; app=\"%s\"; batch_size=\"%s\"; \
set title app.\" \".dataset.\" \".unsup.\" \".batch_size offset 0,-1 ; \
set ylabel \"Copy Time(ms)\" offset 3.5,0 font \",13\" ; \
plot cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"Rep_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_copy_time)*1000) w lp ps 0.5 lw 1 lc 3 title \"selfish\" \
    ,cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"Coll_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_copy_time)*1000) w lp ps 0.5 lw 1 lc 2 title \"ours\" \
    ,cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"Cliq_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_copy_time)*1000) w lp ps 0.5 lw 1 lc 1 title \"part\" \
    ,cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"CliqDeg_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_copy_time)*1000) w lp ps 0.5 lw 1 lc 7 title \"part_deg\" \
    ,cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"Coll_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_train_time)*1000) w lp ps 0.5 lw 2 lc 4 dashtype(3,2) title \"Train Time\"; \
set for [i=1:6] multiplot next;     \
set ylabel \"Epoch Time(s)\" offset 3.5,0 font \",13\" ; \
plot cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"Rep_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_train_process_time)) w lp ps 0.5 lw 1 lc 3 title \"selfish\" \
    ,cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"Coll_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_train_process_time)) w lp ps 0.5 lw 1 lc 2 title \"ours\" \
    ,cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"CliqDeg_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_train_process_time)) w lp ps 0.5 lw 1 lc 7 title \"part_deg\" \
    ,cmd_filter_dat_by_policy(unsup, dataset, app, batch_size, \"Cliq_2\") using (column(col_cache_percent) > 0 ? column(col_cache_percent) : 1/0):(column(col_train_process_time)) w lp ps 0.5 lw 1 lc 1 title \"part\"; \
set for [i=1:7] multiplot previous;     \
    ", \
    unsup, dataset, app, batch_size)

### Key
set key inside left Left reverse top enhanced box 
set key samplen 1 spacing 1 height 0.2 width -2 font ',13' maxrows 4 center at graph 0.65, graph 0.6 noopaque

set xlabel "Cache Rate" offset 0,1.7 font ",13"
set xrange [0:]
# set xtics rotate by -45
set xtics nomirror offset 0,0.7

## Y-axis
set ylabel "Copy Time(ms)" offset 3.5,0 font ",13"
set yrange [0:]
# set ytics 0,20,100 
set ytics offset 0.5,0 #format "%.1f" #nomirror

set grid ytics

app="3HopRnd"

##############################################################
# unsupervised, large batch size
##############################################################
do for [dataset in "PR TW PA PA_U UK CF"] {
    eval(step_plot_func("True", dataset, app, "4000"))
    unset key
}
eval(step_plot_func("True", "MAG2_H", app, "1000"))
set for [i=1:7] multiplot next;
##############################################################
# unsupervised, small batch size
##############################################################
do for [dataset in "PR TW PA PA_U UK CF"] {
    eval(step_plot_func("True", dataset, app, "2000"))
}
eval(step_plot_func("True", "MAG2_H", app, "500"))
set for [i=1:7] multiplot next;


##############################################################
# supervised, large batch size
##############################################################
do for [dataset in "PR TW PA PA_U UK CF"] {
    eval(step_plot_func("False", dataset, app, "8000"))
}
# mag240 requires different batch size
eval(step_plot_func("False", "MAG2_H", app, "2000"))
set for [i=1:7] multiplot next;

##############################################################
# supervised, small batch size
##############################################################
do for [dataset in "PR TW PA PA_U UK CF"] {
    set multiplot next
}
eval(step_plot_func("False", "MAG2_H", app, "1000"))
set for [i=1:7] multiplot next;


app="2HopRnd"
##############################################################
# unsupervised, large batch size
##############################################################
do for [dataset in "PR TW PA PA_U UK CF"] {
    eval(step_plot_func("True", dataset, app, "8000"))
}
eval(step_plot_func("True", "MAG2_H", app, "4000"))
set for [i=1:7] multiplot next;

##############################################################
# unsupervised, small batch size
##############################################################
do for [dataset in "PR TW PA PA_U UK CF"] {
    eval(step_plot_func("True", dataset, app, "4000"))
}
eval(step_plot_func("True", "MAG2_H", app, "2000"))
set for [i=1:7] multiplot next;


#############################################################
# supervised, large batch size
#############################################################
do for [dataset in "PR TW PA PA_U UK CF"] {
    eval(step_plot_func("False", dataset, app, "8000"))
    unset key
}
eval(step_plot_func("False", "MAG2_H", app, "8000"))
set for [i=1:7] multiplot next;

##############################################################
# supervised, small batch size
##############################################################
set for [i=1:6] multiplot next;
eval(step_plot_func("False", "MAG2_H", app, "4000"))
set for [i=1:7] multiplot next;

set for [i=1:6] multiplot next;
eval(step_plot_func("False", "MAG2_H", app, "2000"))
set for [i=1:7] multiplot next;