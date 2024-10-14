#include <iostream> 
#include <string>
#include "structures.h"
#include <fstream>
#include <cstdint>

using namespace std;

char* LoadJsonFile(const char* fname, int64_t& size) {
    // Открываем файл в бинарном режиме, сразу перемещаемся в конец
    std::ifstream file(fname, std::ios::binary | std::ios::ate);

    // Проверяем, что файл открыт
    if (!file) {
        std::cerr << "Ошибка: не удалось открыть файл." << std::endl;
        exit(1);
    }

    // Получаем размер файла
    size = file.tellg();
    file.seekg(0, std::ios::beg); // Возвращаем указатель в начало файла

    // Выделяем память под содержимое файла + 1 для завершающего нулевого символа
    char* content = new char[size + 1];

    // Читаем содержимое файла
    if (!file.read(content, size)) {
        std::cerr << "Ошибка: не удалось прочитать файл." << std::endl;
        delete[] content; // Освобождаем память в случае ошибки
        exit(1);
    }

    // Добавляем завершающий нулевой символ для корректной работы с C-строкой
    content[size] = '\0';

    // Закрываем файл
    file.close();

    return content;
}



string take_string(char* cont, int& index) {
    string content;
    bool get_content = 0;
    bool geting_content = 0;

    while (true) {
        if (get_content) {
            break;
        }else if (!get_content) {
            if (cont[index] == '"') {
                if (geting_content){
                    get_content = true;
                }else geting_content = true;
            }else if (geting_content) {
                content.push_back(cont[index]);
            }    
        }
        index++;
    }
    return content;
}

string take_list(char* cont, int& index) {
    string content;
    bool get_content = 0;
    bool geting_content = 0;

    while (true) {
        if (get_content){ 
            break;
        }else if (!get_content) {
            if (cont[index] == '[' || cont[index] == ']') {
                if (geting_content){ 
                    get_content = true;
                }else geting_content = true;
            }else if (geting_content)   { 
                content.push_back(cont[index]);
            }
        }

        index++;
    }
    return content;
}


int take_int(char* cont, int& index) {
    int content = 0;

    bool get_content = 0;
    bool geting_content = 0;
    bool geteted_one = 0;

    while (true) {
        if (get_content){ 
            break;
        }
        if (!get_content) {
            if ('0' > cont[index] || '9' < cont[index]) {
                if (geting_content && geteted_one){ 
                    get_content = true;
                }else geting_content = true;
            } else if (geting_content) {
                geteted_one = true;
                content = content * 10 + (cont[index] - '0');
            }

        }

        index++;
    }
    return content;
}

List<string> convert_string(string str) {
    List<string> res;
    string name;

    bool geting = false;

    int index = 0;
    while (index < str.length()) {
        if (str[index] == '"') {
            if (!geting){ 
                geting = true;
            } else {
                geting = false;
                res.push(name);
                name = "";
            }
        } else if (geting) {
            name.push_back(str[index]);
        }
        index++;
    }

    return res;
}

void take_pairs(char* cont, int& index, int size, HashTable<List<string>>& tables, List<string>& tables_names, string& schem_name, int& tuples_limit) {
    string name;
    string content;
    string table_name;
    string columns;

    bool get_name = 0;
    bool geting_name = 0;

    while (index <= size) {
        if (!get_name) {
            if (cont[index] == '"') {
                if (geting_name){ 
                    get_name = true;
                } else geting_name = true;
            } else if (geting_name) {
                name.push_back(cont[index]);
            }
        } else {
            if (name == "name") { 
                schem_name = take_string(cont, index);
            } else if (name == "tuples_limit"){
                 tuples_limit = take_int(cont, index);
            } else if (name == "structure") {
                while (cont[index] != '}') {
                    table_name = take_string(cont, index);
                    columns = take_list(cont, index);
                    List<string> columns_li = convert_string(columns);
                    tables_names.push(table_name);
                    tables.Add(table_name, columns_li);
                    while (cont[index] != '"' && cont[index] != '}') {
                        index++;
                    }
                }
            }
            get_name = false;
            geting_name = false;
            name = "";
        }
        index++;
    }
}


void getJSON(HashTable<List<string>>& tables, List<string>& tables_names, string& schem_name, int& limit) {
    int64_t size;
    char* data = LoadJsonFile("shema.json", size);

    int i = 0;

    take_pairs(data, i, size, tables, tables_names, schem_name, limit);
}
