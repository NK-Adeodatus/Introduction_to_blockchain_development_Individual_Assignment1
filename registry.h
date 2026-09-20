typedef struct Book {
    char book_id[20];
    char title[80];
    char author[50];
} Book;

typedef struct Member {
    char member_id[20];
    char full_name[50];
    char course_code[10];
} Member;

int load_books(const char *filename, Book **books, int *num_books);
int load_members(const char *filename, Member **members, int *num_members);
Book* find_book(const char *book_id, Book *books, int num_books);
Member* find_member(const char *member_id, Member *members, int num_members);
