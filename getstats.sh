Nruns=10
N_PROC=4


for i in $(seq 6 13); do 
for n in $(seq 1 10); do 
	./bin/CAD_time $i
	mpirun -np $N_PROC ./bin/pcv-par_time $i	
done
done
