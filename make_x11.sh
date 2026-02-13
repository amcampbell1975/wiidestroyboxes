# cd box2d
# gcc -c *.c
# cd ..

gcc ./source_x11/directory_x11.c ./source_x11/draw.c ./source/wiidestroyboxes.c ./box2d/*.o -I./box2d/ -lm -lX11 -o wiidestroyboxes -Wall

