par = pcv-par
seq = CAD

all: build_par build_seq
	
build_par:
	mpicc $(par).c -o bin/$(par)
build_seq:
	gcc $(seq).c -o bin/$(seq)
run_par: build_par
	mpirun --oversubscribe -np 8 ./bin/$(par)
run_seq: build_seq
	./bin/$(seq)
