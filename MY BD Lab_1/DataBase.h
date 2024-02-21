
#ifndef BMS_DATABASE_DATABASE_H
#define BMS_DATABASE_DATABASE_H
#include "GarbageCollector.h"
#include "Books.h"
#include "Publishers.h"
class DataBase {
public:
    static void init_files();
    void init_collector();
    void load_collector(bool mode);
    void sort_collector();
    int get_collector_address(int key);
    int get_collector_index(int key);

    void insert_m(Book* book);
    void insert_s(Publisher* publisher);

    void delete_m(int id);
    void delete_s(int key_id_m, int key_id_s);

    void get_m(int key_id);
    void get_s(int key_id_m, int key_id_s);

    void update_m(int id, char name[20], char country[20]);
    void update_s(int key_id_m, int key_id_s, char publisher_name[20], char publisher_address[20]);

    void ut_m();
    void ut_s() const;

private:
    collectorElement collector[MAX_SIZE]{};

    int cnt_books = 0;
    int del_books = 0;

    int cnt_publisher = 0;
    int del_publisher = 0;

};
#endif //BMS_DATABASE_DATABASE_H