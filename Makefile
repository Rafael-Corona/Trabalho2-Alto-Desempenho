par = pcv-par
seq = CAD
flags = -Wall  -Wextra -fopenmp
N_VERTICES ?= 12
N_PROC ?= 4

all: build_par build_seq
	
build_par:
	mpicc $(flags) $(par).c -o bin/$(par)
build_seq:
	gcc $(flags) $(seq).c -o bin/$(seq)
run_par: build_par
	mpirun -np $(N_PROC) ./bin/$(par) $(N_VERTICES)
run_seq: build_seq
	./bin/$(seq) $(N_VERTICES)
time_par:
	mpicc -D TIME $(flags) $(par).c -o bin/$(par)_time
	mpirun -np $(N_PROC) ./bin/$(par)_time $(N_VERTICES)
time_seq:
	gcc -D TIME $(flags) $(seq).c -o bin/$(seq)_time
	./bin/$(seq)_time $(N_VERTICES)
clean:
	rm ./data/*.stats

store:
	mv *.stats ./data
