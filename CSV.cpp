#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include "Struct.h"

// Функция блокировки доступа к файлу
void lock(const filesystem::path& directory, const std::string tableName) {

    // Формируем путь к файлу блокировки
    filesystem::path filePath = directory / (tableName + "_lock");

    // Открываем файл блокировки для записи, перезаписывая его содержимое
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);

    // Записываем 0, что указывает на блокировку файла
    file << 0;

    // Закрываем файл
    file.close();
}

// Функция разблокировки доступа к файлу
void unlock(const filesystem::path& directory, const std::string tableName) {

    // Формируем путь к файлу блокировки
    filesystem::path filePath = directory / (tableName + "_lock");

    // Открываем файл блокировки для записи, перезаписывая его содержимое
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);

    // Записываем 1, что указывает на разблокировку файла
    file << 1;

    // Закрываем файл
    file.close();
}

// Функция проверки, разблокирован ли файл
int isunlocked(const filesystem::path& directory, const std::string& tableName) {

    // Формируем путь к файлу блокировки
    filesystem::path filePath = directory / (tableName + "_lock");

    // Открываем файл блокировки для чтения
    std::ifstream file(filePath);

    // Читаем текущее значение блокировки (по умолчанию 1 — файл разблокирован)
    int value = 1;
    file >> value;

    // Закрываем файл
    file.close();

    // Возвращаем значение блокировки (0 — заблокирован, 1 — разблокирован)
    return value;
}

// Функция записи данных в CSV-файл
void writeToCsv(const filesystem::path& filename, List<List<string>>& data, string table_name, string& schem_name) {

    // Указываем путь к текущей директории (можно заменить на нужную)
    filesystem::path filePath = ".";

    // Ожидаем разблокировки файла, если он заблокирован
    while (isunlocked(filePath / schem_name / table_name, table_name) == 0)
        std::cout << "Waiting for unlock" << std::endl;

    // Блокируем файл перед записью
    lock(filePath / schem_name / table_name, table_name);

    // Открываем CSV-файл для записи (перезаписываем его)
    std::ofstream file(filename, std::ios::out | std::ios::trunc);

    // Записываем данные построчно
    for (int i = 0; i < data.length; i++) {
        for (size_t j = 0; j < data[i].length; j++) {
            file << data[i][j];
            if (j < data[i].length - 1) {
                file << ",";
            }
        }
        file << "\n"; // Переход на новую строку
    }

    // Закрываем файл
    file.close();

    // Разблокируем файл после записи
    unlock(filePath / schem_name / table_name, table_name);
}

// Функция чтения данных из CSV-файла
void readFromCsv(const filesystem::path& filename, List<List<string>>& data, string table_name, string& schem_name) {

    // Указываем путь к текущей директории (можно заменить на нужную)
    filesystem::path filePath = ".";

    // Ожидаем разблокировки файла
    while (!isunlocked(filePath / schem_name / table_name, table_name))
        std::cout << "Waiting for unlock" << std::endl;

    // Блокируем файл перед чтением
    lock(filePath / schem_name / table_name, table_name);

    // Открываем CSV-файл для чтения
    std::ifstream file(filename);
    std::string line;

    // Читаем данные построчно
    while (std::getline(file, line)) {
        List<string> row;
        std::string cell;
        std::istringstream lineStream(line);

        // Разбиваем строку по разделителю ',' и добавляем в список
        while (std::getline(lineStream, cell, ',')) {
            row.push(cell);
        }

        // Добавляем строку в список данных
        data.push(row);
    }

    // Закрываем файл
    file.close();

    // Разблокируем файл после чтения
    unlock(filePath / schem_name / table_name, table_name);
}

// Функция добавления новой строки в CSV-файл
void appendToCsv(const filesystem::path& filename, List<string>& newRow, string table_name, string& schem_name) {

    // Указываем путь к текущей директории (можно заменить на нужную)
    filesystem::path filePath = ".";

    // Ожидаем разблокировки файла
    while (!isunlocked(filePath / schem_name / table_name, table_name))
        std::cout << "Waiting for unlock" << std::endl;

    // Блокируем файл перед добавлением новой строки
    lock(filePath / schem_name / table_name, table_name);

    // Открываем CSV-файл для добавления данных (добавляем в конец файла)
    std::ofstream file(filename, std::ios::app);

    // Записываем новую строку
    for (size_t i = 0; i < newRow.length; ++i) {
        file << newRow[i];
        if (i < newRow.length - 1) {
            file << ",";
        }
    }
    file << "\n"; // Переход на новую строку

    // Закрываем файл
    file.close();

    // Разблокируем файл после записи
    unlock(filePath / schem_name / table_name, table_name);
}
