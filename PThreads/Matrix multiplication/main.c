#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#define MAX 10
// rows and columns variables for matrices
int r1,c1,r2,c2 ,i,j,k;
int sum =0;
int step =0;
int Mat1[MAX][MAX],Mat2[MAX][MAX];
int multiply1[MAX][MAX],multiply2[MAX][MAX];
bool Created = false;
struct timespec start, finish;
// checking the elapsed time of each procedure alone
double elapsed1;
double elapsed2;


// this function is responsible for scanning the matrix from the input file
void scaninputfile()
{
    FILE *fp;
    fp = fopen("/home/ziyad/Desktop/input.txt", "r"); // read mode

   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
    fscanf(fp,"%d",&r1);
    fscanf(fp, "%d", &c1);

    for (int x=0;x<r1;x++)
    {
    for (int y =0;y<c1;y++)
    {
    fscanf(fp, "%d", &Mat1[x][y]);
    }
    }

    fscanf (fp,"%d",&r2);
    fscanf (fp,"%d",&c2);

    for (int x=0;x<r2;x++)
    {
    for (int y =0;y<c2;y++)
    {
    fscanf(fp, "%d", &Mat2[x][y]);
    }
    }

}

/// each element in a thread alone, procedure number 1

void *Matmultiply_runner_1(void *arg)
{
      for ( k = 0; k < r2; k++) {
          sum = sum + Mat1[i][k]*Mat2[k][j];
          }

}
void Matmultiply1()
{
     FILE *file;
    clock_gettime(CLOCK_MONOTONIC,&start);
// initializing of threads with number of rows* columns in matrix to represent all elements

    pthread_t threads [r1*c2];


     for (i = 0; i < r1; i++) {

      for (j = 0; j < c2; j++) {

        pthread_attr_t attr;
        pthread_attr_init (&attr);

        pthread_create(&threads[step],&attr,Matmultiply_runner_1,NULL);
        pthread_join(threads[step],NULL);

        step ++;

        multiply1[i][j] = sum;
        sum = 0;


        }

     }
///checking the elapsed time for procedure number 1

    clock_gettime(CLOCK_MONOTONIC,&finish);
    elapsed1  = (finish.tv_sec - start.tv_sec);
    elapsed1 += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	if (!Created) {
		file = fopen("output.txt", "w");
		Created = true;
	}
	else
		file = fopen("output.txt", "a");

	if (file == NULL) {
		if (Created)
			Created = false;
		return;
	}
	else
	{
      for (int c = 0; c < r1; c++) {
      for (int d = 0; d < c2; d++)
        fprintf(file,"%d ", multiply1[c][d]);

      fprintf(file,"\n");
    }
        fprintf(file,"END1   %f\n", elapsed1);
		fclose(file);
	}
    }


 //each row in a thread procedure number 2
void *Matmultiply_runner_2(void *arg)
{
      for (j = 0; j < c2; j++) {

          for ( k = 0; k < r2; k++) {
          sum = sum + Mat1[i][k]*Mat2[k][j];
        }

        multiply2[i][j] = sum;
        sum = 0;

      }

}

void Matmultiply2()
{
    clock_gettime(CLOCK_MONOTONIC,&start);
    FILE *file;
// initializing of threads with number of rows
    pthread_t threads [r1];


     for (i = 0; i < r1; i++) {

        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_create(&threads[i],&attr,Matmultiply_runner_2,NULL);

        pthread_join(threads[i],NULL);

    }
    clock_gettime(CLOCK_MONOTONIC,&finish);
    elapsed2 = (finish.tv_sec - start.tv_sec);
    elapsed2 += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    	if (!Created) {
		file = fopen("output.txt", "w");
		Created = true;
	}
	else
		file = fopen("output.txt", "a");

	if (file == NULL) {
		if (Created)
			Created = false;
		return;
	}
	else
	{
      for (int c = 0; c < r1; c++) {
      for (int d = 0; d < c2; d++)
        fprintf(file,"%d ", multiply2[c][d]);

      fprintf(file,"\n");
    }
        fprintf(file,"END2   %f", elapsed2);
		fclose(file);

}
}
/// driver function to check reliability of code
int main(int argc,char ** argv)
{

    scaninputfile();
    Matmultiply1();
    Matmultiply2();

    return 0;
}

