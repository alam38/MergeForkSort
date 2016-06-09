#ifndef FORKMERGESORT_H
#define FORKMERGESORT_H

void Report(float *list, int listSize, int child, int fds[2]);
void MainSort(float *list, int listSize, float *halfOne, int oneSize, 
 float *halfTwo, int twoSize, int fds[2]);

void CreateList(float **list, char *argv[], int argc, int *listSize,
 int *child);

void Merge(float **list, float *halfOne, int sizeOne, float *halfTwo,
 int sizeTwo);

void MakeArg(float *src, int srcSize, char ***args, int fds[2]);
void DivideList(float *list, int listSize, float **halfOne, int *oneSize, 
 float **halfTwo, int *twoSize); 


#endif
