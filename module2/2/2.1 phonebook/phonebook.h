#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stddef.h>

typedef struct contact {
    char firstname[32];
    char lastname[32];
    char middlename[32];
    char workspace[32];
    char position[32];
    char numbers[3][13]; // массив
    char email[32];
    char social_link[32];
    char messenger_link[32];
} contact;

typedef struct phonebook {
    contact *contacts;
    size_t size; 
    size_t capacity; 
} phonebook;

void phonebook_init(phonebook *pb);
void phonebook_free(phonebook *pb);
int phonebook_add(phonebook *pb, const contact *c);
int phonebook_edit(phonebook *pb, size_t index, const contact *c);
int phonebook_delete(phonebook *pb, size_t index);

#endif