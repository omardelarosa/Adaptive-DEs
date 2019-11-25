targets = common.c CoDE.c DE.c JADE.c LSHADE.c SHADE.c jDE.c

build-lib: build
	# build DE dll
	cc -fPIC -shared -o dist/DE.so common.c DE.c

build: clean dist
	cc -o dist/devo $(targets) main.c

dist:
	./bin/dist.sh

clean:
	rm -rf dist