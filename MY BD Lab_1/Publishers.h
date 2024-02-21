
#ifndef BMS_DATABASE_PUBLISHERS_H
#define BMS_DATABASE_PUBLISHERS_H

# define file_s "publishers_file_s.bin"

struct Publisher {
    int id;
    int book_id;
    char publisher_name[20];
    char publisher_address[20];
    int next_address;
    int deleted;
};
#endif 
