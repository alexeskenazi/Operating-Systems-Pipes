# STATIC_FLAG := -static
STATIC_FLAG :=

all: task1 task2 
	
task1: task1.c
	gcc $(STATIC_FLAG) task1.c -o $@

task2: task2.c
	gcc $(STATIC_FLAG) task2.c -o $@

clean:
	rm -f task1 task2
