#ifndef INTERFACE_LISTGRAVES_H
#define INTERFACE_LISTGRAVES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define NAME_SIZE ((size_t) 30)

typedef struct Location {
    int row;
    int column;
} sLoc_t;

typedef struct Grave {
    long uniqueNum;
    char name[NAME_SIZE];
    int age;
    sLoc_t location;
    char startDate[NAME_SIZE];
    char endDate[NAME_SIZE];
} sGrave_t;

typedef struct Node {
    sGrave_t payload;
    struct Node* next;
} sNode_t;

typedef enum Status {
    eFail = -1,
    eUninitialized = 0,
    eSuccess = 1
} eStatus_t;

eStatus_t openFile(const char[], const char mode[], FILE **);
eStatus_t readFromFile(FILE *, sNode_t **);
eStatus_t writeToFile(FILE **, sNode_t *);
eStatus_t deleteList(sNode_t **);
void printData(sNode_t*);
void insert_before(sNode_t **head, sGrave_t payload);
sNode_t* findUnique(sNode_t *head, long unique);
void showMenu(FILE **fp, const char filename[]);
sNode_t* isLocationFree(sNode_t *head, sLoc_t *loc);
sGrave_t* addNewGrave(void);
int countLeapYears(struct tm *tm);
long getDifference(struct tm *tm1, struct tm *tm2);

#endif
