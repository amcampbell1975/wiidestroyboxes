# cd box2d
# echo "make /box2d"
# gcc -c *.c
# cd ..

echo "make /source_x11"
gcc ./source_x11/wiidestroyboxes.c ./source_x11/draw.c ./box2d/*.o -I./box2d/ -lm -lX11 -o wiidestroyboxes -Wall
