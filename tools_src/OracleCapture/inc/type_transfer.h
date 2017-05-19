#pragma once

#include "dict_opt.h"

int tt_number(char* data, char* pval);

void tt_varchar2(char* data, char* val, int max_len);

void tt_char(char* data, char* val, int max_len);

//Ω‚Œˆvarray¿‡–Õ
//0001000000000001000000156ec1 0020 090000000000 0010 000000000001 8801 10 0101 0003 02c10202c10302c104
int tt_varray(char* data, DICT_COL* col, char* val, int max_len);

int get_column_type_value(int type, char* data, char* val, DICT_COL* col);
