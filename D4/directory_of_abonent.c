#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 100
#define FOUND 1
#define NOT_FOUND 0
#define SIZE_BUFFER 20

struct abonent{
    char name[SIZE_BUFFER];
    char surname[SIZE_BUFFER];
    char number[SIZE_BUFFER];

};
//Замена символа переноса строки на конец строки
void replace_enter(char *str){
    int i = 0;
    while('\0' != str[i]){
        if(str[i] == '\n'){
            str[i] = '\0';
            break;
        }
        ++i;
    }
}
int add(struct abonent *data, int *found_abonents, int *count){
    if( (*count) >= N){
        printf("The list is full\n");
        return -1;
    }
    int i;
    for(i = 0; i < N; ++i){
        if(found_abonents[i] == NOT_FOUND){
            found_abonents[i] = FOUND;
            (*count)++;
            break;
        }
    }
    printf("Input name: ");
    fgets(data[i].name, SIZE_BUFFER , stdin);
    replace_enter(data[i].name);
    printf("Input surname: ");
    fgets(data[i].surname, SIZE_BUFFER , stdin);
    replace_enter(data[i].surname);
    printf("Input phone number: ");
    fgets(data[i].number, SIZE_BUFFER , stdin);
    replace_enter(data[i].number);

    printf("\tAdd abonent\n");
    return 0;
}
int delete(struct abonent *data, int *found_abonents, int *count){
    if( (*count) == 0){
        printf("The list is full\n");
        return -1;
    }
    char buffer[SIZE_BUFFER];  
    int i; 
    printf("Input number: ");
    fgets(buffer, SIZE_BUFFER , stdin);
    replace_enter(buffer);
    for(i = 0; i < N; ++i){
        if(FOUND == found_abonents[i] && 0 == strcmp(data[i].number, buffer) ){
            printf("Delete abonent %s\n", data[i].number);
            found_abonents[i] = NOT_FOUND;
            (*count)--;
            return i;
        }
    }
    printf("\tAbonent %s not found\n", buffer);
    return -1;
}
int print(struct abonent *data, int *found_abonents, int *count){
    if(*count == 0){
        printf("The list is empty\n");
        return -1;
    }
    printf("name, surname, number:\n");
    int i;
    for(i = 0; i < N; ++i){
        if(found_abonents[i] == FOUND){
            printf("%s %s %s\n", data[i].name, data[i].surname, data[i].number);
        }
    }
    printf("\n");
    return 0;
}
int search(struct abonent *data, int *found_abonents, int *count){
    if(*count == 0){
        printf("The list is empty\n");
        return -1;
    }
    char buffer[SIZE_BUFFER];  
    int i; 
    printf("Input number: ");
    fgets(buffer, SIZE_BUFFER , stdin);
    replace_enter(buffer);
    for(i = 0; i < N; ++i){
        if(FOUND == found_abonents[i] && 0 == strcmp(data[i].number, buffer) ){

            printf("name: %s,  surname: %s,  number: %s\n", data[i].name, data[i].surname, data[i].number);
            return i;
        }
    }
    printf("\tAbonent %s not found\n", buffer);
    return -1;
}
int main(){
    int i;
    int found_abonents[N];
    struct abonent data[N];
    int count = 0;

    for(i = 0; i < N; ++i){
        found_abonents[i] = NOT_FOUND;
    }
    int comand = -1;
    int run = 1;
    while(run){
        printf("Comands: exit(0), add(1), delete(2), print(3), search(4)\n");
        printf("Input comand: ");
        if(!scanf("%d%*c", &comand) == 1){
            comand = -1;
            while (getchar() != '\n');
        }
        switch(comand){
            case 0:
                run = 0;
                break;
            case 1:
                add(data, found_abonents, &count);
                break;
            case 2:
                delete(data, found_abonents, &count);
                break;
            case 3:
                print(data, found_abonents, &count);
                break;
            case 4:
                search(data, found_abonents, &count);
                break;
            default:
                printf("\tComand not found\n");
                break;
        }
    }

    printf("End\n");
    return 0;
}