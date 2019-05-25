/*
 * pointjmp.c
 *
 *  Created on: Apr 18, 2019
 *      Author: amneiht
 */

#include <mpi.h>
#include <stdio.h>

void pointjump(int id, int size);
void pointjump(int id, int size) {
	if (id == 0) {
		int n = 9;
		int luu = n;
		int mg[9];
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		mg[1] = 4;
		mg[2] = 6;
		mg[3] = 7;
		mg[4] = 2;
		mg[5] = 0;
		mg[6] = 8;
		mg[7] = 1;
		mg[8] = 5;
		for (int i = 1; i < n; i++) { // khoi tao
			int a = mg[i];
			MPI_Send(&a, 1, MPI_INT, i, 42, MPI_COMM_WORLD);
		}
		n = n - 2;
		MPI_Status st;
		int h;
		while (n > 0) {
			MPI_Recv(&h, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
			MPI_COMM_WORLD, &st);
			n--;
		}
		n = luu;
		int a = 0;
		for (int i = 1; i < n; i++) {

			MPI_Send(&a, 1, MPI_INT, i, 42, MPI_COMM_WORLD);

		}
		//MPI_Barrier(MPI_COMM_WORLD);
		//printf("cmn \n");
		return;
	} else {
		int n;
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		if (id >= n)
			return; // loai cac tien trinh thua
		MPI_Status st;
		int next, rank;
		MPI_Recv(&next, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, &st);
		rank = 1;
		int sd[2];
		int k;
		int run = 1;
		while (run > 0) {
			if (next != 0) {
				MPI_Send(&id, 1, MPI_INT, next, 42, MPI_COMM_WORLD);
				printf("id %d truyen tham so cho %d \n", id, next);
				MPI_Recv(&sd, 2, MPI_INT, next, 42, MPI_COMM_WORLD, &st);
				rank = rank + sd[0];
				next = sd[1];
				if (next == 0) {
					MPI_Send(&id, 1, MPI_INT, 0, 42, MPI_COMM_WORLD); // thong bao ket thuc
					printf("id %d ket thuc rank %d \n", id, rank);
				}
			}
			MPI_Recv(&k, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD, &st);
				printf("id %d K: %d end \n",id,k);
			if (k == 0) {
				printf("id %d end  \n", id);
				break;
			} else {
				sd[0] = rank;
				sd[1] = next;
				MPI_Send(&sd, 2, MPI_INT, k, 42, MPI_COMM_WORLD);
				printf("id %d tra gia tri cho %d \n", id, k);
			}
		}
		//MPI_Barrier(MPI_COMM_WORLD);
		printf("id :%d rank %d \n", id, rank);
		return;
	}
}

