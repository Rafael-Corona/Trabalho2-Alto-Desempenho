par = pcv-par
seq = CAD
flags = -Wall -Werror -Wextra
N_VERTICES ?= 13
N_PROC ?= 4

all: build_par build_seq
	
build_par:
	mpicc $(flags) $(par).c -o bin/$(par)
build_seq:
	gcc $(flags) $(seq).c -o bin/$(seq)
run_par: build_par
	mpirun --oversubscribe -np $(N_PROC) ./bin/$(par) $(N_VERTICES)
run_seq: build_seq
	./bin/$(seq)
time_par:
	mpicc -D TIME $(flags) $(par).c -o bin/$(par)_time
	mpirun --oversubscribe -np $(N_PROC) ./bin/$(par) $(N_VERTICES)
time_seq:
	gcc -D TIME $(flags) $(seq).c -o bin/$(seq)_time
	./bin/$(seq)_time $(N_VERTICES)
