#include<stdio.h>
#include "registry.h"

int load_books(const char *filename, Book **books, int *num_books) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Error opening file
    }

    fscanf(file, "%d", num_books);
    *books = (Book *)malloc(sizeof(Book) * (*num_books));
    for (int i = 0; i < *num_books; i++) {
        fscanf(file, "%s %s %s", (*books)[i].book_id, (*books)[i].title, (*books)[i].author);
    }

    fclose(file);
    return 0; // Success
}

int load_members(const char *filename, Member **members, int *num_members) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Error opening file
    }

    fscanf(file, "%d", num_members);
    *members = (Member *)malloc(sizeof(Member) * (*num_members));
    for (int i = 0; i < *num_members; i++) {
        fscanf(file, "%s %s %s", (*members)[i].member_id, (*members)[i].full_name, (*members)[i].course_code);
    }

    fclose(file);
    return 0; // Success
}

Book* find_book(const char *book_id, Book *books, int num_books) {
    for (int i = 0; i < num_books; i++) {
        if (strcmp(books[i].book_id, book_id) == 0) {
            return &books[i];
        }
    }
    return NULL; // Book not found
}

Member* find_member(const char *member_id, Member *members, int num_members) {
    for (int i = 0; i < num_members; i++) {
        if (strcmp(members[i].member_id, member_id) == 0) {
            return &members[i];
        }
    }
    return NULL; // Member not found
}