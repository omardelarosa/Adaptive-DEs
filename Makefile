targets = common.c CoDE.c DE.c JADE.c LSHADE.c SHADE.c jDE.c
dist:
	./bin/dist.sh

build: clean dist
	cc -o dist/devo $(targets) main.c

build-lib: build
	# build DE dll
	cc -fPIC -shared -o dist/DE.so common.c DE.c

clean:
	rm -rf dist