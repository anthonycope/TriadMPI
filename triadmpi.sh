
#$ -N triadmpi

# Save any output to a file with a suffix matching the GE assigned job id

#$ -o triadmpi.output.$JOB_ID

# Tell GridEngine to merge STDERR and STDOUT into a single file.

# You may or may not want to do this depending on how you write your code.

#$ -j y

# Set the working directory to the current directory

#$ -cwd

# Set the number of cpus to 4 and a job timeout of 8 hours.

#$ -l pcpus=8,h_rt=4:00:00

# Load appropriate modules

module add compilers/gcc/4.4.7;

module add mpi/openmpi/1.6.1;

# Execute our program

time mpirun triadmpi;

# END OF FILE