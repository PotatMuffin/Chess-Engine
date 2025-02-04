gcc src/main.c ./include/libraylib.a -o build/main -lm -O1 -mlzcnt -mbmi2 -msse4.2 $1
gcc src/perft.c -o build/perft -O1 -mlzcnt -mbmi2 -msse4.2