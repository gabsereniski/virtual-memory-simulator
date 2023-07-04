all:
	gcc globals.c queue.c mmu.c main.c -o sim
	gcc input_generator.c -o input_generator

sim:
	gcc globals.c queue.c mmu.c main.c -o sim

trace:
	gcc input_generator.c -o input_generator

clean:
	rm -f sim
	rm -f input_generator
	rm -f trace*