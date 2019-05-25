/*
 * piplesort.c
 *
 *  Created on: Apr 19, 2019
 *      Author: amneiht
 */

#include <mpi.h>
#include <stdio.h>
#define end -123123
void prasort(int id);
void prasort(int id) {
	if (id == 0) {
		int n = 10;
		int mg[n + 1];
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		// khoi tao
		for (int i = 1; i <= n; i++) {
			mg[i] = 13 - i;
		}
		for (int i = 1; i <= n; i++) {
			int *m = mg + i;
			MPI_Send(m, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
		}
		// ket thuc tien trinh
		int d = end;
		MPI_Status st;

		MPI_Send(&d, 1, MPI_INT, 1, 42, MPI_COMM_WORLD);
		for (int i = 1; i <= n; i++) {
					printf(" gia tri i:%d chua sap sep %d\n",i, mg[i]);
				}
		printf(" \n");
		for (int i = 1; i <= n; i++) {

			MPI_Recv(&d, 1, MPI_INT, i, MPI_ANY_TAG,
			MPI_COMM_WORLD, &st);
			mg[i] = d;
			printf(" gia tri i:%d sau sap xep %d \n",i, mg[i]);
		}

	} else {
		int n;

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if (id > n)
			return; // loai cac tien trinh thua
		else {
			int so = end;
			MPI_Status st;
			int nhan;
			int temp;
			while (1) {
				MPI_Recv(&nhan, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
				MPI_COMM_WORLD, &st);
				if (nhan == end) {
					if (id < n) {
						int d = end;
						MPI_Send(&d, 1, MPI_INT, id + 1, 42, MPI_COMM_WORLD);
					}
					break;
				}
				if (so == end)
					so = nhan; // luu lai so
				else {
					if (nhan < so) {
						temp = so;
						so = nhan;
						nhan = temp;
					}
					MPI_Send(&nhan, 1, MPI_INT, id + 1, 42, MPI_COMM_WORLD); // gui cho nut tiep;

				}
			}
			printf("id %d gia tri %d \n", id, so);
			MPI_Send(&so, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
		}
	}
}



