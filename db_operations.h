#ifndef DB_OPERATIONS_H
#define DB_OPERATIONS_H

#include <string>
#include <filesystem>
#include "structures.h"  

using namespace std;

void setPrimaryKeyFile(const filesystem::path& directory, const std::string& tableName, int key);
int readPrimaryKeyValue(const filesystem::path& directory, const std::string& tableName);
void check_csv(string& schem_name, HashTable<List<string>>& tables, List<string>& tables_names);
void get_names(string& line, int& i, List<string>& selected_tables, List<string>& selected_columns);
void get_tables(string& line, int& i, List<string>& selected_tables);
void copyList(List<string>& from, List<string>& to);
string take_string(string& line, int& i);
void double_clear(List<List<string>>& formed);
void cross_joint(List<List<string>>& itog, List<List<string>>& pred);
bool check_synt(List<string>& selected_tables, List<string>& from_tables);
void printRows(List<List<string>>& to_print);
void printFilteredRows(List<List<string>>& to_print, List<bool>& cmp);
string unwrap_string(string str);
void form(string& schem_name, List<string>& from_tables, HashTable<List<string>>& rasp, List<string>& posl, List<List<string>>& old_formed, HashTable<List<string>>& tables);
void where_select(string& line, int& i, string& logi, List<string>& posl, List<List<string>>& old_formed, List<bool>& cmp);
void select(string& line, int& i, string& schem_name, HashTable<List<string>>& tables);
string take_list(string& cont, int& index);
void insert(string& line, int& i, HashTable<List<string>>& tables, string& schem_name, int& limit);
void deleting(string& line, int& i, HashTable<List<string>>& tables, string& schem_name);
void console_parse(string& schem_name, HashTable<List<string>>& tables, List<string>& tables_names, int& limit);

#endif // 
