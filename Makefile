all:
	gcc *.c -o sim
	@echo "\nUSE: ./sim <ram_size> <page_size> <process_size> <algorithm> <trace_file>"
	@echo "algorithm values: \n0 - FIFO; \n1 - LRU; \n2 - NRU"

clean:
	rm sim