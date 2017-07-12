cc = g++ -std=c++11
exe = calc_roadrate
obj = calc_roadrate.o

$(exe):$(obj)
	$(cc) -o $(exe) $(obj) -fopenmp -msse2

calc_roadrate.o:calc_roadrate.c
	$(cc) -c calc_roadrate.c -fopenmp -msse2

clean:
	rm -rf *.o calc_roadrate
