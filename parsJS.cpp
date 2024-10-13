#include <stdio.h> // Подключаем библиотеку для работы с вводом-выводом
#include <stdlib.h> // Подключаем библиотеку для работы с памятью
#include <stdint.h> // Подключаем библиотеку для работы с целыми типами фиксированного размера
#include <string.h> // Подключаем библиотеку для работы со строками
#include <iostream> // Подключаем библиотеку для ввода-вывода в C++
#include <string> // Подключаем библиотеку для работы со строками в C++
#include "Struct.h" // Подключаем пользовательский заголовочный файл Struct.h

using namespace std; // Используем пространство имен std для удобства

// Функция для загрузки содержимого JSON-файла
char* LoadJsonFile(const char* fname, int64_t& size) {
    FILE* fp = fopen(fname, "rb"); // Открываем файл для чтения в бинарном режиме

    // Проверка успешности открытия файла
    if (!fp) {
        printf("Ошибка: не удалось открыть файл.\n"); // Выводим сообщение об ошибке
        exit(1); // Завершаем программу
    }

    int64_t file_length = 0; // Переменная для хранения длины файла

    fseek(fp, 0, SEEK_END); // Перемещаем указатель в конец файла
    file_length = ftell(fp); // Получаем длину файла
    fseek(fp, 0, SEEK_SET); // Перемещаем указатель обратно в начало файла

    // Выделяем память для содержимого файла
    char* content = new char[file_length + 1];
    fread(content, 1, file_length, fp); // Читаем содержимое файла в буфер
    content[file_length] = '\0'; // Добавляем нуль-терминатор для корректного окончания строки

    fclose(fp); // Закрываем файл
    size = file_length; // Устанавливаем размер содержимого

    return content; // Возвращаем загруженное содержимое
}

// Функция для извлечения строки из содержимого
string take_string(char* cont, int& index) {
    string content; // Переменная для хранения извлеченной строки
    bool get_content = 0; // Флаг для контроля извлечения содержимого
    bool geting_content = 0; // Флаг для контроля нахождения внутри строки

    while (true) {
        if (get_content) break; // Выходим из цикла, если строка извлечена
        else if (!get_content) {
            if (cont[index] == '"') { // Если встречаем кавычку
                if (geting_content) get_content = true; // Если уже внутри строки, устанавливаем флаг
                else geting_content = true; // Входим в строку
            }
            else if (geting_content) content.push_back(cont[index]); // Добавляем символ в извлекаемую строку
        }
        index++; // Переходим к следующему символу
    }
    return content; // Возвращаем извлеченную строку
}

// Функция для извлечения списка из содержимого
string take_list(char* cont, int& index) {
    string content; // Переменная для хранения извлеченного списка
    bool get_content = 0; // Флаг для контроля извлечения содержимого
    bool geting_content = 0; // Флаг для контроля нахождения внутри списка

    while (true) {
        if (get_content) break; // Выходим из цикла, если список извлечен
        else if (!get_content) {
            if (cont[index] == '[' || cont[index] == ']') { // Если встречаем квадратные скобки
                if (geting_content) get_content = true; // Если уже внутри списка, устанавливаем флаг
                else geting_content = true; // Входим в список
            }
            else if (geting_content) content.push_back(cont[index]); // Добавляем символ в извлекаемый список
        }
        index++; // Переходим к следующему символу
    }
    return content; // Возвращаем извлеченный список
}

// Функция для извлечения целого числа из содержимого
int take_int(char* cont, int& index) {
    int content = 0; // Переменная для хранения извлеченного числа

    bool get_content = 0; // Флаг для контроля извлечения содержимого
    bool geting_content = 0; // Флаг для контроля нахождения внутри числа
    bool geteted_one = 0; // Флаг для контроля, было ли извлечено хотя бы одно число

    while (true) {
        if (get_content) break; // Выходим из цикла, если число извлечено
        if (!get_content) {
            if ('0' > cont[index] || '9' < cont[index]) { // Если символ не цифра
                if (geting_content && geteted_one) get_content = true; // Если находимся внутри числа и уже извлекли хотя бы одну цифру
                else geting_content = true; // Входим в число
            }
            else if (geting_content) { // Если находимся внутри числа
                geteted_one = true; // Устанавливаем флаг, что извлекли цифру
                content = content * 10 + (cont[index] - '0'); // Обновляем число
            }
        }
        index++; // Переходим к следующему символу
    }
    return content; // Возвращаем извлеченное число
}

// Функция для конвертации строки в список строк
List<string> convert_string(string str) {
    List<string> res; // Результирующий список
    string name; // Переменная для хранения текущего значения

    bool geting = false; // Флаг для контроля извлечения строки

    int index = 0; // Индекс текущего символа
    while (index < str.length()) {
        if (str[index] == '"') { // Если встречаем кавычку
            if (!geting) geting = true; // Входим в строку
            else {
                geting = false; // Выходим из строки
                res.push(name); // Добавляем извлеченное значение в результат
                name = ""; // Очищаем переменную для следующей строки
            }
        }
        else if (geting) {
            name.push_back(str[index]); // Добавляем символ в текущее значение
        }
        index++; // Переходим к следующему символу
    }

    return res; // Возвращаем результирующий список
}

// Функция для извлечения пар из содержимого
void take_pairs(char* cont, int& index, int size, HashTable<List<string>>& tables, List<string>& tables_names, string& schem_name, int& tuples_limit) {
    string name; // Переменная для хранения текущего имени
    string content; // Переменная для хранения содержимого
    string table_name; // Переменная для хранения имени таблицы
    string columns; // Переменная для хранения столбцов

    bool get_name = 0; // Флаг для контроля извлечения имени
    bool geting_name = 0; // Флаг для контроля нахождения внутри имени

    while (index <= size) { // Проходим по содержимому
        if (!get_name) {
            if (cont[index] == '"') { // Если встречаем кавычку
                if (geting_name) get_name = true; // Если уже внутри имени, устанавливаем флаг
                else geting_name = true; // Входим в имя
            }
            else if (geting_name) name.push_back(cont[index]); // Добавляем символ в текущее имя
        }
        else {
            // Проверка на ключи и извлечение значений
            if (name == "name") schem_name = take_string(cont, index); // Извлекаем имя схемы
            else if (name == "tuples_limit") tuples_limit = take_int(cont, index); // Извлекаем лимит кортежей
            else if (name == "structure") { // Если ключ "structure"
                while (cont[index] != '}') { // Пока не встретим закрывающую фигурную скобку
                    table_name = take_string(cont, index); // Извлекаем имя таблицы
                    columns = take_list(cont, index); // Извлекаем список столбцов
                    List<string> columns_li = convert_string(columns); // Конвертируем список в формат List<string>
                    tables_names.push(table_name); // Добавляем имя таблицы в список имен
                    tables.Add(table_name, columns_li); // Добавляем таблицу в хэш-таблицу
                    while (cont[index] != '"' && cont[index] != '}') {
                        index++; // Пропускаем символы до закрывающей кавычки или фигурной скобки
                    }
                }
            }
            get_name = false; // Сбрасываем флаги
            geting_name = false;
            name = ""; // Очищаем имя для следующей пары
        }
        index++; // Переходим к следующему символу
    }
}

// Главная функция для обработки JSON
void getJSON(HashTable<List<string>>& tables, List<string>& tables_names, string& schem_name, int& limit) {
    int64_t size; // Переменная для хранения размера содержимого
    char* data = LoadJsonFile("shema1.json", size); // Загружаем JSON-файл

    int i = 0; // Индекс для чтения содержимого

    take_pairs(data, i, size, tables, tables_names, schem_name, limit); // Извлекаем пары из содержимого
}
