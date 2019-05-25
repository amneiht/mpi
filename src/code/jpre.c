#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
void jmaster(int id, int size, int n, int **mg);
void jslave(int id, int size);
void ps(int **arr, int n); // tinh preix tuan tu
void ghepmg(int **arr, int **l, int a, int b);
void jprefix(int id, int size,int n);

void jprefix(int id, int size,int n) {
	if (id == 0) {
		int n =200;
		int* mg = (int*) calloc(n, sizeof(int));
		for (int i = 0; i < n; i++)
			mg[i] = i+1;
		jmaster(id, size, n, &mg);
		for (int i = 0; i < n; i++)
			printf("%d \n", mg[i]);
		free(mg);
	} else
		jslave(id, size);
}
void ps(int **arr, int n) {
	if (n == 1)
		return;
	for (int i = 1; i < n; i++)
		*((*arr) + i) = *((*arr) + i - 1) + *((*arr) + i);
	return;
}
void jmaster(int id, int size, int n, int **mg) {
	double start =MPI_Wtime();
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int d = n / size;
	if (d == 0)
		d++;
	int i = d, k = 1;
	while (i < n) {
		int *p = *mg + i;
		if (k != size - 1) {
			MPI_Send(p, d, MPI_INT, k, 42, MPI_COMM_WORLD);
			i = i + d;
		} else {
			MPI_Send(p, n - i, MPI_INT, k, 42, MPI_COMM_WORLD);
			i = n;
		}

		k++;
	}

	ps(mg, d);
	int max = *(*mg + d - 1);
	int rank = max, loop = 1;
	while (loop < size) {
		MPI_Barrier(MPI_COMM_WORLD);
		if (loop < size)
			MPI_Send(&rank, 1, MPI_INT, loop, 42, MPI_COMM_WORLD);
		loop = loop * 2;
	}
	MPI_Status st;
	i = 1;
	int ghep[n - (size-1) * d];
	while (i < n && i < size) {
		MPI_Recv(&k, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
				&st);
		id=st.MPI_SOURCE;
		MPI_Recv(&ghep, k, MPI_INT, id, MPI_ANY_TAG, MPI_COMM_WORLD,
						&st);
		int *gs=ghep;
		ghepmg(mg, &gs, id * d, k);
		//printf("k: %d \n",id*d);
		i++;
	}
	 start =MPI_Wtime()-start;
	 printf("time master %f \n",start);
}
void jslave(int id, int size) {
	double start =MPI_Wtime();
	int n;
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (id < n) {
		int k = n / size;
		if (k == 0)
			k++;
		if(id==size-1) k=n-k*id;
		int mg[k];
		MPI_Recv(mg, k, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
		MPI_STATUS_IGNORE);
		int *pt = mg;

		ps(&pt, k);
		int recv = 0;
		int max = mg[k - 1];
		int rank = max, loop = 1;
		while (loop < size) {
			MPI_Barrier(MPI_COMM_WORLD);
			if (id + loop < size) {
				MPI_Send(&rank, 1, MPI_INT, id + loop, 42, MPI_COMM_WORLD);
			}
			if (id - loop > -1) {
				MPI_Recv(&recv, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
				MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
				rank = rank + recv;
			}
			loop = loop * 2;
		}
		rank = rank - max;
		for(int i=0;i<k;i++)
			mg[i]=mg[i]+rank;
		MPI_Send(&k, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
		MPI_Send(mg, k, MPI_INT, 0, 42, MPI_COMM_WORLD);
	}
	 start =MPI_Wtime()-start;
	 printf("id %d time %f \n",id,start);
}
void ghepmg(int **arr, int **l, int a, int b) {
	int *mg=*arr;
	int *d=*l;
	for (int i = 0; i < b; i++) {
		mg[i+a]=d[i];
	}
}
