all:tst
	
tst:log.o main.o
	gcc $^ -o $@ -g
.c.o:
	gcc -c $< -o $@ 
clean:
	rm -rf *.o tst
