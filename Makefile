SHELL := bash
CC :=  mpicc 
SOURCE := main.c
CFLAG := -lm -o 
TARGET := ./test
MPIRUN := mpirun -n

# Change when core quntity is changed 
CORE_N := 4

all:
	$(CC) $(SOURCE) $(CFLAG) $(TARGET)

#generate 1000 data
inputGen:
	$(CC) input_gen.c $(CFLAG) ./input
	./input 

#run main.c
qtest: all 
	$(MPIRUN) $(CORE_N)  $(TARGET)	

thtest: inputGen qtest


collectData:  
	for i in {1..100} ; do \
		printf "%d, " $$i; \
		$(MPIRUN) $(CORE_N) $(TARGET); \
	done > "file_$(CORE_N).dat"

hello:
	$(MPIRUN) $(CORE_N) $(TARGET)

clean: 
	rm ./test
