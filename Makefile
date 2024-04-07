all:
	make -C ./build/

run: all
	./build/main
