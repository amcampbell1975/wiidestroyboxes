# cd box2d
# echo "make /box2d"
# gcc -c *.c
# cd ..

echo "make /src"
gcc ./src/wiidestroyboxes.c ./src/draw.c ./box2d/*.o -I./box2d/ -lm -lX11 -o wiidestroyboxes -Wall
