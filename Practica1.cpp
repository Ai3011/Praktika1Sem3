#include <string>
#include <iostream>
#include <filesystem>
#include <direct.h>
#include <fstream>
#include "Struct.h"
#include "parseJS.h"
#include "csv.h"

using namespace std;

// Функция для печати строк из двумерного списка
void printRows(List<List<string>>& to_print);

// Устанавливает значение первичного ключа в файл
void setPrimaryKeyFile(const filesystem::path& directory, const std::string& tableName, int key) {
    // Формируем путь к файлу первичного ключа
    filesystem::path filePath = directory / (tableName + "_pk");

    std::ofstream file(filePath, std::ios::out | std::ios::trunc); // Открываем файл для записи
    file << key; // Записываем ключ в файл
    file.close(); // Закрываем файл
}

// Читает значение первичного ключа из файла
int readPrimaryKeyValue(const filesystem::path& directory, const std::string& tableName) {
    // Формируем путь к файлу первичного ключа
    filesystem::path filePath = directory / (tableName + "_pk");
    std::ifstream file(filePath); // Открываем файл для чтения

    int value;
    file >> value; // Читаем значение ключа
    file.close(); // Закрываем файл
    return value; // Возвращаем прочитанное значение
}

// Проверяет, существует ли директория для схемы и создает CSV-файлы, если нет
void check_csv(string& schem_name, HashTable<List<string>>& tables, List<string>& tables_names) {
    filesystem::path dirPath = "."; // Устанавливаем текущую директорию
    dirPath = dirPath / schem_name; // Путь к директории схемы

    // Проверяем, существует ли директория
    if (filesystem::exists(dirPath) & filesystem::is_directory(dirPath)) {
        return; // Если существует, выходим из функции
    }
    else {
        for (int i = 0; i < tables_names.length; i++) {
            // Создаем директории для каждой таблицы
            filesystem::create_directories(dirPath / tables_names[i]);
            List<List<string>> headers; // Список для заголовков

            headers.push(tables.Get(tables_names[i])); // Получаем заголовки из хеш-таблицы
            unlock(dirPath / schem_name / tables_names[i], tables_names[i]); // Сбрасываем замок на таблице
            writeToCsv(dirPath / tables_names[i] / "1.csv", headers, tables_names[i], schem_name); // Записываем заголовки в CSV
            setPrimaryKeyFile(dirPath / tables_names[i], tables_names[i], 0); // Устанавливаем первичный ключ
            headers.clear(); // Очищаем список заголовков
        }
    }
}

// Получает имена таблиц и столбцов из строки
void get_names(string& line, int& i, List<string>& selected_tables, List<string>& selected_columns) {
    bool more = true; // Флаг для продолжения цикла
    int count = 0; // Счетчик

    while (more) {
        string name; // Имя таблицы
        string column; // Имя столбца
        bool geted_name = false; // Флаг для отслеживания, получили ли имя

        // Проверяем, не вышли ли за пределы строки
        if (i > line.length()) throw runtime_error("Wrong syntax");

        // Получаем имя таблицы и столбца
        while (line[i] != ' ' && line[i] != ',' && i < line.length()) {
            if (line[i] == '.') {
                geted_name = true; // Установить флаг, если точка найдена
                i++;
            }
            if (!geted_name) {
                name.push_back(line[i]); // Собираем имя таблицы
            }
            else {
                column.push_back(line[i]); // Собираем имя столбца
            }
            i++;
        }

        selected_tables.push(name); // Добавляем имя таблицы в список
        selected_columns.push(column); // Добавляем имя столбца в список

        name = ""; // Очищаем имя
        column = ""; // Очищаем имя столбца
        geted_name = false; // Сбрасываем флаг

        // Проверяем, продолжать ли цикл
        if (line[i] == ' ' || i >= line.length()) more = false;
        else i++;
        i++;
    }
}

// Получает имена таблиц из строки
void get_tables(string& line, int& i, List<string>& selected_tables) {
    bool more = true; // Флаг для продолжения цикла
    int count = 0; // Счетчик

    while (more) {
        string name; // Имя таблицы
        if (i > line.length()) throw runtime_error("Wrong syntax"); // Проверка на выход за пределы строки

        // Собираем имя таблицы
        while (line[i] != ' ' && line[i] != ',' && i < line.length()) {
            name.push_back(line[i]);
            i++;
        }

        // Проверяем, существует ли таблица
        if (selected_tables.find(name) != -1) throw runtime_error("Wrong syntax");

        selected_tables.push(name); // Добавляем имя таблицы в список
        name = ""; // Очищаем имя

        // Проверяем, продолжать ли цикл
        if (line[i] == ' ' || i >= line.length()) more = false;
        else i++;
        i++;
    }
}

// Копирует элементы из одного списка в другой
void copyList(List<string>& from, List<string>& to) {
    for (int i = 0; i < from.length; i++) {
        to.push(from[i]); // Копируем каждый элемент
    }
}

// Извлекает строку из строки с заданным индексом
string take_string(string& line, int& i) {
    string res; // Результирующая строка
    while (line[i] != ' ' && i < line.length()) {
        res.push_back(line[i]); // Собираем строку
        i++;
    }
    i++; // Увеличиваем индекс
    return res; // Возвращаем строку
}

// Очищает двумерный список
void double_clear(List<List<string>>& formed) {
    for (int o = 0; o < formed.length; o++) {
        formed[o].clear(); // Очищаем каждый подсписок
    }
    formed.clear(); // Очищаем основной список
}

// Выполняет перекрестное соединение двух списков
void cross_joint(List<List<string>>& itog, List<List<string>>& pred) {
    if (pred.length == 0) return; // Если второй список пуст, ничего не делаем

    List<List<string>> res; // Результирующий список

    for (int i = 0; i < itog.length; i++) {
        for (int j = 0; j < pred.length; j++) {
            List<string> prom1; // Временный список
            List<string> geted = itog[i]; // Получаем текущий список

            copyList(geted, prom1); // Копируем текущий список

            for (int o = 0; o < pred[j].length; o++) {
                prom1.push(pred[j][o]); // Добавляем элементы из второго списка
            }

            res.push(prom1); // Добавляем в результат
        }
    }

    double_clear(itog); // Очищаем итоговый список
    itog = res; // Присваиваем результат
}

// Проверяет синтаксис выбранных таблиц
bool check_synt(List<string>& selected_tables, List<string>& from_tables) {
    for (int i = 0; i < selected_tables.length; i++) {
        if (from_tables.find(selected_tables[i]) == -1) return false; // Если таблица не найдена, возвращаем false
    }
    for (int i = 0; i < from_tables.length; i++) {
        if (selected_tables.find(from_tables[i]) == -1) return false; // Если таблица не найдена, возвращаем false
    }

    return true; // Если все таблицы найдены, возвращаем true
}

// Печатает строки из двумерного списка
void printRows(List<List<string>>& to_print) {
    for (int i = 0; i < to_print.length; i++) {
        for (int j = 0; j < to_print[i].length; j++) {
            cout << to_print[i][j] << ' '; // Печатаем каждый элемент
        }
        cout << endl; // Печатаем перевод строки
    }
}

// Печатает отфильтрованные строки
void printFilteredRows(List<List<string>>& to_print, List<bool>& cmp) {
    for (int i = 0; i < to_print.length; i++) {
        if (!cmp[i]) continue; // Пропускаем, если флаг не установлен
        for (int j = 0; j < to_print[i].length; j++) {
            cout << to_print[i][j] << ' '; // Печатаем каждый элемент
        }
        cout << endl; // Печатаем перевод строки
