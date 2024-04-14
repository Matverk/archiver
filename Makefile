all:
	gcc ./src/*.c -o ./build/main
	clean
clean:
	rm ./src/*.o