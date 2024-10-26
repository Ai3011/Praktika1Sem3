#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "Commands.h"

using namespace std;


// Основная функция программы
int main(){
    HashTable<List<string>> tables; // Хэш-таблица для хранения данных таблиц
    List<string> tables_names; // Список имен таблиц
    string schem_name; // Имя схемы (каталог для хранения данных)
    int limit; // Лимит записей

    // Загрузка данных из JSON файла
    getJSON(tables, tables_names, schem_name, limit);

    // Проверка или создание необходимых CSV файлов
    check_csv(schem_name, tables, tables_names);

    // Парсер команд
    console_parse(schem_name, tables, tables_names, limit);
}
