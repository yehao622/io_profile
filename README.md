# io_profile
Parse and analyze Strace output with Gnuplot

1. You need to make sure you already have a file with format "[your file name].prof", and assume your '*.prof' file and 'prof.cpp', 'gnuplot-iostream.h' under the same path.
2. refer to set GnuPlot from "http://www.stahlke.org/dan/gnuplot-iostream"
3.Compile 'prof.cpp' with c++ compiler(g++ assumed): "g++ -o prof_app prof.cpp"
4.Run "./prof_app *.prof" and it will generate plot then(see plot_sample.png).
