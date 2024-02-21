#ifndef BMS_DATABASE_BOOKS_H
#define BMS_DATABASE_BOOKS_H
# define file_m "books_file_m.bin"

struct Book {
    int id;
    int id_publisher;
    char name[20];
    char country[20];
};

#endif 

