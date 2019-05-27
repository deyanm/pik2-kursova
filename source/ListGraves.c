#include "Interface_ListGraves.h"

sNode_t* ListHead = NULL;
int scanStatus = 0;
const int monthDays[12] = {31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31};

void MainThread() {
    eStatus_t temp_status = eUninitialized;
    
    FILE *fp = NULL;
    
    char filename[NAME_SIZE];
    while (scanStatus == 0) {
        printf("Enter text file with data in it, or just the name:\n");
        fgets(filename, NAME_SIZE, stdin);
        if (filename[strlen(filename) - 5] == '.' && filename[strlen(filename) - 4] == 't') {
            scanStatus = 1;
        }
    }
    filename[strlen(filename) - 1] = '\0';
    temp_status = openFile(filename, "r", &fp);
    
    if (temp_status == eSuccess) {
        eStatus_t read_status = eUninitialized;
        read_status = readFromFile(fp, &ListHead);
        if (read_status == eSuccess) {
            showMenu(&fp, filename);
        }
    } else {
        temp_status = openFile(filename, "w", &fp);
        if (temp_status == eSuccess) {
            showMenu(&fp, filename);
        }
    }
    
    if (fclose(fp)) {
        perror("fclose error");
        exit(1);
    }
    deleteList(&ListHead);
}

void showMenu(FILE **fp, const char filename[]) {
    int option = '0';
    while (1) {
        printf("\nChoose option below:\n");
        printf("1 - Add new deceased\n");
        printf("2 - Print all graves records\n");
        printf("3 - Lookup mourning by unique number\n");
        printf("4 - Exit & Save\n");
        option = getchar();
        fseek(stdin, 0, SEEK_SET);
        switch (option) {
            case '1':
            {
                sGrave_t* grave = addNewGrave();
                if (grave != NULL) {
                    insert_before(&ListHead, *grave);
                } else {
                    printf("This unique number already exists!\n\n");
                }
                break;
            }
            case '2':
            {
                printData(ListHead);
                break;
            }
            case '3':
            {
                long id = 0;
                scanStatus = 0;
                while(scanStatus != 1) {
                    printf("\nEnter unique number: ");
                    scanStatus = scanf("%ld", &id);
                    fseek(stdin, 0, SEEK_SET);
                }
                sNode_t *found = findUnique(ListHead, id);
                if (found != NULL) {
                    time_t t = time(NULL);
                    struct tm *tm1 = localtime(&t);
                    tm1->tm_mon += 1;
                    tm1->tm_year += 1900;
                    struct tm tm2 = {0};
                    strptime((found->payload).startDate, "%d-%m-%Y", &tm2);
                    tm2.tm_mon += 1;
                    tm2.tm_year += 1900;
                    long days = getDifference(&tm2, tm1);
                    switch (days) {
                        case 40:
                            printf("\n40-days grief!");
                            break;
                        case 182:
                            printf("\n6-months grief!");
                            break;
                        case 365:
                            printf("\n1-year grief!");
                            break;
                        case 1095:
                            printf("\n3-years grief!");
                            break;
                        default:
                            printf("\nNo grief at the moment!");
                            break;
                    }
                }
                break;
            }
            case '4':
            {
                fclose(*fp);
                // because r+ will add the new contents to the end, without trunking the file
                eStatus_t temp_status = openFile(filename, "w", fp);
                if (temp_status == eSuccess) {
                    writeToFile(fp, ListHead);
                }
                return;
                break;
            }
                
            default:
                break;
        }
        
    }
}

eStatus_t openFile(const char file_path_param[], const char mode[], FILE **fp_param) {
    if (file_path_param != NULL) {
        *fp_param = fopen(file_path_param, mode);
        
        if (*fp_param != NULL)
        {
            return eSuccess;
        }
        else {
            return eFail;
        }
    } else {
        return eFail;
    }
}

eStatus_t readFromFile(FILE *fp_param, sNode_t **head_param) {
    //    sNode_t* temp_node = NULL;
    //    sNode_t* iterator = NULL;
    sGrave_t temp_data = {0};
    
    while (fscanf(fp_param, "%ld | ", &temp_data.uniqueNum) != EOF) {
        
        fscanf(fp_param, "%[^|]s", temp_data.name);
        fscanf(fp_param, "| %d;%d | ", &temp_data.location.row, &temp_data.location.column);
        fscanf(fp_param, "%[^|]s", temp_data.startDate);
        fscanf(fp_param, "| %d | ", &temp_data.age);
        fscanf(fp_param, "%[^\n]s\n", temp_data.endDate);
        
        //        temp_node = (sNode_t*) calloc(sizeof(sNode_t), 1);
        //        temp_node->payload.uniqueNum = temp_data.uniqueNum;
        //        strcpy(temp_node->payload.name, temp_data.name);
        //        temp_node->payload.age = temp_data.age;
        //        temp_node->payload.location = temp_data.location;
        //        strcpy(temp_node->payload.date, temp_data.date);
        //        temp_node->next = NULL;
        
        insert_before(head_param, temp_data);
        
        //        if (*head_param == NULL) {
        //            iterator = *head_param = temp_node;
        //        } else {
        //            iterator = iterator->next = temp_node;
        //        }
    }
    
    return eSuccess;
}

eStatus_t writeToFile(FILE **fp_param, sNode_t *head_param) {
    
    sNode_t* iterator = head_param;
    
    while (iterator != NULL) {
        fprintf(*fp_param, "%ld | %s | %d;%d | %s| %d | %s\n", (iterator->payload).uniqueNum, (iterator->payload).name, (iterator->payload).location.row, (iterator->payload).location.column, (iterator->payload).startDate, (iterator->payload).age, (iterator->payload).endDate);
        iterator = iterator->next;
    }
    
    return eSuccess;
}

sGrave_t* addNewGrave(void) {
    sGrave_t* grave = (sGrave_t*) malloc(sizeof(sGrave_t));
    long unique = 0;
    scanStatus = 0;
    while(scanStatus != 1) {
        printf("Unique number: ");
        scanStatus = scanf("%ld", &unique);
        fseek(stdin, 0, SEEK_SET);
    }
    if (findUnique(ListHead, unique) != NULL) {
        return NULL;
    }
    grave->uniqueNum = unique;
    printf("Name of the deceased: ");
    fgets(grave->name, NAME_SIZE, stdin);
    grave->name[strlen(grave->name) - 1] = '\0';
    fseek(stdin, 0, SEEK_SET);
    scanStatus = 0;
    while(scanStatus != 1) {
        printf("Age: ");
        scanStatus = scanf("%d", &grave->age);
        fseek(stdin, 0, SEEK_SET);
    }
    scanStatus = 0;
    while(scanStatus != 2) {
        printf("Location (row;column): ");
        scanStatus = scanf("%d;%d", &grave->location.row, &grave->location.column);
        fseek(stdin, 0, SEEK_SET);
    }
    if (isLocationFree(ListHead, &grave->location) != NULL) {
        printf("\nThis location is not free!");
        return NULL;
    }
    
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(grave->startDate, sizeof(grave->startDate), "%d-%m-%Y", tm);
    grave->startDate[strlen(grave->startDate)] = '\0';
    
    char buff[NAME_SIZE];
    while (strlen(grave->endDate) != 3) {
        printf("Enter the years of rental (ex. \'15\' or \'forever\'): ");
        fgets(buff, NAME_SIZE, stdin);
        if (strcmp(grave->endDate, "forever\n") == 0) {
            strcpy(grave->endDate, buff);
            break;
        } else {
            int years = atoi(buff);
            if (years != 0) {
                tm->tm_year += years;
                strftime(grave->endDate, sizeof(grave->endDate), "%d-%m-%Y", tm);
                break;
            }
        }
    }
    grave->endDate[strlen(grave->endDate)] = '\0';
    
    return grave;
}

void insert_before(sNode_t **head, sGrave_t payload) {
    if (head != NULL) {
        sNode_t *next = malloc(sizeof(sNode_t));
        next->payload = payload;
        next->next = *head;
        *head = next;
    }
}

sNode_t* findUnique(sNode_t *head, long unique) {
    sNode_t *current = head;
    
    if (head == NULL) {
        return NULL;
    }
    
    while((current->payload).uniqueNum != unique) {
        if (current->next == NULL) {
            return NULL;
        } else {
            current = current->next;
        }
    }
    
    return current;
}

sNode_t* isLocationFree(sNode_t *head, sLoc_t *loc) {
    sNode_t *current = head;
    
    if (head == NULL) {
        return NULL;
    }
    
    while(current != NULL) {
        if ((current->payload).location.row == loc->row
            && (current->payload).location.column == loc->column) {
            if (strcmp((current->payload).endDate, "forever") != 0) {
                time_t now = time(NULL);
                struct tm *tm = localtime(&now);
                struct tm end = {0};
                strptime((current->payload).endDate, "%d-%m-%Y", &end);
                if (tm->tm_year + 1900 < end.tm_year + 1900 && tm->tm_mon + 1 < end.tm_mon + 1 && tm->tm_mday < end.tm_mday) {
                    return current;
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        } else {
            current = current->next;
        }
    }
    
    return NULL;
}

eStatus_t deleteList(sNode_t** head_param) {
    sNode_t* iterator = *head_param;
    
    while (iterator != NULL) {
        *head_param = (*head_param)->next;
        free(iterator);
        iterator = *head_param;
    }
    
    return eSuccess;
}

void printData(sNode_t *head_param) {
    sNode_t* iterator = head_param;
    
    printf("\n   id:\tName:\t\t  Age:\tLocation:\t\tDate created:\t\tEnd date of rental:\n");
    printf("----------------------------------------------------------------------------------\n");
    while(iterator != NULL) {
        printf("%5ld\t%-10s%3d%6d;%d%20s%20s\n", (iterator->payload).uniqueNum, (iterator->payload).name,(iterator->payload).age, (iterator->payload).location.row, (iterator->payload).location.column, (iterator->payload).startDate, (iterator->payload).endDate);
        iterator = iterator->next;
    }
}

int countLeapYears(struct tm *tm) {
    int years = tm->tm_year;
    if (tm->tm_mon <= 2)
        years--;
    return years / 4 - years / 100 + years / 400;
}

long getDifference(struct tm *tm1, struct tm *tm2) {
    // the tm1 is the date of the death, the tm2 is the current date
    
    // initialize count using years and day
    long n1 = (tm1->tm_year) * 365 + (tm1->tm_mday);
    
    // Add days for months in given date
    for (int i=0; i<(tm1->tm_mon) - 1; i++)
        n1 += monthDays[i];
    
    n1 += countLeapYears(tm1);
    
    long n2 = (tm2->tm_year) * 365 + tm2->tm_mday;
    for (int i=0; i<(tm2->tm_mon) - 1; i++)
        n2 += monthDays[i];
    n2 += countLeapYears(tm2);
    
    // return difference between two counts
    return (n2 - n1);
}
