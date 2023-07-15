# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
main:
	gcc src/main.c -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o build/blackhat && echo "\n\e[0;32mBuilt successfully!\e[0;37m"

run: main
	./build/blackhat && echo "\n\e[0;32mExecuted successfully!\e[0;37m"

clean:
	rm build/blackhat && echo "\e[0;32mAll cleaned up.\e[0;37m"
