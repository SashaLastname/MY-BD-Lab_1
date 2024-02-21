#ifndef BMS_DATABASE_GARBAGECOLLECTOR_H
#define BMS_DATABASE_GARBAGECOLLECTOR_H
# define file_i "garbage_collector_file_i.bin"
# define MAX_SIZE 3000

struct collectorElement {
    int key;
    int address;
    int deleted;
};

#endif 