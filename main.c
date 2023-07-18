#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

static int N = 3000000;
const int RUN = 32;

int min(int a,int b){
	if(a < b){
		return a;
	}else{
		return b;
	}
}

int isSorted(int array[],int size){
	for(int i=0;i<size-1;i++){
		if(array[i] > array[i+1]){
			return 1;
		}
	}
	return 0;
}

void merge(int array[],int begin,int middle,int end){
	int left = middle - begin + 1;
	int right = end - middle;
	
	int *array1 = (int*)malloc(left*sizeof(int));
	int *array2 = (int*)malloc(right*sizeof(int));
	
	// Filling the "left" array
	for(int i=0;i<left;i++){
		array1[i] = array[begin+i];
	}
	// Filling the "right" array
	for(int j=0;j<right;j++){
		array2[j] = array[middle +j + 1];
	}
	int i=0;
	int j=0;
	int k = begin;
	// Traversing through both arrays and comparing the values of the two arrays
	while(i<left && j<right){
		if(array1[i] <= array2[j]){
			array[k] = array1[i];
			i++;
		}else{
			array[k] = array2[j];
			j++;
		}
		k++;
	}
	// Copying the remaining elements from the "left" array
	while(i<left){
		array[k] = array1[i];
		i++;
		k++;
	}
	// Copying the remaining elements from the "right" array
	while(j<right){
		array[k] = array2[j];
		j++;
		k++;
	}
	free(array1);
	free(array2);
}

void mergeSort(int array[],int begin,int end){
	if(begin >= end){
		return;
	}
	int middle = begin + (end-begin) / 2;
	mergeSort(array,begin,middle);
	mergeSort(array,middle+1,end);
	merge(array,begin,middle,end);
}

void parallelMergeSort(int array[],int begin,int end){
	if(begin >= end){
		return;
	}
	
	int middle = begin + (end-begin) / 2;
	
	#pragma omp parallel sections
	{
		#pragma omp section
		mergeSort(array,begin,middle);
		
		#pragma omp section
		mergeSort(array,middle+1,end);
	}
	
	merge(array,begin,middle,end);
}

void insertionSort(int array[],int a,int b){
	for(int i=a+1;i<=b;i++){
		int temp = array[i];
		int j = i-1;
		while(j>=a && array[j] > temp){
			array[j+1] = array[j];
			j--;
		}
		array[j+1] = temp;
	}
}

void timSort(int array[],int n){
	for(int i=0;i<n;i+=RUN){
		insertionSort(array,i,min((i+RUN-1),(n-1)));
	}
	
	for(int size=RUN;size<n;size=2*size){
		for(int left=0;left<n;left+=2*size){
			int middle = left+size-1;
			int right = min((left+2*size-1),(n-1));
			if(middle < right){
				merge(array,left,middle,right);
			}
		}
	}
}

void parallelTimSort(int array[],int n){
	#pragma omp parallel for
	for(int i=0;i<n;i+=RUN){
		insertionSort(array,i,min((i+RUN-1),(n-1)));
	}
	for(int size=RUN;size<n;size*=2){
		#pragma omp parallel for
		for(int left=0;left<n;left+=2*size){
			int middle = left+size-1;
			int right = min((left+2*size-1),(n-1));
			if(middle<right){
				merge(array,left,middle,right);
			}
		}
	}
}

void printArray(int array[],int size){
	for(int i=0;i<size;i++){
		printf("%d ",array[i]);
	}
	printf("\n");
}

int main(){
	int *array = (int*)malloc(N*sizeof(int));
	int *array1 = (int*)malloc(N*sizeof(int));
	int *array2 = (int*)malloc(N*sizeof(int));
	int *array3 = (int*)malloc(N*sizeof(int));

	srand(time(0));
	for(int i=0;i<N;i++){
		int random = rand();
		array[i]  = random;
		array1[i] = random;
		array2[i] = random;
		array3[i] = random;
	}

	clock_t begin = clock();
	mergeSort(array,0,N-1);
	clock_t end = clock();

	double diff = (end-begin)/(double)CLOCKS_PER_SEC;
	printf("The execution time of merge sort is:%lf\n",diff);
	
	if(isSorted(array,N)==1){
		printf("The array is not sorted.\n");
	}
	
	double clockStart = omp_get_wtime();
	parallelMergeSort(array1,0,N-1);
	double clockEnd = omp_get_wtime();
	printf("The execution time of parallel merge sort is:%lf\n",clockEnd-clockStart);
	
	if(isSorted(array1,N)==1){
		printf("The array is not sorted.\n");
	}
	
	clock_t begin1 = clock();
	timSort(array2,N);
	clock_t end1 = clock();
	double diff1 = (end1-begin1)/(double)CLOCKS_PER_SEC;
	printf("The execution time of tim sort is:%lf\n",diff1);
	
	if(isSorted(array2,N)==1){
		printf("The array is not sorted.\n");
	}
	
	double clockStart1 = omp_get_wtime();
	parallelTimSort(array3,N);
	double clockEnd1 = omp_get_wtime();
	printf("The execution time of parallel tim sort is.:%lf\n",clockEnd1-clockStart1);
	
	if(isSorted(array3,N)==1){
		printf("The array is not sorted.\n");
	}
	
	free(array);
	free(array1);
	free(array2);
	free(array3);
}


