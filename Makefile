.c.o:
	gcc -c $<
single: synthesized-single.o synchronize.o NumberControl.o
	gcc -o $@ $^
final: synthesized.o synchronize.o NumberControl.o
	gcc -o $@ $^
clean:
	rm -f *.o single final