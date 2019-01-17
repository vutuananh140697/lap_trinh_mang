#define	MAXLENNAME 200
#define MAXLENPASSWORD 200
#include "linklist.h"
#include <string.h>
void load_data_base();

void export_to_data_file();

int search_userid(Node**user,char *name);
