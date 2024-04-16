all:
	gcc ./src/*.c -o ./build/arh
	clean
clean:
	rm ./src/*.o