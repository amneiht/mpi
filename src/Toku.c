#include <bits/types/FILE.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Lmax 1000000 ;
int check(int **so, int **ps, int p);
int* thongke(int **sd, int p, int *z);
int* kt(int max, int tam);
void fill(int **m, int p, int k);
void center(int **cs, int** s, int n, int **mt, int tam);
void kmean(int id, int size);
void kmaster(int **l, int n, int max, int size, int tam);
void update(int**cs, int**s, int h, long**al, int**st);
int readfile(int **ng, int *lg, int *mx);
int tammoi(int tam, int **mt, long **al, int**st);
void kslave(int id, int size);

int main(int argc, char** argv) {
	int size, id;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id); //what rank is the current processor
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	kmean(id, size);
	MPI_Finalize();
	return 0;
}
void kmean(int id, int size) {
	if (id == 0) {
		int *mg;
		int len, max;
		int tam = 5;
		int err = readfile(&mg, &len, &max);
		if (err == -1) {
			MPI_Bcast(&err, 1, MPI_INT, 0, MPI_COMM_WORLD);
			return;
		}
		kmaster(&mg, len, max, size, tam);
		free(mg);
	} else
		kslave(id, size);
}
void kmaster(int **l, int n, int max, int size, int tam) {
	int id = 0;
	int *mg = *l; // chuyen mang
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int p = n / size;

	int *sort, sd[2];
	sort = (int*) malloc(p * 2 * sizeof(int));
	fill(&sort, 2 * p, -1);
	for (int i = 0; i < n; i++) {
		sd[1] = mg[i * 2 + 1];
		sd[0] = mg[2 * i];
		int *ps = sd;
		int l = check(&sort, &ps, p); // kiem tra thu tu trong nang
		if (l == 1)
			MPI_Send(&sd, 2, MPI_INT, 1, 42, MPI_COMM_WORLD);
	}
	sd[0] = -1; // ket thuc
	MPI_Send(&sd, 2, MPI_INT, 1, 42, MPI_COMM_WORLD);

	printf("id %d co %d phan tu \n",id,p);
	int h; // kich thuoc mang moi
	int *csort = thongke(&sort, p, &h); // x,y, sl
	int *mtam = kt(max, tam); //x,y
	MPI_Bcast(&tam, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(mtam, tam * 2, MPI_INT, 0, MPI_COMM_WORLD);
	int* sl = (int*) malloc(h * sizeof(int)); // chi so tam cum
	int *stam = (int*) malloc(tam * sizeof(int)); // so luong moi tam
	long *all = (long*) malloc(tam * 2 * sizeof(long));
	long *nhan = (long*) malloc(tam * 2 * sizeof(long)); // nhan tong so tam
	int *nhansl = (int*) malloc(tam * sizeof(int)); // nhan so luong tam

	while (1) {
		for (int i = 0; i < tam * 2; i++) {
			all[i] = 0; // reset du lieu
		}
		center(&csort, &sl, h, &mtam, tam);
		fill(&stam, tam, 0);
		update(&csort, &sl, h, &all, &stam);

		if (1 < size) {
			//printf("id %d nhan tu %d \n", id, 2 * id + 1);
			MPI_Recv(nhan, tam * 2, MPI_LONG, id * 2 + 1, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(nhansl, tam, MPI_INT, id * 2 + 1, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int i = 0; i < tam; i++) {
				//printf("tam %ld %ld %d \n",nhan[i * 2],nhan[i * 2 + 1],nhansl[i]);
				all[i * 2] = all[i * 2] + nhan[i * 2]; //x
				all[i * 2 + 1] = all[i * 2 + 1] + nhan[i * 2 + 1]; //y
				stam[i] = stam[i] + nhansl[i];
			}
		}
		if (2 < size) {
			//printf("id %d nhan tu %d \n", id, 2 * id + 2);
			MPI_Recv(nhan, tam * 2, MPI_LONG, id * 2 + 2, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(nhansl, tam, MPI_INT, id * 2 + 2, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int i = 0; i < tam; i++) {
				all[i * 2] = all[i * 2] + nhan[i * 2]; //x
				all[i * 2 + 1] = all[i * 2 + 1] + nhan[i * 2 + 1]; //y
				stam[i] = stam[i] + nhansl[i];
			}
		}
		int cm = tammoi(tam, &mtam, &all, &stam);
		MPI_Bcast(mtam, tam * 2, MPI_INT, 0, MPI_COMM_WORLD);
		if (cm == 0)
			break;
		//printf("cmn \n");
	}
	//printf("%d\n", size);
	int ma = 0, m = h;
	if (1 < size) {
		MPI_Recv(&ma, 1, MPI_INT, 1, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (ma > m)
			m = ma;
	}
	if (2 < size) {
		MPI_Recv(&ma, 1, MPI_INT, 2, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (ma > m)
			m = ma;
	}

	//printf("max %d\n", m);
	for (int i = 0; i < h; i++) // gan gia tri bang tam
			{
		csort[3 * i + 2] = sl[i];
	}
//	 tao tep

	FILE *fl;
	fl = fopen("/home/amneiht/Desktop/anh/data3", "w");
	if (fl != NULL) {
		fprintf(fl, "1 %d %d\n", n, max);
		for (int i = 0; i < h; i++) {
			fprintf(fl, "%d %d %d\n", csort[i * 3], csort[i * 3 + 1],
					csort[3 * i + 2]);
		}

		int* mghi = (int*) malloc(m * 3 * sizeof(int));// gia tri lon nnhat

		for (int j = 1; j < size; j++) {
			MPI_Recv(&ma, 1, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD,
			MPI_STATUS_IGNORE);
			MPI_Recv(mghi, ma * 3, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD,
			MPI_STATUS_IGNORE);
			for (int i = 0; i < ma; i++) {
				fprintf(fl, "%d %d %d\n", csort[i * 3], csort[i * 3 + 1],
						csort[3 * i + 2]);
			}
		}
		free(mghi);
	}

	fclose(fl);
	free(sort);
	free(csort);
	free(mtam);
	free(stam);
	free(sl);
	free(all);
	free(nhan);
	free(nhansl);
}
void kslave(int id, int size) {
	int n;
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int p = n / size;
	if (id == size - 1)
		p = n - id * p; // tach do co so le
	int sd[2];
	int *sort = (int*) malloc(sizeof(int) * p * 2);
	fill(&sort, p * 2, -1);
	printf("id %d co %d phan tu \n",id,p);
	while (1) {
		MPI_Recv(&sd, 2, MPI_INT, id - 1, MPI_ANY_TAG, MPI_COMM_WORLD,
		MPI_STATUS_IGNORE);
		if (sd[0] == -1) {
			if (id < size - 1)
				MPI_Send(&sd, 2, MPI_INT, id + 1, 42, MPI_COMM_WORLD);
			break;
		}
		int *ps = sd;
		int l = check(&sort, &ps, p); // kiem tra thu tu trong nang
		if (l == 1)
			MPI_Send(&sd, 2, MPI_INT, id + 1, 42, MPI_COMM_WORLD);
	}
	int tam;
	MPI_Bcast(&tam, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int *mtam = (int*) malloc(2 * tam * sizeof(int));
	MPI_Bcast(mtam, tam * 2, MPI_INT, 0, MPI_COMM_WORLD);

	int h; // kich thuoc mang moi
	int *csort = thongke(&sort, p, &h); // x,y, sl
	int* sl = (int*) malloc(h * sizeof(int)); // chi so tam cum
	int *stam = (int*) malloc(tam * sizeof(int)); // so luong moi tam
	long *all = (long*) malloc(tam * 2 * sizeof(long));
	long *nhan = (long*) malloc(tam * 2 * sizeof(long)); // nhan tong so tam
	int *nhansl = (int*) malloc(tam * sizeof(int)); // nhan so luong tam
//	printf("cmn");
//
	while (1) {
		for (int i = 0; i < tam * 2; i++) {
			all[i] = 0; // reset du lieu
		}
		fill(&stam, tam, 0);
		center(&csort, &sl, h, &mtam, tam);
		update(&csort, &sl, h, &all, &stam);

		if (id * 2 + 1 < size) {
			//printf("id %d nhan tu %d\n", id, 2 * id + 1);
			MPI_Recv(nhan, tam * 2, MPI_LONG, id * 2 + 1, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(nhansl, tam, MPI_INT, id * 2 + 1, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int i = 0; i < tam; i++) {
				all[i * 2] = all[i * 2] + nhan[i * 2]; //x
				all[i * 2 + 1] = all[i * 2 + 1] + nhan[i * 2 + 1]; //y
				stam[i] = stam[i] + nhansl[i];
			}
		}
		if (id * 2 + 2 < size) {
			//printf("id %d nhan tu %d \n", id, 2 * id + 2);
			MPI_Recv(nhan, tam * 2, MPI_LONG, id * 2 + 2, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(nhansl, tam, MPI_INT, id * 2 + 2, MPI_ANY_TAG,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int i = 0; i < tam; i++) {
				all[i * 2] = all[i * 2] + nhan[i * 2]; //x
				all[i * 2 + 1] = all[i * 2 + 1] + nhan[i * 2 + 1]; //y
				stam[i] = stam[i] + nhansl[i];
			}
		}
		int z = (id - 1) / 2;
		//printf("id %d gui cho %d\n", id, z);
		MPI_Send(all, tam * 2, MPI_LONG, z, 42, MPI_COMM_WORLD);
		MPI_Send(stam, tam, MPI_INT, z, 42, MPI_COMM_WORLD);

		MPI_Bcast(mtam, tam * 2, MPI_INT, 0, MPI_COMM_WORLD);
		if (mtam[0] == -1)
			break;
	}

	// lay max gia tri
	int ma = 0, m = h;
	if (id * 2 + 1 < size) {
		//if(id==4) printf("id nhan %d", id * 2 + 1);
		MPI_Recv(&ma, 1, MPI_INT, id * 2 + 1, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (ma > m)
			m = ma;
	}
	if (id * 2 + 2 < size) {
		MPI_Recv(&ma, 1, MPI_INT, id * 2 + 2, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (ma > m)
			m = ma;
	}
	int z = (id - 1) / 2;
	MPI_Send(&m, 1, MPI_INT, z, 42, MPI_COMM_WORLD);

	for (int i = 0; i < h; i++) // gan gia tri bang tam
			{
		csort[3 * i + 2] = sl[i];
	}

	MPI_Send(&h, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
	MPI_Send(csort, h * 3, MPI_INT, 0, 42, MPI_COMM_WORLD);

	free(sort);
	free(csort);
	free(mtam);
	free(stam);
	free(sl);
	free(all);
	free(nhan);
	free(nhansl);
}
/**
 * tinh so tam moi
 */
int tammoi(int tam, int **mt, long **al, int**st) {
	int check = 0;
	int *mtam = *mt;
	long*all = *al;
	int *stam = *st;
	long px, py;
	for (int i = 0; i < tam; i++) {
		if (stam[i] != 0) {

			px = all[i * 2] / stam[i];
			py = all[i * 2 + 1] / stam[i];
			if (px != mtam[i * 2]) {
				check = 1;
				mtam[i * 2] = (int) px;
			}
			if (py != mtam[i * 2 + 1]) {
				check = 1;
				mtam[i * 2 + 1] = (int) py;
			}
		}
	}
//	for (int i = 0; i < tam; i++)
//	{
//
//		printf("mt[i * 2] :%d mt[i * 2+ 1] %d %d\n",mtam[i * 2],
//				mtam[i * 2 + 1],stam[i]);
//
//	}
	if (check == 0)
		mtam[0] = -1;
	return check;
}
/**
 *  tinh lai so tam:cong tong
 */
void update(int**cs, int**s, int h, long**al, int**st) {
	int*csort = *cs;
	int *sl = *s;
	long *all = *al;
	int *stam = *st;
	for (int i = 0; i < h; i++) {
		int m = sl[i];
		all[2 * m] = all[2 * m] + csort[3 * m] * csort[3 * m + 2];
		all[2 * m + 1] = all[2 * m + 1] + csort[3 * m + 1] * csort[3 * m + 2];
		stam[m] = stam[m] + csort[3 * m + 2];
	}
}
/**
 * tinh khoang cach gia cac tam
 */
void center(int **cs, int** s, int n, int **mt, int tam) {
	int *csort = *cs;
	int *sl = *s;
	int *mtam = *mt;
//	int *stam=*st;
	int a = 0, len;
	int fl = 0;
	for (int i = 0; i < n; i++) {
		len = Lmax
		;
		int k = 3 * i;
		for (int j = 0; j < tam; j++) {
			int h = 2 * j;
			double fd = ((csort[k] - mtam[h]) * (csort[k] - mtam[h])
					+ (csort[k + 1] - mtam[h + 1])
							* (csort[k + 1] - mtam[h + 1]));
			fl = (int) sqrt(fd);
			if (fl < len) {
				a = j;
				len = fl;
			}
		}
		sl[i] = a;
	}
}
/**
 * thong ke so luong phan tu
 */
//ok
int* thongke(int **sd, int p, int *z) {
	int *cs = (int*) malloc(p * 3 * sizeof(int)); // x,y,so luong
	int *mg = *sd;
	int d = 0, sl = 1; // d chi so phan tu, sl chi so mang
	cs[0] = mg[0];
	cs[1] = mg[1];
	for (int i = 1; i < p; i++) {
		int a = 2 * i, b = a - 2; // b=2*(i-1);
		if ((mg[a] != mg[b]) || (mg[a + 1] != mg[b + 1])) {
			cs[d * 3 + 2] = sl; // gan so luong
			sl = 1;
			d++;
			cs[3 * d] = mg[a];
			cs[3 * d + 1] = mg[a + 1];
		} else {
			sl++;
		}
	}
	cs[d * 3 + 2] = sl; // gan phan tu cuoi
	cs = (int*) realloc(cs, (d + 1) * 3 * sizeof(int));
	*z = d + 1;
	return cs;
}
/**
 * dien mang m voi gia tri -1
 */
void fill(int **m, int p, int k) {
	int *mg = *m;
//p = 2 * p;
	for (int i = 0; i < p; i++)
		mg[i] = k;
}
/**
 * so sanh x truoc , y sau
 */
//ok
int* kt(int max, int tam) {
	int *cs = (int*) malloc(tam * 2 * sizeof(int));
	srand(time(NULL));
	for (int i = 0; i < tam; i++) {
		int a = i * 2;
		cs[a] = rand() % max;
		cs[a + 1] = rand() % max;
	}
	return cs;
}
int check(int **so, int **ps, int p) {
	int *sort = *so, *sd = *ps;
	int temp;
	for (int i = 0; i < p; i++) {
		int l = 2 * i;
		if (sort[l] < sd[0]) { // kiem tra toa do z;
			temp = sd[0];
			sd[0] = sort[l];
			sort[l] = temp;
			temp = sd[1];
			sd[1] = sort[l + 1];
			sort[l + 1] = temp;
		} else if (sort[l] == sd[0]) {
			if (sort[l + 1] < sd[1]) {
				temp = sd[0];
				sd[0] = sort[l];
				sort[l] = temp;
				temp = sd[1];
				sd[1] = sort[l + 1];
				sort[l + 1] = temp;
			}
		}

	}
	if (sd[0] == -1)
		return 0;
	return 1;
}
//ok
int readfile(int **ng, int *lg, int *mx) {
	FILE *fl;
	fl = fopen("/home/amneiht/Desktop/anh/data", "r");
	if (fl == NULL)
		return -1;
	int mode, leng, max;
	fscanf(fl, "%d %d %d", &mode, &leng, &max);
	leng = leng * 2;
	*ng = (int*) calloc(leng, sizeof(int));
	int *mg = *ng; // chuyen vi tri con tro
	int i = 0, x, y;
	while (i < leng) {
		fscanf(fl, "%d %d", &x, &y);
		mg[i] = x;
		mg[i + 1] = y;
		//printf("%d %d\n", x, y);
		i = i + 2;
	}
	leng = leng / 2;
	fclose(fl);
	*lg = leng;
	*mx = max;
	return 0;
}
