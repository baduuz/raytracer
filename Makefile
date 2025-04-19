CFLAGS = -Wall -Wpedantic -Wconversion --std=c99 -ggdb -fsanitize=address
LDFLAGS = -lm

main: main.o stb_image_write.o
	clang $(CFLAGS) $(LDFLAGS) -o main main.o stb_image_write.o

main.o: main.c camera.h log.h pixel.h ray.h types.h
	clang -c $(CFLAGS) -o main.o main.c

stb_image_write.o: stb_image_write.c stb_image_write.h
	clang -c --std=c99 -ggdb -fsanitize=address -o stb_image_write.o stb_image_write.c


.PHONY: clean
clean:
	$(RM) main main.o stb_image_write.o
