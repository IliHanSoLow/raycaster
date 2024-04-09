all:
	cmake --build build

run: all
	./build/main

build_res:
	rm -rf build/*
	cmake -S . -B build
