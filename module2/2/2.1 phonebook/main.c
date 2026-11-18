#include <stdio.h>
#include <string.h>
#include "phonebook.h"

void read_string(const char *prompt, char *buffer, size_t max_len) {
    printf("%s", prompt);
    if (fgets(buffer, max_len, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

void input_contact(contact *c) {
    memset(c, 0, sizeof(contact)); 
    do {
        read_string("input firstname: ", c->firstname, sizeof(c->firstname));
        if (strlen(c->firstname) == 0) printf("its necessary\n");
    } while (strlen(c->firstname) == 0);

    do {
        read_string("input lastname: ", c->lastname, sizeof(c->lastname));
        if (strlen(c->lastname) == 0) printf("its necessary\n");
    } while (strlen(c->lastname) == 0);

    printf("enter to skip\n");
    read_string("input middlename: ", c->middlename, sizeof(c->middlename));
    read_string("input workspace: ", c->workspace, sizeof(c->workspace));
    read_string("input position: ", c->position, sizeof(c->position));
    
    printf("--- Numbers (up to 3) ---\n");
    for (int i = 0; i < 3; i++) {
        char prompt[50];
        snprintf(prompt, sizeof(prompt), "input number %d: ", i + 1);
        read_string(prompt, c->numbers[i], sizeof(c->numbers[i]));
    }
    
    read_string("input email: ", c->email, sizeof(c->email));
    read_string("input social link: ", c->social_link, sizeof(c->social_link));
    read_string("input messenger link: ", c->messenger_link, sizeof(c->messenger_link));
}

void edit_string_field(const char *prompt, char *field, size_t max_len) {
    char buffer[256];
    printf("%s [current: '%s'] (Enter to keep): ", prompt, field);
    
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0) {
            strncpy(field, buffer, max_len - 1);
            field[max_len - 1] = '\0'; 
        }
    }
}

void edit_contact(contact *c) {
    edit_string_field("firstname", c->firstname, sizeof(c->firstname));
    edit_string_field("lastname", c->lastname, sizeof(c->lastname));
    
    edit_string_field("middlename", c->middlename, sizeof(c->middlename));
    edit_string_field("workspace", c->workspace, sizeof(c->workspace));
    edit_string_field("position", c->position, sizeof(c->position));
    
    printf("--- Numbers ---\n");
    for (int i = 0; i < 3; i++) {
        char prompt[50];
        snprintf(prompt, sizeof(prompt), "number %d", i + 1);
        edit_string_field(prompt, c->numbers[i], sizeof(c->numbers[i]));
    }
    
    edit_string_field("email", c->email, sizeof(c->email));
    edit_string_field("social link", c->social_link, sizeof(c->social_link));
    edit_string_field("messenger link", c->messenger_link, sizeof(c->messenger_link));
}

void print_all(const phonebook *pb){
    if (pb->size == 0){
        printf("phonebook is empty\n");
        return;
    }
    printf("\n=== PHONEBOOK ===\n");
    for (size_t i = 0; i < pb->size; i++) {
        printf("[%zu] %s %s %s\n", i, pb->contacts[i].lastname, pb->contacts[i].firstname, pb->contacts[i].middlename);
        
        for (int j = 0; j < 3; j++) {
            if (strlen(pb->contacts[i].numbers[j]) > 0) {
                printf("   number %d: %s\n", j + 1, pb->contacts[i].numbers[j]);
            }
        }
        
        printf("   work: %s %s\n", pb->contacts[i].workspace, pb->contacts[i].position);
        printf("   email: %s\n", pb->contacts[i].email);
        printf("   social link: %s\n", pb->contacts[i].social_link);
        printf("   messenger link: %s\n", pb->contacts[i].messenger_link);
    }
}

void menu() {
    printf("\n1. print all\n");
    printf("2. add contact\n");
    printf("3. edit contact\n");
    printf("4. delete contact\n");
    printf("0. exit\n");
    printf("choice: ");
}

int main() {
    phonebook pb;
    phonebook_init(&pb);
    contact temp_contact;
    
    int choice;
    size_t index;
    int rc;

    while (1) {
        menu();
        if (scanf("%d", &choice) != 1) {
            printf("err input.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');
        
        switch(choice) {
            case 0:
                phonebook_free(&pb);
                return 0;
            case 1: 
                print_all(&pb);
                break;
            case 2: 
                printf("\n=== ADD CONTACT ===\n");
                input_contact(&temp_contact);
                rc = phonebook_add(&pb, &temp_contact);
                if (rc == 0) printf("successfully added\n");
                else printf("error: check required fields\n");
                break;
            case 3:
                printf("\n=== EDIT CONTACT ===\n");
                if (pb.size == 0) {
                    printf("phonebook is empty\n");
                    break;
                }
                print_all(&pb);
                printf("input contact index for edit: ");
                if (scanf("%zu", &index) != 1) {
                    printf("err input.\n");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');

                if (index >= pb.size) {
                    printf("index error.\n");
                    break;
                }

                temp_contact = pb.contacts[index]; 
                
                edit_contact(&temp_contact);
                
                rc = phonebook_edit(&pb, index, &temp_contact);
                if (rc == 0) printf("successfully edited\n");
                else printf("error during edit\n");
                break;
            case 4:
                printf("\n=== DELETE CONTACT ===\n");
                if (pb.size == 0) {
                    printf("phonebook is empty\n");
                    break;
                }
                print_all(&pb);
                printf("input contact index for delete: ");
                if (scanf("%zu", &index) != 1) {
                    printf("err input.\n");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n');

                if (index >= pb.size) {
                    printf("index error.\n");
                    break;
                }

                rc = phonebook_delete(&pb, index);
                if (rc == 0) printf("successfully deleted\n");
                else printf("err\n");
                break;
            default:
                printf("invalid choice\n");
                break;
        }
    }
    return 0;
}