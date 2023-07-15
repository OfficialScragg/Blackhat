main:
	gcc src/main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o build/blackhat
run: main
	./build/blackhat
clean:
	rm build/blackhat