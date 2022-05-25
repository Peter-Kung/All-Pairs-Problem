#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <mpi.h>
#include <assert.h>
#include <time.h>
#include "stack.h"

#define FROM_BOSS 1
#define FROM_WORKER 2

/* Change when command np number is changed */
//#define process_number 4 


void stackInitialize(stack_t **, int );
void stackPush(stack_t *, stack_t *, stack_t *, double, int, int);
void boss_initialize();
void boss_deallocate();
void create_arr(int , double **);
void free_arr(double **);
void enter_data(int , double **, double **);
void print_data(int , double const *);
double getDistance(double , double, double, double);
void worker(int );
void boss();

MPI_Status status;

/* Change when data size is changed */
int n = 10000; /* data number */
int process_number;
double *x = NULL, *y = NULL, *distance = NULL, *fifth = NULL;
double *ans_x = NULL, *ans_y = NULL;
stack_t *s = NULL;

#define BILLION 1000000000.0

int main(int argc, char **argv)
{

#define TIMETEST
#ifdef TIMETEST
	struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
#endif
	int rc, rank;	
	int worker_process = process_number - 1;
	
	rc = MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS) {
		printf("Initialize failed\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_number);
	
//#define SHOWRANK
#ifdef SHOWRANK

	if (rank == 0)
		printf("\nprocess number: %d\n", process_number);	
#endif

	/* if n is not multiple of core quntity, add to core's multiple */
	if (n % worker_process)
		n = worker_process * (n / worker_process + 1);

	if (rank == 0) {
		boss();
	} else {
		worker(rank);
	}
	MPI_Finalize();

#ifdef TIMETEST
	clock_gettime(CLOCK_REALTIME, &end);
	double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;


	if (rank == 0) {
		//printf("core %d: %lf\n",process_number, time_spent);
//		printf("Rank %d: The elapsed time is %lf seconds\n", rank, time_spent);
		//FILE *fp = fopen("./data/file_2.dat", "a+");
		printf("%lf\n", time_spent);
		//fclose(fp);
	}
#endif

	return 0;
}

void stackInitialize(stack_t **s, int size)
{
	*s = (stack_t *)malloc(sizeof(stack_t));
	create_arr(5, &(*s)->stack);
	(*s)->top = -1;
}

void stackPush(stack_t *s, stack_t *x, stack_t *y, double element, int index_x, int index_y)
{
	double temp[5];
	int temp_x[5], temp_y[5];
	int temp_index = -1;

	if(s->stack[s->top] <= element && s->top < 4) { 
		/* push element */
		s->stack[++(s->top)] = element;
		x->stack[++(x->top)] = index_x;
		y->stack[++(y->top)] = index_y;
	} else if (s->stack[s->top] > element) {
		/* sort by stack pop */
		while (s->top >= 0 && s->stack[s->top] > element) {
			temp[++temp_index] = s->stack[s->top];
			temp_x[temp_index] = x->stack[x->top];
			temp_y[temp_index] = y->stack[y->top];
			s->top--;
			x->top--;
			y->top--;
		}
		/* push element */
		s->stack[++(s->top)] = element;
		x->stack[++(x->top)] = index_x;
		y->stack[++(y->top)] = index_y;
		/* push origin elements into stack not over five */
		while (temp_index >= 0  && s->top < 4) {
			s->stack[++(s->top)] = temp[temp_index];
			x->stack[++(x->top)] = temp_x[temp_index];
			y->stack[++(y->top)] = temp_y[temp_index];
			temp_index--;
		}
	}
}

void boss_deallocate()
{
	free(s->stack);
	free(s);
	free_arr(&x);
	free_arr(&y);
	free_arr(&distance);
	free_arr(&fifth);
	free(ans_x);
	free(ans_y);
}

void boss_initialize(char const path[])
{
	freopen(path, "r", stdin);
	create_arr(n ,&x);
	create_arr(n, &y);
	create_arr(5 * process_number, &ans_x);
	create_arr(5 * process_number, &ans_y);
	distance = (double *)calloc(sizeof(double *) * n * n, sizeof(double));
	create_arr(5 * (process_number - 1), &fifth);
	stackInitialize(&s, 5);	
	enter_data(n, &x, &y);
	//puts("Data reading...\n");
}

void boss()
{
	int worker_process = process_number - 1;
	/* Change input file:
	 * 1. test.txt  --- generate by program
	 * 2. example.txt --- hw doc sample
         */
	boss_initialize("./example.txt");
	/* if n = 9, data_number = 9 / (4 - 1) = 3 */
	int cal_row = n / worker_process;
	
	/* Not include rank 0 process */
	for (int i = 0;i < worker_process ;++i) {
		/*
		 * rank 1: 0 ~ 2 row
		 * rank 2: 3 ~ 5 row
		 * rank 3: 6 ~ 8 row
 		 */
		MPI_Send(x, n, MPI_DOUBLE, i + 1, FROM_BOSS, MPI_COMM_WORLD);
		MPI_Send(y, n, MPI_DOUBLE, i + 1, FROM_BOSS, MPI_COMM_WORLD);
	}

	/* Collect data from worker */	
	/* return calculate data 
	 * q      : n * data_number(row) 
	 * address: distance[rank *  data_number] 
	 */

	/* result per process: 5 */
	for (int i = 0;i < worker_process ;++i) {
		MPI_Recv(&fifth[i * 5], 5, MPI_DOUBLE, i + 1, FROM_WORKER, MPI_COMM_WORLD, &status);
		MPI_Recv(&ans_x[i * 5], 5, MPI_DOUBLE, i + 1, FROM_WORKER, MPI_COMM_WORLD, &status);
		MPI_Recv(&ans_y[i * 5], 5, MPI_DOUBLE, i + 1, FROM_WORKER, MPI_COMM_WORLD, &status);
	}

//#define ASSERTCHECK
#ifdef ASSERTCHECK

	/* i: diagonal */
	for (int i = 0;i < n ;++i) {
		for (int j = i;j < n;++j) {
			int order = j + i * n;
			int reverse = j * n + i;
			assert(distance[order] == distance[reverse]);
		}
	}
	puts("Assertion testing pass");
#endif
	stack_t *index_x = NULL, *index_y = NULL;
stackInitialize(&index_x, 5);
	stackInitialize(&index_y, 5);	
	for (int i = 0;i < worker_process * 5;i++) {
		stackPush(s, index_x, index_y, fifth[i], (int)ans_x[i], (int)ans_y[i]);
//#define PRINTFIVE
#ifdef PRINTFIVE
		printf("(%d, %d): %lf\n", (int)ans_x[i], (int)ans_y[i], fifth[i]);
#endif
	}

#define RESULT
#ifdef RESULT
	for (int i = 0;i < 5;i++)
		printf("%lf (%d, %d)\n", s->stack[i], (int)index_x->stack[i], (int)index_y->stack[i]) ;
	puts("");
#endif
	free(index_x->stack);
	free(index_y->stack);
	free(index_x);
	free(index_y);
	boss_deallocate();
}

void worker(int rank)
{
	int cal_row = n / (process_number - 1);
	create_arr(n, &x);
	if (!x) {
		printf("worker %d's array x allocate failed\n", rank);
		goto failx;
	}
	create_arr(n, &y);
	if (!y) {
		printf("worker %d's array y allocate failed\n", rank);
		goto faily;
	}
	create_arr(cal_row * n, &distance);
	if (!distance) {
		printf("worker %d's array distance allocate failed\n", rank);
		goto faild;
	}
	create_arr(5, &fifth);
	if (!fifth) {
		printf("worker %d's array distance allocate failed\n", rank);
		goto failf;
	}
	
	MPI_Recv(x, n, MPI_DOUBLE, 0, FROM_BOSS, MPI_COMM_WORLD, &status);
	MPI_Recv(y, n, MPI_DOUBLE, 0, FROM_BOSS, MPI_COMM_WORLD, &status);

//#define GETDATA
#ifdef GETDATA
	printf("rank %d ", rank);
	puts("getData:");
	for (int i = 0;i < n;++i) {
		printf("x: %lf, y: %lf\n", x[i], y[i]);
	}
	puts("------------");
#endif


	/*
	 *                  0 1 2 3 4 5 6 7 8
	 * 0 (index_source)
	 * 1 (index_source)
	 * 2 (index_source)
	 */
	stack_t *index_x = NULL;
	stack_t *index_y = NULL;
	stackInitialize(&s, 5);	
	stackInitialize(&index_x, 5);	
	stackInitialize(&index_y, 5);	

	//for (int i = (session - 1) * cal_row;i < session * cal_row ;i++) { 	
	for (int i = 0;i < cal_row;i++) {
		/* index_source: left hand side number */
		int index_source = i + (rank - 1) * cal_row;
		for (int j = index_source + 1;j < n;j++) {
			distance[i*n + j] = getDistance(x[index_source], y[index_source], x[j], y[j]);
//#define DEBUG2
#ifdef DEBUG2
			//printf("%lf, %lf, %lf, %lf\n", x[index_source], y[index_source], x[j], y[j]);
			printf("%d: %lf (%d, %d)\n", rank, distance[i * n + j], index_source + 1, j + 1 );
#endif
			/* Find the smallest of five distances */
			if (distance[i * n + j] != 0.0) {
				stackPush(s, index_x, index_y, distance[i*n + j], index_source + 1, j + 1);
				
			}
			//printf("%d, %d: %10.5lf | ", index_source, j, distance[i*n + j]);
		}
	//	puts("");
	}
//#define DEBUG3
#ifdef DEBUG3
	for (int i = 0;i < 5;++i) {
		printf("%d: %lf (%d, %d)\n", rank, s->stack[i], (int)index_x->stack[i], (int)index_y->stack[i]);
	}
#endif
	MPI_Send(s->stack, 5, MPI_DOUBLE, 0, FROM_WORKER, MPI_COMM_WORLD);
	MPI_Send(index_x->stack, 5, MPI_DOUBLE, 0, FROM_WORKER, MPI_COMM_WORLD);
	MPI_Send(index_y->stack, 5, MPI_DOUBLE, 0, FROM_WORKER, MPI_COMM_WORLD);

	free(s->stack);
	free(s);

failf:
	free_arr(&distance);
faild:
	free_arr(&y);
faily:
	free_arr(&x);
failx: 
	return;
}

double getDistance(double x1, double y1, double x2, double y2)
{
	double x_dis = x1 - x2;
	double y_dis = y1 - y2;
	return sqrt(x_dis * x_dis + y_dis * y_dis);
}

void enter_data(int n, double **x, double **y)
{
	for (int i = 0;i < n;++i)
		scanf("%lf %lf", &(*x)[i], &(*y)[i]);
}

void print_data(int n, double const *x)
{
	for (int i = 0;i < n;++i) {
		printf("%lf ", x[i]);
	}
	puts("");
}

void create_arr(int size, double **ptr)
{
	*ptr = (double *)calloc(sizeof(double) * size, sizeof(double));	
	if (!*ptr) {
		puts("Fail to malloc mem");
		return ;
	}
	for (int i = 0;i < size;++i)
		(*ptr)[i] = 1E5;
}

void free_arr(double **ptr)
{
	free(*ptr);
	*ptr = NULL;
}
