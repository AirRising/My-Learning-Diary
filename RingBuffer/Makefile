RingBuffer: RingBuffer.o main.o
	gcc -o $@ $^

RingBuffer.o: RingBuffer.c
	gcc -c $< -o $@

main.o: main.c
	gcc -c $< -o $@

clean:
	rm -f RingBuffer *.o