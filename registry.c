#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registry.h"

void trim_newline(char *str) {
    int len = strlen(str);
    if (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[len-1] = '\0';
    }
}

int load_books(const char *filename, Book **out_books, int *out_count) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int count = 0;
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1) count++;
    }

    if (count == 0) {
        fclose(file);
        return 0;
    }

    Book *books = malloc(count * sizeof(Book));
    rewind(file);

    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        if (strlen(line) <= 1) continue;

        char *id = strtok(line, ",");
        char *title = strtok(NULL, ",");
        char *author = strtok(NULL, ",");

        if (id && title && author) {
            strncpy(books[i].book_id, id, sizeof(books[i].book_id) - 1);
            strncpy(books[i].title, title, sizeof(books[i].title) - 1);
            strncpy(books[i].author, author, sizeof(books[i].author) - 1);
            books[i].book_id[sizeof(books[i].book_id) - 1] = '\0';
            books[i].title[sizeof(books[i].title) - 1] = '\0';
            books[i].author[sizeof(books[i].author) - 1] = '\0';
            i++;
        }
    }

    fclose(file);
    *out_books = books;
    *out_count = i;
    return 1;
}

int load_members(const char *filename, Member **out_members, int *out_count) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int count = 0;
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1) count++;
    }

    if (count == 0) {
        fclose(file);
        return 0;
    }

    Member *members = malloc(count * sizeof(Member));
    rewind(file);

    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        trim_newline(line);
        if (strlen(line) <= 1) continue;

        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        char *course = strtok(NULL, ",");

        // Simple fix for bullet point dashes if they exist in file
        if (id[0] == '-' && id[1] == ' ') id += 2;

        if (id && name && course) {
            strncpy(members[i].member_id, id, sizeof(members[i].member_id) - 1);
            strncpy(members[i].full_name, name, sizeof(members[i].full_name) - 1);
            strncpy(members[i].course_code, course, sizeof(members[i].course_code) - 1);
            members[i].member_id[sizeof(members[i].member_id) - 1] = '\0';
            members[i].full_name[sizeof(members[i].full_name) - 1] = '\0';
            members[i].course_code[sizeof(members[i].course_code) - 1] = '\0';
            i++;
        }
    }

    fclose(file);
    *out_members = members;
    *out_count = i;
    return 1;
}

Book* find_book(Book *books, int num_books, const char *book_id) {
    for (int i = 0; i < num_books; i++) {
        if (strcmp(books[i].book_id, book_id) == 0) {
            return &books[i];
        }
    }
    return NULL;
}

Member* find_member(Member *members, int num_members, const char *member_id) {
    for (int i = 0; i < num_members; i++) {
        if (strcmp(members[i].member_id, member_id) == 0) {
            return &members[i];
        }
    }
    return NULL;
}