#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ForkMergeSort.h"

#define OUTPIPEARG 3
#define ARGOFFSET 4
#define ALLOCSIZE 16

int main(int argc, char *argv[]) {

   int oneSize = 0, twoSize = 0, listSize = argc - 1, fds[2], child = 0;

   float *list, *halfOne, *halfTwo;

   if (argc > 1) {
      if (strcmp(argv[1], "-child") != 0) {
         pipe(fds);
      }
      else {
         fds[0] = atoi(argv[2]);
         fds[1] = atoi(argv[OUTPIPEARG]);
      }

      CreateList(&list, argv, argc, &listSize, &child);

      if (listSize > 1) {

         DivideList(list, listSize, &halfOne, &oneSize, &halfTwo, &twoSize);
         MainSort(list, listSize, halfOne, oneSize, halfTwo, twoSize, fds);

         Merge(&list, halfOne, oneSize, halfTwo, twoSize);
      }
      Report(list, listSize, child, fds);
   }
   return 0;
}

void Report(float *list, int listSize, int child, int fds[2]) {

   int ctr = 0;

   if (child == 1) {

      close(fds[0]);
      write(fds[1], list, listSize * sizeof(float));
      close(fds[1]);
   }

   else {

      for (ctr = 0; ctr < listSize; ctr++) {
         fprintf(stderr, "%f\n", list[ctr]);
      }


      close(fds[0]);
      close(fds[1]);

   }
}

void MainSort(float *list, int listSize, float *halfOne, int oneSize, 
 float *halfTwo, int twoSize, int fds[2]) {

   int kidPid = 0, stat = 0;

   char **args = NULL;

   if (oneSize > 0 && !(kidPid = fork())) {
      MakeArg(halfOne, oneSize, &args, fds);

      execv("./ForkMergeSort", args);
   }
   waitpid(kidPid, &stat, 0);
   read(fds[0], halfOne, oneSize * sizeof(float));

   if (twoSize > 0 && !(kidPid = fork())) {
      MakeArg(halfTwo, twoSize, &args, fds);;

      execv("./ForkMergeSort", args);
   }

   waitpid(kidPid, &stat, 0);
   read(fds[0], halfTwo, twoSize * sizeof(float));
}

void MakeArg(float *src, int srcSize, char ***args, int fds[2]) {

   int ctr = 0;

   *args = calloc(srcSize + ARGOFFSET, sizeof(char *));

   for (ctr = 0; ctr < srcSize + ARGOFFSET; ctr++) {
      (*args)[ctr] = calloc(ALLOCSIZE, sizeof(char));

   }

   sprintf((*args)[0], "%s", "./ForkMergeSort");
   sprintf((*args)[1], "%s", "-child");
   sprintf((*args)[2], "%d", fds[0]);
   sprintf((*args)[OUTPIPEARG], "%d", fds[1]);

   for (ctr = 0; ctr < srcSize; ctr++) {
      sprintf((*args)[ctr + ARGOFFSET], "%f", src[ctr]);
   }

   (*args)[srcSize + ARGOFFSET] = NULL;

}

void DivideList(float *list, int listSize, float **halfOne, int *oneSize, 
 float **halfTwo, int *twoSize) {

   int half = listSize / 2, ctr = 0;

   *halfOne = calloc(half + 1, sizeof(float));
   *halfTwo = calloc(listSize - half + 1, sizeof(float));

   *oneSize = 0;
   *twoSize = 0;

   for (ctr = 0; ctr < half; ctr++) {
      (*halfOne)[(*oneSize)++] = list[ctr];
   }

   for (ctr = half; ctr < listSize; ctr++) {
      (*halfTwo)[(*twoSize)++] = list[ctr];
   }

}

void CreateList(float **list, char *argv[], int argc, int *listSize,
 int *child) {

   int ctr = 1, listCtr = 0;

   *list = calloc(argc, sizeof(float));

   if (strcmp(argv[1], "-child") == 0) {

      ctr = ARGOFFSET;
      *listSize = argc - ARGOFFSET;
      *child = 1;

   }

   while (ctr < argc) {
      (*list)[listCtr++] = atof(argv[ctr]);
      ctr++;
   }

}

void Merge(float **list, float *halfOne, int sizeOne, float *halfTwo,
 int sizeTwo) {

   int ctrOne = 0, ctrTwo = 0, listCtr = 0;

   *list = calloc(sizeOne + sizeTwo, sizeof(float));

   while (ctrOne < sizeOne && ctrTwo < sizeTwo) {
      if (halfOne[ctrOne] < halfTwo[ctrTwo]) {
         (*list)[listCtr++] = halfOne[ctrOne++];
      }
      else {
         (*list)[listCtr++] = halfTwo[ctrTwo++];
      }
   }

   if (ctrOne == sizeOne) {
      while (ctrTwo < sizeTwo) {
         (*list)[listCtr++] = halfTwo[ctrTwo++];
      }
   }
   else {
      while (ctrOne < sizeOne) {
         (*list)[listCtr++] = halfOne[ctrOne++];
      }
   }

}
