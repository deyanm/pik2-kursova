#include "Interface_ListGraves.h"

sNode_t* ListHead = NULL;

void MainThread() {
    eStatus_t temp_status = eUninitialized;
    
    FILE *fp = NULL;
    
    char filename[NAME_SIZE];
    int status = 0;
    while (status == 0) {
        printf("Enter text file with data in it, or just the name:\n");
        fgets(filename, NAME_SIZE, stdin);
        if (filename[strlen(filename) - 5] == '.' && filename[strlen(filename) - 4] == 't') {
            status = 1;
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
        printf("3 - Exit\n");
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
        fscanf(fp_param, "| %d;%d ", &temp_data.location.row, &temp_data.location.column);
        fscanf(fp_param, "| %d | ", &temp_data.age);
        fscanf(fp_param, "%[^\n]s\n", temp_data.date);
        
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
        fprintf(*fp_param, "%ld | %s | %d;%d | %d | %s\n", (iterator->payload).uniqueNum, (iterator->payload).name, (iterator->payload).location.row, (iterator->payload).location.column, (iterator->payload).age, (iterator->payload).date);
        iterator = iterator->next;
    }
    
    return eSuccess;
}

sGrave_t* addNewGrave(void) {
    int status = 0;
    sGrave_t* grave = (sGrave_t*) malloc(sizeof(sGrave_t));
    long unique = 0;
    while(status != 1) {
        printf("Unique number: ");
        status = scanf("%ld", &unique);
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
    status = 0;
    while(status != 1) {
        printf("Age: ");
        status = scanf("%d", &grave->age);
        fseek(stdin, 0, SEEK_SET);
    }
    status = 0;
    while(status != 2) {
        printf("Location (row;column): ");
        status = scanf("%d;%d", &grave->location.row, &grave->location.column);
        fseek(stdin, 0, SEEK_SET);
    }
    if (isLocationFree(ListHead, &grave->location) != NULL) {
        printf("\nThis location is not free!");
        return NULL;
    }
    while (strlen(grave->date) != 11) {
        printf("End date of rental (\'forever\' or \'dd-mm-yyyy\'): ");
        fgets(grave->date, NAME_SIZE, stdin);
        if (strcmp(grave->date, "forever\n") == 0) {
            break;
        }
    }
    grave->date[strlen(grave->date) - 1] = '\0';
    //    time_t t = time(NULL);
    //    struct tm *tm = localtime(&t);
    //    strftime(grave->date, sizeof(grave->date), "%d-%m-%Y", tm);
    //    grave->date[strlen(grave->date)] = '\0';
    
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
            if (strcmp((current->payload).date, "forever") != 0) {
                time_t now = time(NULL);
                struct tm *tm = localtime(&now);
                struct tm end = {0};
                strptime((current->payload).date, "%d-%m-%Y", &end);
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
    
    printf("\n   id:\tName:\t\t\t  Age:\tLocation:\t\tEnd date of rental:\n");
    printf("------------------------------------------------------------------\n");
    while(iterator != NULL) {
        printf("%5ld\t%-20s%3d%6d;%d%20s\n", (iterator->payload).uniqueNum, (iterator->payload).name,(iterator->payload).age, (iterator->payload).location.row, (iterator->payload).location.column, (iterator->payload).date);
        iterator = iterator->next;
    }
}
