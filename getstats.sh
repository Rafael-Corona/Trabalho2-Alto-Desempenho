Nruns=10
N_PROC=4


for i in $(seq 9 13); do 
for n in $(seq 1 10); do 
	./bin/CAD_time $i
done
done


for p in $(seq 2 4); do 
for i in $(seq 9 13); do 
for n in $(seq 1 10); do 
	mpirun -np $p ./bin/pcv-par_time $i	
done
done
done

