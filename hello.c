#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    
    /* my process ID and total number of processes*/
    int myrank, nprocs ;
    
    /* Create child processes, each of which has its own variables.
     * From this point on, every process executes a separate copy
     * of this program.  Each process has a different process ID,
     * ranging from 0 to num_procs minus 1, and COPIES of all
     * variables defined in the program. No variables are shared.
     **/
    MPI_Init( &argc, &argv ) ;

    /* find out MY process ID, and how many processes were started. */
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank ) ;
    MPI_Comm_size( MPI_COMM_WORLD, &nprocs ) ;

    printf( "Hello from process %d of %d\n", myrank, nprocs ) ;

    /* Stop this process */
    MPI_Finalize() ;
    return 0 ;

} 
