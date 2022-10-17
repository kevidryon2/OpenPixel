stacksize=$((16777216*1))
gcc -g main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o build/main -Wl,-z,stack-size=$stacksize
