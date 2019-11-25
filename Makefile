targets = common.c CoDE.c DE.c JADE.c LSHADE.c SHADE.c jDE.c
dist:
	./bin/dist.sh

build: clean dist
	cc -o dist/devo $(targets) main.c

build-lib: build
	cc -fPIC -shared -o dist/devolib.so $(targets) main.c

clean:
	rm -rf dist