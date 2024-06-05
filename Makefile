do: all clean
all:
	gcc ./src/*.c -o ./build/arh
clean:
	rm ./src/*.o -f