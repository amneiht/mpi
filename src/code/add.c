/*
 * add.c
 *
 *  Created on: Apr 17, 2019
 *      Author: amneiht
 *      thuat toan cong cac phan tu mang song song
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int sum(int*mg, int n);

void padd(int id, int size) {
	if (id == 0) {
		int n = 100;
		int *mg = (int*) calloc(n, sizeof(int));
		for (int i = 0; i < n; i++) {
			mg[i] = i ;
		} // khoi tao
		int s1, s2;

		int a = n / 2;
		int b = n - a;
		MPI_Send(&a, 1, MPI_INT, 1, 42, MPI_COMM_WORLD); // gui so luong
		MPI_Send(&b, 1, MPI_INT, 2, 42, MPI_COMM_WORLD); // gui so luong
		MPI_Send(mg, a, MPI_INT, 1, 42, MPI_COMM_WORLD);
		MPI_Send(mg, b, MPI_INT, 2, 42, MPI_COMM_WORLD);

		MPI_Recv(&s1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&s2, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int s = s1 + s2;
		printf("tong = %d", s);

		free(mg);
	} else {

		int sl;
		int p = (id - 1) / 2;
		MPI_Recv(&sl, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (sl == 0) {
			int s = 0;
			if ((2 * id + 1) < size)
				MPI_Send(&sl, 1, MPI_INT, (2 * id + 1), 42, MPI_COMM_WORLD);
			if ((2 * id + 2) < size)
				MPI_Send(&sl, 1, MPI_INT, (2 * id + 2), 42, MPI_COMM_WORLD);
			return;

		}

		int mg[sl];
		MPI_Recv(&mg, sl, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("id: %d sl:%d \n", id, sl);
		if (sl == 1) {
			MPI_Send(&mg, sl, MPI_INT, p, 42, MPI_COMM_WORLD);
			int s = 0;
			if ((2 * id + 1) < size)
				MPI_Send(&s, 1, MPI_INT, (2 * id + 1), 42, MPI_COMM_WORLD);
			if ((2 * id + 2) < size)
				MPI_Send(&s, 1, MPI_INT, (2 * id + 2), 42, MPI_COMM_WORLD);
		} else if (sl == 2) {
			sl = mg[1] + mg[0];
			MPI_Send(&sl, 1, MPI_INT, p, 42, MPI_COMM_WORLD);

			int s = 0;
			if ((2 * id + 1) < size)
				MPI_Send(&s, 1, MPI_INT, (2 * id + 1), 42, MPI_COMM_WORLD);
			if ((2 * id + 2) < size)
				MPI_Send(&s, 1, MPI_INT, (2 * id + 2), 42, MPI_COMM_WORLD);
		} else {
			int a = sl / 2;
			int b = sl - a;
			int s1, s2;
			// gui neu con process
			if ((2 * id + 1) < size) {
				MPI_Send(&a, 1, MPI_INT, 2 * id + 1, 42, MPI_COMM_WORLD); // gui so luong
				MPI_Send(mg, a, MPI_INT, 2 * id + 1, 42, MPI_COMM_WORLD);
			}
			if ((2 * id + 2) < size) {
				MPI_Send(&b, 1, MPI_INT, 2 * id + 2, 42, MPI_COMM_WORLD); // gui so luong
				MPI_Send(mg + a, b, MPI_INT, 2 * id + 2, 42,
				MPI_COMM_WORLD);
			}
			// nhan so luong
			if (2 * id + 1 >= size)
				s1 = sum(mg, a);
			else
				MPI_Recv(&s1, 1, MPI_INT, MPI_ANY_SOURCE,
				MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (2 * id + 2 >= size)
				s2 = sum(mg + a, b);
			else
				MPI_Recv(&s2, 1, MPI_INT, MPI_ANY_SOURCE,
				MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			sl = s1 + s2;
			MPI_Send(&sl, 1, MPI_INT, p, 42, MPI_COMM_WORLD);

		}

	}

}
int sum(int*mg, int n) {
	int res = 0;
	for (int i = 0; i < n; i++)
		res = res + mg[i];
	return res;

}
