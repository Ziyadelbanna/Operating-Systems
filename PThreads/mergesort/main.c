#include <stdio.h>
#include <stdlib.h>

int arr[] ;
int arr_size ;

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 +j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}
int left,right,mid;

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
   void inputarray()
{
    FILE *fp;
    fp = fopen("/home/ziyad/Desktop/input1.txt", "r"); // read mode
   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
    fscanf(fp,"%d",&arr_size);

    for (int y =0;y<arr_size;y++)
        fscanf(fp, "%d", &arr[y]);

}
    pthread_t threads[20];
    int step =0;

    struct subarr{
    int left;
    int mid;
    int right;
    };

    void *merge_sort_left(void *args)
    {
        struct subarr *argus = args;
        step++;
        mergeSort(arr, argus->left, argus->mid);

    }
    void *merge_sort_right(void *args)
    {
        struct subarr *argus = args;

        step++;
        mergeSort(arr, argus->mid+1, argus->right);

    }

void mergeSort(int arr[], int l, int r)
{
    // Same as (l+r)/2, but avoids overflow for
    // large l and h

    int m = l+(r-l)/2;

    struct subarr subarray;

    if (l < r)
    {

        pthread_attr_t attr;
        pthread_attr_init (&attr);

        subarray.left = l;
        subarray.right = r;
        subarray.mid = m;

        pthread_create(&threads[step],&attr,merge_sort_left,&subarray);
        pthread_join(threads[step],NULL);

        pthread_create(&threads[step],&attr,merge_sort_right,&subarray);
        pthread_join(threads[step],NULL);

        merge(arr, l, m, r);

    }
}

void printArray(int A[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

int main()
{

    inputarray();
    mergeSort(arr, 0, arr_size - 1);
    printArray(arr, arr_size);

    return 0;
}
