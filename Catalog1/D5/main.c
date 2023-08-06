#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_BUFFER 20

struct Abonent{
    char name[SIZE_BUFFER];
    char surname[SIZE_BUFFER];
    char number[SIZE_BUFFER];

};

struct List{
    struct Abonent *abonent;
    struct List *next;
};

// Supporting
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

//  List
struct List *create_List_element(struct Abonent *abonent){
    struct List *new_element = (struct List*)malloc(sizeof(struct List));
    new_element->abonent = abonent;
    new_element->next = NULL;
    return new_element;
}
struct List *delete_List_element(struct List *element){
    if(element->abonent){
        free(element->abonent);
    }
    free(element);
    return NULL;
}
struct List *delete_List_lists(struct List *head){
    struct List *ptr = head, *ptr_del;
    while(ptr){
        ptr_del = ptr;
        ptr = ptr->next;
        delete_List_element(ptr_del);
    }
    return NULL;
}
//  Abonent
struct Abonent *create_Abonent(){
    struct Abonent *abonent = (struct Abonent*)malloc(sizeof(struct Abonent));
    return abonent;
}

struct Abonent *create_Abonent_and_full(){
    struct Abonent *abonent = create_Abonent();

    printf("Input name: ");
    fgets(abonent->name, SIZE_BUFFER , stdin);
    replace_enter(abonent->name);
    printf("Input surname: ");
    fgets(abonent->surname, SIZE_BUFFER , stdin);
    replace_enter(abonent->surname);
    printf("Input phone number: ");
    fgets(abonent->number, SIZE_BUFFER , stdin);
    replace_enter(abonent->number);

    return abonent;
}
void print_Abonent(struct Abonent *abonent){
    printf("\t%s %s %s\n", abonent->name, abonent->surname, abonent->number);
}
//  Comands

struct List *add_list(struct List *end_list){
    
    struct List *new_element = create_List_element(create_Abonent_and_full());

    end_list->next = new_element;
    printf("\tAdd abonent\n");
    return new_element;
}
void print_list(struct List *head){
    struct List *ptr = head;
    printf("name, surname, number:\n");
    while(ptr){
        print_Abonent(ptr->abonent);
        ptr = ptr->next;
    }
}
int delete_abonent_by_number(struct List *head, struct List **end_list){
    struct List *ptr = head, *ptr_del;
    char buffer[SIZE_BUFFER];  

    printf("Input number: ");
    fgets(buffer, SIZE_BUFFER , stdin);
    replace_enter(buffer);

    while(ptr->next){
        if( 0 == strcmp(ptr->next->abonent->number, buffer) ){
            
            ptr_del = ptr->next;
            ptr->next = ptr_del->next;
            if( 0 == strcmp( (*end_list)->abonent->number, buffer) ){
                // ситуация при которой указатель на след элемент не NULL и наоборот
                if(ptr_del->next)
                    (*end_list) = ptr_del->next;// след элемент за удаляемым
                else
                    (*end_list) = ptr;// удаляемый элем последний, присваевам до
            }
            delete_List_element(ptr_del);
            printf("\tDelete abonent\n");
            return 0;
        }
        ptr = ptr->next;
    }
    printf("\tAbonent %s not found\n", buffer);
    return -1;
}
int search(struct List *head){
    struct List *ptr = head;
    char buffer[SIZE_BUFFER];  

    printf("Input number: ");
    fgets(buffer, SIZE_BUFFER , stdin);
    replace_enter(buffer);
    while(ptr->next){
        if( 0 == strcmp(ptr->next->abonent->number, buffer) ){
            printf("Abonent(name, surname, number):\n");
            print_Abonent(ptr->next->abonent);
            return 0;
        }
        ptr = ptr->next;
    }
    printf("\tAbonent %s not found\n", buffer);
    return -1;
}

int main(){
    int count = 0;
    // служит как начальный элемент хранящий указатель на след элемент, не содержит данные
    struct List *head = create_List_element(NULL); 
    // хранит указатель на последний элемент
    struct List *end_list = head;    
    int run = 1;
    int comand = -1;

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
                end_list = add_list(end_list);
                ++count;
                break;
            case 2:
                if(count > 0){
                    delete_abonent_by_number(head, &end_list);
                    --count;
                }
                else{
                    printf("\tThe list is empty\n");
                }
                break;
            case 3:
                if(count > 0)
                    print_list(head->next);
                else{
                    printf("\tThe list is empty\n");
                }
                break;
            case 4:
                if(count > 0)
                    search(head);
                else{
                    printf("\tThe list is empty\n");
                }
                
                break;
            default:
                printf("\tComand not found\n");
                break;
        }
    }
    delete_List_lists(head);
    printf("END\n");
    return 0;
}


