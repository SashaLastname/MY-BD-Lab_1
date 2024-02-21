#include <iostream>
#include "DataBase.h"
#define _CRT_SECURE_NO_WARNINGS

int main() {
    std::cout << "Database Lab1\n";
    DataBase data_base;
    DataBase::init_files();
    data_base.init_collector();

    int id[6] = { 1,2,3,4,5, 6 };
    char book_name[6][20] = { "HarryPotter","Pride","Witcher","CrimeAndPunishment", "GameOfThrones", "TheHobbit" };
    char country[6][20] = { "Ukraine", "USA", "Estonia", "Italy", "Finland", "France" };
    int id_publisher[6] = { -1,-1,-1,-1,-1, -1 };

    for (int i = 0; i < 5; i += 1) {
        Book book = { .id = id[i], .name = "" , .country = ""};
        strcpy(book.name, book_name[i]);
        strcpy(book.country, country[i]);
        data_base.insert_m(&book);
    }

    int publisher_id[7] = { 1,2,3,4,5, 6, 8 };
    int book_id[7] = { 5,5,5,2,2,4, 6 };
    char publisher_name[7][20] = { "Nord1","Nord2","Nord3","NBU6", "NBU11", "Swed4", "NY" };
    char publisher_address[7][20] = { "Helsinki 12913", "Helsinki 12142", "Espoo 64312", "Dolyna 77550", "Kyiv 45312", "Tallinn 77432", "Brooklyn 123" };

    for (int i = 0; i < 7; i += 1) {
        Publisher publisher = { .id = publisher_id[i],.book_id = book_id[i] ,.publisher_name = "", .publisher_address = "", .next_address = -1, .deleted = 0 };
        strcpy(publisher.publisher_name, publisher_name[i]);
        strcpy(publisher.publisher_address, publisher_address[i]);
        data_base.insert_s(&publisher);
    }

    data_base.ut_m();
    data_base.ut_s();

    std::cout << "------------------------\n";
    data_base.delete_m(2);
    data_base.ut_m();
    data_base.ut_s();

    std::cout << "------------------------\n";
    data_base.delete_s(5, 2);

    data_base.ut_m();
    data_base.ut_s();

    Book book = { .id = id[5], .name = "" , .country = ""};
    strcpy(book.name, book_name[5]);
    strcpy(book.country, country[5]);
    data_base.insert_m(&book);

    Publisher publisher = { .id = 8,.book_id = 6 ,.publisher_name = "", .publisher_address = "", .next_address = -1, .deleted = 0 };
    strcpy(publisher.publisher_name, publisher_name[6]);
    strcpy(publisher.publisher_address, publisher_address[6]);
    data_base.insert_s(&publisher);

    std::cout << "------------------------\n";
    data_base.ut_m();
    data_base.ut_s();

    char new_name[20] = { "OTF" };
    char new_country[20] = { "Germany" };

    data_base.update_m(6, new_name, new_country);

    char new_publisher_name[20] = "Munich2";
    char new_publisher_address[20] = "Munich 12437";
    data_base.update_s(6, 8, new_publisher_name, new_publisher_address);

    std::cout << "------------------------\n";
    data_base.ut_m();
    data_base.ut_s();

    data_base.get_m(4);
    data_base.get_s(5, 3);

    return 0;
}
