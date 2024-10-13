#include <string>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <fstream>
#include "Struct.h"
#include "parseJS.h"
#include "csv.h"

using namespace std;

void printRows(List<List<string>>& to_print);

void setPrimaryKeyFile(const filesystem::path& directory, const std::string& tableName, int key) {

    filesystem::path filePath = directory / (tableName + "_pk");

    std::ofstream file(filePath, std::ios::out | std::ios::trunc);

    file << key;

    file.close();
}

int readPrimaryKeyValue(const filesystem::path& directory, const std::string& tableName) {

    filesystem::path filePath = directory / (tableName + "_pk");

    std::ifstream file(filePath);

    int value;
    file >> value;

    file.close();

    return value;
}

void check_csv(string& schem_name, HashTable<List<string>>& tables, List<string>& tables_names) {
    filesystem::path dirPath = ".";
    dirPath = dirPath / schem_name;

    if (filesystem::exists(dirPath) & filesystem::is_directory(dirPath)) {
        return;
    }
    else {
        for (int i = 0; i < tables_names.length; i++) {
            filesystem::create_directories(dirPath / tables_names[i]);
            List<List<string>> headers;

            headers.push(tables.Get(tables_names[i]));
            unlock(dirPath / schem_name / tables_names[i], tables_names[i]);
            writeToCsv(dirPath / tables_names[i] / "1.csv", headers, tables_names[i], schem_name);
            setPrimaryKeyFile(dirPath / tables_names[i], tables_names[i], 0);
            headers.clear();
        }
    }
}

void get_names(string& line, int& i, List<string>& selected_tables, List<string>& selected_columns) {
    bool more = true;
    int count = 0;

    while (more) {
        string name;
        string column;
        bool geted_name = false;
        if (i > line.length()) throw runtime_error("Wrong syntax");

        while (line[i] != ' ' && line[i] != ',' && i < line.length()) {
            if (line[i] == '.') {
                geted_name = true;
                i++;
            }
            if (!geted_name) {
                name.push_back(line[i]);
            }
            else {
                column.push_back(line[i]);
            }
            i++;
        }

        selected_tables.push(name);
        selected_columns.push(column);

        name = "";
        column = "";
        geted_name = false;
        if (line[i] == ' ' || i >= line.length()) more = false;
        else i++;
        i++;
    }
}

void get_tables(string& line, int& i, List<string>& selected_tables) {
    bool more = true;
    int count = 0;

    while (more) {
        string name;
        if (i > line.length()) throw runtime_error("Wrong syntax");

        while (line[i] != ' ' && line[i] != ',' && i < line.length()) {
            name.push_back(line[i]);
            i++;
        }

        if (selected_tables.find(name) != -1) throw runtime_error("Wrong syntax");

        selected_tables.push(name);

        name = "";
        if (line[i] == ' ' || i >= line.length()) more = false;
        else i++;
        i++;
    }
}

void copyList(List<string>& from, List<string>& to) {
    for (int i = 0; i < from.length; i++) {
        to.push(from[i]);
    }
}


string take_string(string& line, int& i) {
    string res;
    while (line[i] != ' ' && i < line.length()) {
        res.push_back(line[i]);
        i++;
    }
    i++;
    return res;
}

void double_clear(List<List<string>>& formed) {
    for (int o = 0; o < formed.length; o++) {
        formed[o].clear();
    }
    formed.clear();
}

void cross_joint(List<List<string>>& itog, List<List<string>>& pred) {
    if (pred.length == 0) return;

    List<List<string>> res;

    for (int i = 0; i < itog.length; i++) {
        for (int j = 0; j < pred.length; j++) {

            List<string> prom1;
            List<string> geted = itog[i];

            copyList(geted, prom1);

            for (int o = 0; o < pred[j].length; o++) {
                prom1.push(pred[j][o]);
            }

            res.push(prom1);
        }
    }

    double_clear(itog);
    itog = res;
}

bool check_synt(List<string>& selected_tables, List<string>& from_tables) {
    for (int i = 0; i < selected_tables.length; i++) {
        if (from_tables.find(selected_tables[i]) == -1) return false;
    }
    for (int i = 0; i < from_tables.length; i++) {
        if (selected_tables.find(from_tables[i]) == -1) return false;
    }

    return true;
}

void printRows(List<List<string>>& to_print) {
    for (int i = 0; i < to_print.length; i++) {
        for (int j = 0; j < to_print[i].length; j++) {
            cout << to_print[i][j] << ' ';
        }
        cout << endl;
    }
}

void printMenu() {
    cout << "Доступные команды:" << endl;
    cout << "1. SELECT INTO <название таблицы> VALUES ('значение 1', 'значение2') - выбрать данные" << endl;
    cout << "2. INSERT - вставить данные" << endl;
    cout << "3. DELETE - удалить данные" << endl;
    cout << "4. END - завершить работу" << endl;
}

void printFilteredRows(List<List<string>>& to_print, List<bool>& cmp) {
    for (int i = 0; i < to_print.length; i++) {
        if (!cmp[i]) continue;
        for (int j = 0; j < to_print[i].length; j++) {
            cout << to_print[i][j] << ' ';
        }
        cout << endl;
    }
}

string unwrap_string(string str) {
    string res;

    int index = 0;
    bool geting = false;

    while (index < str.length()) {
        if (str[index] == '\'') {
            if (geting) return res;
            else geting = true;
        }
        else if (geting) res.push_back(str[index]);

        index++;
    }

    throw runtime_error("Invalid argument");
}

void form(string& schem_name, List<string>& from_tables, HashTable<List<string>>& rasp, List<string>& posl, List<List<string>>& old_formed, HashTable<List<string>>& tables) {
    filesystem::path  dirPath = ".";

    for (int k = 0; k < from_tables.length; k++) {

        List<List<string>> formed;

        for (int j = 0; j < rasp.Get(from_tables[k]).length; j++) {
            string to_posl = from_tables[k] + '.' + rasp.Get(from_tables[k])[j];
            posl.push(to_posl);

            List<List<string>> geted;

            readFromCsv(dirPath / schem_name / from_tables[k] / "1.csv", geted, from_tables[k], schem_name);

            int index = tables.Get(from_tables[k]).find(rasp.Get(from_tables[k])[j]);

            List<List<string>> newFromed;



            double_clear(formed);
            formed = newFromed;

            double_clear(geted);
        }

        if (old_formed.length >= formed.length) {
            cross_joint(old_formed, formed);
            double_clear(formed);
        }
        else {
            cross_joint(formed, old_formed);
            double_clear(old_formed);
            old_formed = formed;
        }
    }

}

void where_select(string& line, int& i, string& logi, List<string>& posl, List<List<string>>& old_formed, List<bool>& cmp) {
    while (logi != "") {
        string first = take_string(line, i);
        string op = take_string(line, i);
        if (op != "=") throw runtime_error("Invalid operator");
        string second = take_string(line, i);

        if (posl.find(first) == -1) first = unwrap_string(first);
        if (posl.find(second) == -1) second = unwrap_string(second);

        int sod_first = posl.find(first);
        int sod_second = posl.find(second);

        List<bool> cmp_new;




        List<bool> cmp_res;

        for (int j = 0; j < cmp.length; j++) {
            if (logi == "AND") cmp_res.push(cmp[j] && cmp_new[j]);
            else if (logi == "OR") cmp_res.push(cmp[j] || cmp_new[j]);
            else throw runtime_error("Invalid logick");
        }

        cmp.clear();
        cmp_new.clear();
        cmp = cmp_res;

        logi = take_string(line, i);
    }
}

void select(string& line, int& i, string& schem_name, HashTable<List<string>>& tables) {
    filesystem::path dirPath = ".";
    List<string> selected_tables;
    List<string> selected_columns;
    get_names(line, i, selected_tables, selected_columns);
    string com = take_string(line, i);
    if (com != "FROM") throw runtime_error("Wrong syntax");

    List<string> from_tables;
    get_tables(line, i, from_tables);

    if (!check_synt(selected_tables, from_tables)) throw runtime_error("Wrong syntax");

    HashTable<List<string>> rasp;


    List<List<string>> old_formed;
    List<string> posl;

    form(schem_name, from_tables, rasp, posl, old_formed, tables);

    com = take_string(line, i);
    string logi = "AND";

    List<bool> cmp;
    for (int j = 0; j < old_formed.length; j++) {
        cmp.push(true);
    }

    if (com == "WHERE") {
        where_select(line, i, logi, posl, old_formed, cmp);
    }

    printFilteredRows(old_formed, cmp);

    selected_tables.clear();
    selected_columns.clear();

    for (int h = 0; h < from_tables.length; h++) {
        rasp.Get(from_tables[h]).clear();
    }

    from_tables.clear();
    posl.clear();
    double_clear(old_formed);
}

string take_list(string& cont, int& index) {
    string content;
    bool get_content = 0;
    bool geting_content = 0;

    while (index < cont.length()) {
        if (get_content) break;
        else if (!get_content) {
            if (cont[index] == '(' || cont[index] == ')') {
                if (geting_content) get_content = true;
                else geting_content = true;
            }

            else if (geting_content) content.push_back(cont[index]);
        }

        index++;
    }
    return content;
}

void insert(string& line, int& i, HashTable<List<string>>& tables, string& schem_name, int& limit) {
    filesystem::path dirPath = ".";
    string table_name = take_string(line, i);

    string cm = take_string(line, i);
    List<string> values;

    if (cm != "VALUES") throw runtime_error("Wrong syntax");

    string wraped = take_list(line, i);

    bool more = true;

    string value;
    int j = 0;

    while (more) {
        if (j >= wraped.length()) more = false;
        if (wraped[j] == ',') {
            value = unwrap_string(value);
            values.push(value);
            value = "";
            j++;
        }
        else value.push_back(wraped[j]);

        j++;
    }
    if (value != "") values.push(unwrap_string(value));

    if (tables.Get(table_name).length > values.length) throw runtime_error("Too few arguments");
    if (tables.Get(table_name).length < values.length) throw runtime_error("Too many argumets");

    int pk = readPrimaryKeyValue(dirPath / schem_name / table_name, table_name);
    if (pk + 1 > limit) throw runtime_error("Out of limit");

    appendToCsv(dirPath / schem_name / table_name / "1.csv", values, table_name, schem_name);
    setPrimaryKeyFile(dirPath / schem_name / table_name, table_name, pk + 1);

    values.clear();


}

void deleting(string& line, int& i, HashTable<List<string>>& tables, string& schem_name) {
    filesystem::path dirPath = ".";

    List<List<string>> table;
    List<List<string>> new_table;
    List<string> headers;
    int counter = 0;

    string table_name = take_string(line, i);

    readFromCsv(dirPath / schem_name / table_name / "1.csv", new_table, table_name, schem_name);

    for (int i = 1; i < new_table.length; i++) {
        List<string> tek = new_table[i];
        List<string> nov;
        copyList(tek, nov);
        table.push(nov);
    }

    List<string> newHeaders = new_table[0];

    for (int j = 0; j < newHeaders.length; j++) {
        headers.push(table_name + '.' + newHeaders[j]);
    }
    double_clear(new_table);

    List<bool> cmp;

    for (int i = 0; i < table.length; i++) {
        cmp.push(true);
    }

    string logi = "AND";

    if (take_string(line, i) == "WHERE") where_select(line, i, logi, headers, table, cmp);

    List<List<string>> result;
    List<string> origHead = tables.Get(table_name);
    List<string> headersDel;

    copyList(origHead, headersDel);

    result.push(headersDel);

    for (int i = 0; i < table.length; i++) {
        if (!cmp[i]) result.push(table[i]);
        else counter++;
    }

    writeToCsv(dirPath / schem_name / table_name / "1.csv", result, table_name, schem_name);
    int pk = readPrimaryKeyValue(dirPath / schem_name / table_name, table_name);
    pk -= counter;
    setPrimaryKeyFile(dirPath / schem_name / table_name, table_name, pk);


    double_clear(table);
    double_clear(result);
    headers.clear();
}

void console_parse(string& schem_name, HashTable<List<string>>& tables, List<string>& tables_names, int& limit) {
    filesystem::path dirPath = ".";
    string line;
    bool ended = true;

    printMenu(); // выводим меню команд

    while (ended) {
        try {
            getline(cin, line);
            string command;
            int i = 0;

            while (i < line.length()) {
                while (line[i] != ' ' && i < line.length()) {
                    command.push_back(line[i]);
                    i++;
                }
                i++;

                if (command == "SELECT") {
                    select(line, i, schem_name, tables);
                }
                else if (command == "INSERT") {
                    string cm = take_string(line, i);
                    if (cm != "INTO") throw runtime_error("Wrong syntax");
                    insert(line, i, tables, schem_name, limit);
                }
                else if (command == "DELETE") {
                    string cm = take_string(line, i);
                    if (cm != "FROM") throw runtime_error("Wrong syntax");
                    deleting(line, i, tables, schem_name);
                }
                else if (command == "END") {
                    ended = false;
                    break;
                }
                else {
                    throw runtime_error("Unknown command");
                }

                // После выполнения команды показываем меню
                printMenu();
                command.clear(); // очищаем команду для следующего ввода
            }
        }
        catch (runtime_error& err) {
            cerr << err.what() << endl;
        }
    }
}



int main() {
    HashTable<List<string>> tables;
    List<string> tables_names;
    string schem_name;
    int limit;

    getJSON(tables, tables_names, schem_name, limit);
    check_csv(schem_name, tables, tables_names);

    console_parse(schem_name, tables, tables_names, limit);
}
