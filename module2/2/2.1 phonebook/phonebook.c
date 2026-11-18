#include "phonebook.h"
#include <stdlib.h>
#include <string.h>

void phonebook_init(phonebook *pb) {
    if (pb == NULL) return;
    pb->contacts = NULL;
    pb->size = 0;
    pb->capacity = 0;
}

void phonebook_free(phonebook *pb) {
    if (pb == NULL) return;
    free(pb->contacts);
    pb->contacts = NULL;
    pb->size = 0;
    pb->capacity = 0;
}

int phonebook_add(phonebook *pb, const contact *c) {
    if (pb == NULL || c == NULL) return -1;
    if (strlen(c->firstname) == 0 || strlen(c->lastname) == 0) return -1;
    if (pb->size >= pb->capacity) {
        size_t new_cap = (pb->capacity==0) ? 4 : pb->capacity*2; 
        contact *new_arr = realloc(pb->contacts, new_cap * sizeof(contact));
        if (new_arr == 0) return -2;
        pb->contacts = new_arr;
        pb->capacity = new_cap;
    }
    pb->contacts[pb->size] = *c;
    pb->size++; 

    return 0;
}

int phonebook_delete(phonebook *pb, size_t index) {
    if (pb == NULL) return -1;
    if (index < 0 || index >= pb->size) return -3;
    for (size_t i = index; i < pb->size - 1; i++) {
        pb->contacts[i] = pb->contacts[i + 1];
    }
    pb->size--;
    return 0;
}

int phonebook_edit(phonebook *pb, size_t index, const contact *c) {
    if (pb == NULL) return -1;
    if (index < 0 || index >= pb->size) return -3;
    if (strlen(c->firstname) == 0 || strlen(c->lastname) == 0) return -1;
    pb->contacts[index] = *c;
    return 0;
}

