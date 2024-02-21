
#include <cstdio>
#include <algorithm>

#define _CRT_SECURE_NO_WARNINGS

#include "DataBase.h"
#include "Books.h"
#include "Publishers.h"
#include "GarbageCollector.h"

void DataBase::init_files() {
    FILE* books_file_m = fopen(file_m, "wb+");
    fclose(books_file_m);
    FILE* publishers_file_s = fopen(file_s, "wb+");
    fclose(publishers_file_s);
    FILE* indexes_file_i = fopen(file_i, "wb+");
    fclose(indexes_file_i);

}

void DataBase::init_collector() {
    for (int i = 0; i < MAX_SIZE; i += 1)
    {
        collector[i].key = -1;
        collector[i].address = -1;
        collector[i].deleted = 0;
    }
}

void  DataBase::load_collector(bool mode) { // load if mode == True else rewrite collector
    FILE* collector_file = fopen(file_i, mode ? "rb+" : "wb+");

    for (int i = 0; i < cnt_books; i += 1) {
        mode ? fread(&collector[i].key, sizeof(int), 1, collector_file) : fwrite(&collector[i].key, sizeof(int), 1, collector_file);
        mode ? fread(&collector[i].address, sizeof(int), 1, collector_file) : fwrite(&collector[i].address, sizeof(int), 1, collector_file);
        mode ? fread(&collector[i].deleted, sizeof(int), 1, collector_file) : fwrite(&collector[i].deleted, sizeof(int), 1, collector_file);
    }
    fclose(collector_file);
}

void DataBase::sort_collector() {
    std::qsort(collector,
        cnt_books, sizeof(collectorElement),
        [](const void* a, const void* b)
        { return (((collectorElement*)a)->key - ((collectorElement*)b)->key); }
    );
}

int DataBase::get_collector_address(int key) {

    for (int i = 0; i < cnt_books + del_books; i += 1) { // can be faster --> log(cnt_books)
        if (collector[i].key == key && collector[i].deleted == 0) {
            return collector[i].address;
        }
    }

    return -1;

}

int DataBase::get_collector_index(int key) {
    for (int i = 0; i < cnt_books + del_books; i += 1) { // can be faster --> log(cnt_books)
        if (collector[i].key == key && collector[i].deleted == 0) {
            return i;
        }
    }
    return -1;
}

void DataBase::insert_m(Book* book)
{
    for (int i = 0; i < cnt_books; i += 1) {
        if (collector[i].key == book->id) {
            printf("Reserved\n");
            return;
        }
    }

    FILE* books_file_m = fopen(file_m, "rb+");

    load_collector(true);
    collector[cnt_books + del_books].key = book->id;
    collector[cnt_books + del_books].address = (cnt_books + del_books) * (int)sizeof(Book);
    cnt_books++;

    fseek(books_file_m, 0, SEEK_END);

    fwrite(&book->id, sizeof(int), 1, books_file_m);
    fwrite(&book->name, sizeof(book->name), 1, books_file_m);
    fwrite(&book->country, sizeof(book->country), 1, books_file_m);
    fwrite(&book->id_publisher, sizeof(int), 1, books_file_m); 
    //!!!!
    sort_collector();
    load_collector(false);
    fclose(books_file_m);

}

void DataBase::insert_s(Publisher* publisher)
{
    int flag = 0;
    for (int i = 0; i < cnt_books + del_books; i++) {
        if (collector[i].deleted != 1 && collector[i].key == publisher->book_id) {
            flag = 1;
        }
    }
    if (flag == 0) {
        printf("ERROR\n");
        return;
    }

    FILE* publishers_file_s = fopen(file_s, "rb+");
    fseek(publishers_file_s, 0, SEEK_END);
    fwrite(&publisher->id, sizeof(int), 1, publishers_file_s);
    fwrite(&publisher->book_id, sizeof(int), 1, publishers_file_s);
    fwrite(&publisher->publisher_name, sizeof(publisher->publisher_name), 1, publishers_file_s);
    fwrite(&publisher->publisher_address, sizeof(publisher->publisher_address), 1, publishers_file_s);
    fwrite(&publisher->next_address, sizeof(int), 1, publishers_file_s);
    fwrite(&publisher->deleted, sizeof(int), 1, publishers_file_s);

    FILE* books_file_m = fopen(file_m, "rb+");
    int to_first_publisher_address = get_collector_address(publisher->book_id) + 44;
    fseek(books_file_m, to_first_publisher_address, SEEK_SET);

    int first_publisher_address;
    fread(&first_publisher_address, sizeof(int), 1, books_file_m);

    int address = (cnt_publisher + del_publisher) * (int)sizeof(Publisher);
    if (first_publisher_address == -1) {
        fseek(books_file_m, -4, SEEK_CUR);
        fwrite(&address, sizeof(int), 1, books_file_m);
    }
    else {
        int next_publisher = first_publisher_address;
        while (next_publisher != -1) {
            fseek(publishers_file_s, next_publisher + 48, SEEK_SET);
            fread(&next_publisher, sizeof(int), 1, publishers_file_s);
        }
        fseek(publishers_file_s, -4, SEEK_CUR);
        fwrite(&address, sizeof(int), 1, publishers_file_s);
    }
    cnt_publisher += 1;
    fclose(publishers_file_s);
    fclose(books_file_m);
}


void DataBase::ut_m()
{
    FILE* books_file_m = fopen(file_m, "rb+");
    printf("ut_m:\n");

    int id;
    char name[20];
    char country[20];
    int first_publisher;



    for (int i = 0; i < cnt_books + del_books; i += 1)
    {
        if (collector[i].deleted == 0)
        {

            fseek(books_file_m, get_collector_address(collector[i].key), SEEK_SET);
            fread(&id, sizeof(int), 1, books_file_m);
            fread(&name, sizeof(name), 1, books_file_m);
            fread(&country, sizeof(country), 1, books_file_m);
            fread(&first_publisher, sizeof(int), 1, books_file_m);
            printf("Id: %d, Name: %s, Country: %s, Publisher1 address: %d\n", id, name, country, first_publisher);
        }

    }

    fclose(books_file_m);


}

void DataBase::ut_s() const {
    FILE* publishers_file_s = fopen(file_s, "rb+");

    printf("ut_s:\n");

    int id;
    int book_id;
    char publisher_name[20];
    char publisher_address[20];
    int next_address;
    int deleted;
    // printf("cnt publisher: %d \n", cnt_publisher);
    for (int i = 0; i < cnt_publisher + del_publisher; i += 1) {
        fseek(publishers_file_s, 52, SEEK_CUR);
        fread(&deleted, sizeof(int), 1, publishers_file_s);

        if (deleted == 0) {
            fseek(publishers_file_s, -56, SEEK_CUR);
            fread(&id, sizeof(int), 1, publishers_file_s);
            fread(&book_id, sizeof(int), 1, publishers_file_s);
            fread(&publisher_name, sizeof(publisher_name), 1, publishers_file_s);
            fread(&publisher_address, sizeof(publisher_address), 1, publishers_file_s);
            fread(&next_address, sizeof(int), 1, publishers_file_s);
            fread(&deleted, sizeof(int), 1, publishers_file_s);
            printf("Id: %d, Book id %d: Publisher name: %s,  Publisher address(location) : %s, Next  address: %d, deleted:%d \n",
                id, book_id, publisher_name, publisher_address, next_address, deleted);
        }
    }

    fclose(publishers_file_s);
}

void DataBase::delete_m(int id)
{
    int tmp = get_collector_index(id);
    if (tmp == -1) {
        printf("Error, not presented id %d\n", id);
        return;
    }
    collector[tmp].deleted = 1;

    int offset = collector[tmp].address;
    FILE* books_file_m = fopen(file_m, "rb+");
    int first_publisher;
    fseek(books_file_m, offset + 44, SEEK_SET);
    fread(&first_publisher, sizeof(int), 1, books_file_m);
    fclose(books_file_m);
    if (first_publisher != -1)
    {
        FILE* publishers_file_s = fopen(file_s, "rb+");
        int new_del = 1;
        fseek(publishers_file_s, first_publisher, SEEK_SET);
        int next_publisher = first_publisher;
        int current_address;
        while (next_publisher != -1)
        {
            current_address = next_publisher;
            fseek(publishers_file_s, current_address + 52, SEEK_SET);
            fwrite(&new_del, sizeof(int), 1, publishers_file_s);
            del_publisher += 1;
            cnt_publisher -= 1;
            fseek(publishers_file_s, next_publisher + 48, SEEK_SET);
            fread(&next_publisher, sizeof(int), 1, publishers_file_s);
        }

        fclose(publishers_file_s);

    }
    cnt_books -= 1;
    del_books += 1;
    sort_collector();
    load_collector(false);
    fclose(books_file_m);
}

void DataBase::delete_s(int key_id_m, int key_id_s) {
    int skip_m = get_collector_address(key_id_m);
    if (skip_m == -1) {
        printf("Error, key id %d\n", key_id_m);
        return;
    }

    FILE* books_file_m = fopen(file_m, "rb+");
    fseek(books_file_m, skip_m, SEEK_SET);
    int first_publisher;
    fseek(books_file_m, skip_m + 44, SEEK_SET);
    fread(&first_publisher, sizeof(int), 1, books_file_m);


    if (first_publisher == -1) {
        printf("Book without publishers\n");
        return;
    }
    else {
        FILE* publishers_file_s = fopen(file_s, "rb+");


        int new_key_id_s;
        int next_publisher_new = -1;

        int new_del = 1;

        int new_next_publisher_address = -1;
        int next_publisher_address = first_publisher;
        int current_address = first_publisher;


        fseek(publishers_file_s, first_publisher, SEEK_SET);
        fread(&new_key_id_s, sizeof(int), 1, publishers_file_s);

        fseek(publishers_file_s, first_publisher + 48, SEEK_SET);
        fread(&next_publisher_address, sizeof(int), 1, publishers_file_s);

        if (new_key_id_s == key_id_s) {

            fseek(books_file_m, skip_m + 44, SEEK_SET);
            fwrite(&next_publisher_address, sizeof(int), 1, books_file_m);

            fseek(publishers_file_s, first_publisher + 48, SEEK_SET);
            fwrite(&next_publisher_new, sizeof(int), 1, publishers_file_s);
            fwrite(&new_del, sizeof(int), 1, publishers_file_s);

            cnt_publisher -= 1;
            del_publisher += 1;

            return;
        }
        else {
            while (next_publisher_address != -1) {
                fseek(publishers_file_s, next_publisher_address, SEEK_SET);
                fread(&new_key_id_s, sizeof(int), 1, publishers_file_s);
                if (new_key_id_s == key_id_s)
                {
                    fseek(publishers_file_s, next_publisher_address + 48, SEEK_SET);      //finding new nextAddress
                    fread(&new_next_publisher_address, sizeof(int), 1, publishers_file_s);

                    fseek(publishers_file_s, current_address + 48, SEEK_SET);         // writing new nextAddress
                    fwrite(&new_next_publisher_address, sizeof(int), 1, publishers_file_s);

                    fseek(publishers_file_s, next_publisher_address + 48, SEEK_SET);
                    fwrite(&next_publisher_new, 4, 1, publishers_file_s);
                    fwrite(&new_del, 4, 1, publishers_file_s);

                    fflush(publishers_file_s);

                    cnt_publisher -= 1;
                    del_publisher += 1;
                    return;
                }

                current_address = next_publisher_address;
                fseek(publishers_file_s, next_publisher_address + 48, SEEK_SET);
                fread(&next_publisher_address, sizeof(int), 1, publishers_file_s);

            }
        }
        fclose(publishers_file_s);
    }
    fclose(books_file_m);
}

void DataBase::update_m(int id, char name[20], char country[20]) {
    int skip = get_collector_address(id);
    if (skip == -1) {
        printf("Error update_m, key id %d\n", id);
        return;
    }

    FILE* books_file_m = fopen(file_m, "rb+");
    fseek(books_file_m, skip + 4, SEEK_SET);
    fwrite(name, 20, 1, books_file_m);
    fwrite(country, 20, 1, books_file_m);
    fclose(books_file_m);
}

void DataBase::update_s(int key_id_m, int key_id_s, char publisher_name[20], char publisher_address[20]) {
    int skip_m = get_collector_address(key_id_m);
    if (skip_m == -1) {
        printf("ERROR update_s, key id %d\n", key_id_m);
        return;
    }

    FILE* books_file_m = fopen(file_m, "rb+");
    fseek(books_file_m, skip_m, SEEK_SET);

    int first_publisher;
    fseek(books_file_m, skip_m + 44, SEEK_SET);
    fread(&first_publisher, sizeof(int), 1, books_file_m);

    fclose(books_file_m);

    if (first_publisher == -1) {
        printf("ERROR, book without publishers\n");
        return;
    }
    else {
        FILE* publishers_file_s = fopen(file_s, "rb+");
        fseek(publishers_file_s, first_publisher, SEEK_SET);
        int new_key_id_s;
        int next_publisher = first_publisher;
        int current_address = first_publisher;
        fread(&new_key_id_s, sizeof(int), 1, publishers_file_s);
        while (new_key_id_s != key_id_s) {
            if (next_publisher == -1) {
                printf("ERROR, update_s");
                return;
            }
            fseek(publishers_file_s, next_publisher, SEEK_SET);
            fread(&new_key_id_s, sizeof(int), 1, publishers_file_s);
            current_address = next_publisher;
            fseek(publishers_file_s, next_publisher + 48, SEEK_SET);
            fread(&next_publisher, sizeof(int), 1, publishers_file_s);
        }


        fseek(publishers_file_s, current_address + 8, SEEK_SET);
        fwrite(publisher_name, 20, 1, publishers_file_s);
        fwrite(publisher_address, 20, 1, publishers_file_s);
        fclose(publishers_file_s);
    }
}

void DataBase::get_m(int key_id) {
    int skip = get_collector_address(key_id);
    if (skip == -1) {
        printf("Error, key_id %d \n", key_id);
        return;
    }

    FILE* books_file_m = fopen(file_m, "rb+");
    fseek(books_file_m, skip + 4, SEEK_SET);
    char name[20], country[20];
    int  first_publisher;
    fread(&name, sizeof(name), 1, books_file_m);
    fread(&country, sizeof(country), 1, books_file_m);
    fread(&first_publisher, sizeof(int), 1, books_file_m);
    printf("Book id = %d, Book name= %s, Book country= %s\n",
        key_id, name, country);
    fclose(books_file_m);
}

void DataBase::get_s(int key_id_m, int key_id_s) {
    int skip_m = get_collector_address(key_id_m);
    if (skip_m == -1) {
        printf("Error, key_id %d \n", key_id_m);
        return;
    }

    FILE* books_file_m = fopen(file_m, "rb+");
    fseek(books_file_m, skip_m, SEEK_SET);
    int  first_publisher;
    fseek(books_file_m, skip_m + 44, SEEK_SET);
    fread(&first_publisher, sizeof(int), 1, books_file_m);
    fclose(books_file_m);


    if (first_publisher == -1) {
        printf("Book without publishers\n");
        return;
    }
    else {
        FILE* publisher_file_s = fopen(file_s, "rb+");
        fseek(publisher_file_s, first_publisher, SEEK_SET);
        char publisher_name[20], publisher_address[20];
        int new_key_id_s;
        int next_publisher = first_publisher;
        int current_address = first_publisher;
        fread(&new_key_id_s, sizeof(int), 1, publisher_file_s);
        while (new_key_id_s != key_id_s) {
            if (next_publisher == -1) {
                printf("Book %d doesn't have publisher %d\n", key_id_m, key_id_s);
                return;
            }
            fseek(publisher_file_s, next_publisher, SEEK_SET);
            fread(&new_key_id_s, sizeof(int), 1, publisher_file_s);
            current_address = next_publisher;
            fseek(publisher_file_s, next_publisher + 48, SEEK_SET);
            fread(&next_publisher, sizeof(int), 1, publisher_file_s);
        }
        int deleted = -1;
        fseek(publisher_file_s, current_address + 8, SEEK_SET);
        fread(publisher_name, sizeof(publisher_name), 1, publisher_file_s);
        fread(publisher_address, sizeof(publisher_address), 1, publisher_file_s);
        fseek(publisher_file_s, sizeof(int), SEEK_CUR);
        fread(&deleted, sizeof(int), 1, publisher_file_s);

        if (deleted == 0) {
            printf("Book id: %d Publisher id: %d Publisher name: %s Publisher address: %s\n", key_id_m, key_id_s, publisher_name,
                publisher_address);
        }
        else {
            printf("Book %d doesn't have publisher %d(prev_deleted)\n", key_id_m, key_id_s);
        }

        fclose(books_file_m);

    }
}
