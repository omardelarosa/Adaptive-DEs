build:
	mkdir bin
	cc -o bin/des common.c CoDE.c DE.c JADE.c LSHADE.c SHADE.c jDE.c main.c

clean:
	rm bin/*