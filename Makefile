CFLAGS= -g -Wall -Wpedantic  

a: intal_sampletest.o intal.o
	gcc $(CFLAGS) $^ -o a

intal_sampletest.o: intal_sampletest.c intal.h
	gcc $(CFLAGS) -c $<  -o intal_sampletest.o

intal.o: intal.c 
	gcc $(CFLAGS) -c $<

clean:
	rm -rf *.o

run:
	./a
