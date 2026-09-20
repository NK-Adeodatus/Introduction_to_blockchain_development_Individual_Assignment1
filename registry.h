#ifndef REGISTRY_H
#define REGISTRY_H

typedef struct {
    char book_id[20];
    char title[80];
    char author[50];
} Book;

typedef struct {
    char member_id[20];
    char full_name[50];
    char course_code[10];
} Member;

int load_books(const char *filename, Book **out_books, int *out_count);
int load_members(const char *filename, Member **out_members, int *out_count);

Book* find_book(Book *books, int num_books, const char *book_id);
Member* find_member(Member *members, int num_members, const char *member_id);

#endif // REGISTRY_H
