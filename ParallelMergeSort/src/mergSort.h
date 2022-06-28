#pragma once



/****************************************************************
 * function: mergeSort
 *
 * parameters:
 * *  pointer to memory where the set of integers is stored, the
 *    code does neither free nor modifies the memory.
 * *  length number of elements stored in the set
 *
 * retuns:
 * * heap allocated array with the sorted elements on it
 ***************************************************************/
int *mergeSort(int *blob, int length);


//TODO: rmove
void printArray(int *blob, int length);
