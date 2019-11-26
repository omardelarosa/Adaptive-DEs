targets = common.c CoDE.c DE.c JADE.c LSHADE.c SHADE.c jDE.c

build-lib: build
	# build DE dll
	cc -fPIC -shared -o build/DE.so common.c DE.c

build: clean prepare
	cc -o build/devo $(targets) main.c

prepare:
	./bin/prepare.sh

clean:
	rm -rf build