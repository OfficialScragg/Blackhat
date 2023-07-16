main:
	gcc src/editor.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o build/blackhat-editor
run: main
	./build/blackhat-editor
clean:
	rm build/blackhat-editor