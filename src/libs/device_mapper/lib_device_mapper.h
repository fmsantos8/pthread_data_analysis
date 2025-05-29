#ifndef __LIB_DEVICE_MAPPER_H__
#define __LIB_DEVICE_MAPPER_H__

#include "classes/linked_list/cls_linked_list.h"

#include "libs/csv/lib_csv.h"

linked_list_t *lib_device_mapper_from_csv(csv_data_t *csv);

csv_data_t *lib_device_mapper_to_csv(linked_list_t *devices);

#endif