a.out : tetris.c 
	gcc -o a.out tetris.c -lncurses 

# modified

clean:
	rm a.out *.o
