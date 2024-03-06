#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <windows.h>

extern int errno;

#define MAX_ITEMS 128                               //max number of todo items
#define CLEAR_MENU 0
#define ITEM_NAME_MAX 32
#define ITEM_DESCRIPTION_MAX 128     

const int itemNameMax = 32;
const int itemDescriptionMax = 32;

typedef struct                                      //Todo Item struct
{
    char itemName[ITEM_DESCRIPTION_MAX];
    char itemDescription[ITEM_DESCRIPTION_MAX];
    time_t timeCreated;
    time_t timeDeadline;
}TodoItem;

int currentAmountOfItems = 0;                       //current amount of todo items
TodoItem todoItemsArray[MAX_ITEMS];                 //Todo Items Array

char saveFileName[32] = "itemTodoArraySaveFile.bin";

// start functions declaration
bool deleteAllTodoItems();
bool viewItemsMenu(TodoItem inputTodoItemsArray[]);
bool deleteTodoItemByNameMenu();
bool addItemMenu();
bool viewItemsMenu(TodoItem inputTodoItemsArray[]);
void printAllItems(TodoItem inputTodoItemsArray[]);
bool saveTodoItemsArray(char filename[], TodoItem *inputTodoItemsArrayPtr, int amountOfItems);
bool loadTodoItemsArray(char filename[], int *amountOfItems);
int checkIfExistsByName(TodoItem inputTodoItemsArray[], char userInputItemName[]);
bool createTodoItem(TodoItem inputTodoItemsArray[], char userInputItemName[], char userInputItemDescription[], int userInputDaysTillDeadline);
bool deleteTodoItemByName(TodoItem inputTodoItemsArray[], char userInputItemName[]);
// end fucntions declaration

int checkIfExistsByName(TodoItem inputTodoItemsArray[], char inputItemName[]){
    int i;

    for(i=0; i < currentAmountOfItems; ++i){
        if(strcmp(inputTodoItemsArray[i].itemName, inputItemName) == 0){
            return i;
        }
    }
    return -1;


}

bool createTodoItem(TodoItem inputTodoItemsArray[], char userInputItemName[], char userInputItemDescription[], int userInputDaysTillDeadline){
    if(checkIfExistsByName(inputTodoItemsArray, userInputItemName) == -1){
        time_t timeCreated = time(NULL);
        time_t timeDeadline = timeCreated + 24*3600*userInputDaysTillDeadline; // time created + hours in a day*seconds in a hour*days till dead line

        TodoItem item;

        strcpy(item.itemName, userInputItemName);
        strcpy(item.itemDescription, userInputItemDescription);

        item.timeCreated = timeCreated;
        item.timeDeadline = timeDeadline;

        inputTodoItemsArray[currentAmountOfItems] = item;
        ++currentAmountOfItems;

        return true;
    } else {
        return false;
    }
}

bool deleteTodoItemByName(TodoItem inputTodoItemsArray[], char userInputItemName[]){
    int j;

    int i = checkIfExistsByName(inputTodoItemsArray, userInputItemName);

    if(i != -1){
        for(j=i; j < currentAmountOfItems - 1; ++j) {
            inputTodoItemsArray[j] = inputTodoItemsArray[j+1];
        }

        if(currentAmountOfItems > 0) {
            --currentAmountOfItems;
        }
    } else {
        fprintf(stderr, "deleteTodoItemByName: item (%s) for deletion does not exist\n", userInputItemName);
        return false;
    }
    return true;
}

bool saveTodoItemsArray(char filename[], TodoItem *inputTodoItemsArrayPtr, int amountOfItems){
    FILE *filePtr;
    filePtr = fopen(filename, "wb");

    if (filePtr == NULL) return false;
    
    if (fwrite(&amountOfItems, sizeof(int), 1, filePtr) != 1) return false;

    if (fwrite(inputTodoItemsArrayPtr, sizeof(TodoItem), amountOfItems, filePtr) != amountOfItems) return false;

    if (fclose(filePtr) == EOF) return false;

    return true;
}


bool loadTodoItemsArray(char filename[], int *amountOfItems){
    FILE *filePtr;

    filePtr = fopen(filename, "rb");

    if (filePtr == NULL) {
        printf("\nError filePtr\n");
        return false;
    }
    if (fread(amountOfItems, sizeof(int), 1, filePtr) != 1) {
        printf("Amount Of Items To Read: %d, Actual Read Amount: %d\n", *amountOfItems, fread(amountOfItems, sizeof(int), 1, filePtr));
        printf("\nError read amount\n");   
        return false;
    }

    TodoItem *data = malloc(sizeof(TodoItem) * *amountOfItems);

    if (fread(data, sizeof(TodoItem), *amountOfItems, filePtr) != *amountOfItems) {
        printf("\nError amount\n");
        free(data);
        return false;
    }

    if (fclose(filePtr) == EOF) {
        printf("\nError EOF\n");
        free(data);
        return false;
    }
    
    for (int i = 0; i < *amountOfItems; ++i) {
        printf("loading %d\n", i);
        strcpy(todoItemsArray[i].itemName, data[i].itemName);
        strcpy(todoItemsArray[i].itemDescription, data[i].itemDescription);

        todoItemsArray[i].timeCreated = data[i].timeCreated;
        todoItemsArray[i].timeDeadline = data[i].timeDeadline;
    }

    return true;
}

void printAllItems(TodoItem inputTodoItemsArray[]){
    int i;
    
    for (i = 0; i < currentAmountOfItems; ++i){
        printf("\n\nItem Name: %s", inputTodoItemsArray[i].itemName);
        printf("\nItem Description: %s", inputTodoItemsArray[i].itemDescription);
        printf("\nTime Created: %s", ctime(&inputTodoItemsArray[i].timeCreated));
        printf("Time Deadline: %s", ctime(&inputTodoItemsArray[i].timeDeadline));
        printf("Hours Left: %.1f", difftime(inputTodoItemsArray[i].timeDeadline, time(NULL)) / 3600.0);
    }
}
    

bool addItemMenu(){
    char itemName[ITEM_NAME_MAX];
    char itemDescription[ITEM_DESCRIPTION_MAX];
    char timeLeft[sizeof(int)];


    printf("\nItem Name: ");
    while (fgets(itemName, sizeof(itemName), stdin) == NULL) {
        printf("Bad ItemName Input!!!!!\n");
        printf("\nItem Name: ");
    }

    if(checkIfExistsByName(todoItemsArray, itemName) != -1) {           // Checks If Item Exists
        printf("Item: %s Already Exists :(\n", itemName);
        return false;
    }
    fflush(stdin);

    printf("\nItem Description: ");
    while (fgets(itemDescription, sizeof(itemDescription), stdin) == NULL) {
        printf("Bad ItemDescription Input!!!!!\n");
        printf("\nItem Description: ");
    }
    fflush(stdin);

    printf("\nTime To Complete: ");
    while (fgets(timeLeft, sizeof(timeLeft), stdin) == NULL) {
        printf("Bad TimeLeft Input!!!!!\n");
        printf("\nTime To Complete: ");
    }
    fflush(stdin);

    itemName[strcspn(itemName, "\r\n")] = 0;
    itemDescription[strcspn(itemDescription, "\r\n")] = 0;

    createTodoItem(todoItemsArray, itemName, itemDescription, atoi(timeLeft));
    return true;
}

bool deleteTodoItemByNameMenu() {
    char itemName[ITEM_NAME_MAX];
    char cofirmation;
    char *inputSuccessCheck = NULL;

    do {
        printf("\nItem Name To Delete: ");
        inputSuccessCheck = fgets(itemName, ITEM_NAME_MAX, stdin);
        inputSuccessCheck[strcspn(inputSuccessCheck, "\r\n")] = 0;

        fflush(stdin);


    } while ((inputSuccessCheck == NULL) && checkIfExistsByName(todoItemsArray, itemName));

    inputSuccessCheck = NULL;

    if (deleteTodoItemByName(todoItemsArray, itemName)) {
        printf("\nThe Deletion Of Item [%s] Ended Successfuly", itemName);
        return true;
    } else {
        return false;
    }


    return true;
}

bool deleteAllTodoItems(){
    int i;
    for (i = 0; i < currentAmountOfItems; ++i) {
        if(!deleteTodoItemByName(todoItemsArray, todoItemsArray[i].itemName)){
            return false;
        }
    }
    
    return true;
}

int mainMenu(){
    char buffer;
    int optionPicked;
    do{
        if(CLEAR_MENU) system("CLS");
        printf("\n\t\tMain Menu (Press 1-6 And Then Enter To Pick An Option)\n\n");
        printf("1. View All Items\n2. Add An Item\n3. Delete An Item By Name & Save\n4. Delete All Items & Save\n5. Save\n6. Quit\n");
        fgets(&buffer, 2, stdin);
        fflush(stdin);
        optionPicked = (int)buffer - 48;

        switch (optionPicked){
            case 1:         // View/Edit All Items
                printAllItems(todoItemsArray);
                break;
            case 2:         // Add An Item
                addItemMenu();
                break;
            case 3:         // Delete An Item & Save
                deleteTodoItemByNameMenu();
                saveTodoItemsArray("itemTodoArraySaveFile.bin", todoItemsArray, currentAmountOfItems);
                break;
            case 4:         // Delete All Items & Save
                deleteAllTodoItems();
                saveTodoItemsArray("itemTodoArraySaveFile.bin", todoItemsArray, currentAmountOfItems);
                break;
            case 5:         // Save
                saveTodoItemsArray("itemTodoArraySaveFile.bin", todoItemsArray, currentAmountOfItems);
                break;
            case 6:         // Quit Without Saving
                break;
            default:
                printf("Wrong Option Lil Boy!\n");
                break;
        }
    } while(optionPicked != 6);

    return 0;
}

int main(){

    printf("loading completed: %d\n", loadTodoItemsArray(saveFileName, &currentAmountOfItems));

    mainMenu();
    return 0;
}