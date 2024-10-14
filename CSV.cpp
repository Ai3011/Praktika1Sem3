#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include "structures.h"


void lock(const filesystem::path& directory, const std::string tableName) {
    
    filesystem::path filePath = directory / (tableName + "_lock");

    ofstream file(filePath, ios::out | ios::trunc);

    file << 0;

    file.close();
}

void unlock(const filesystem::path& directory, const std::string tableName) {
    
    filesystem::path filePath = directory / (tableName + "_lock");

    ofstream file(filePath, ios::out | ios::trunc);

    file << 1;

    file.close();
}

int isunlocked(const filesystem::path& directory, const std::string& tableName) {

    filesystem::path filePath = directory / (tableName + "_lock");

    ifstream file(filePath);

    
    int value = 1;
    file >> value;

    file.close();

    return value;
}

void writeToCsv(const filesystem::path& filename, List<List<string>>& data, string table_name, string& schem_name) {
    filesystem::path filePath = ".";
    while(isunlocked(filePath/schem_name/table_name, table_name) == 0){ 
        cout << "Waiting for unlock" << endl;
    }

    lock(filePath/ schem_name/table_name, table_name);
    ofstream file(filename, ios::out | ios::trunc);

    for (int i = 0; i < data.length; i++) {
        for (size_t j = 0; j < data[i].length; j++) {
            file << data[i][j];
            if (j < data[i].length - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
    unlock(filePath/ schem_name/table_name, table_name);
}

void readFromCsv(const filesystem::path& filename, List<List<string>>& data, string table_name, string& schem_name) {

    filesystem::path filePath = ".";
    while(!isunlocked(filePath/schem_name/table_name, table_name)) {
        cout << "Waiting for unlock" << endl;
    }
    lock(filePath/ schem_name/table_name, table_name);


    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        List<string> row;
        string cell;
        istringstream lineStream(line);

        while (getline(lineStream, cell, ',')) {
            row.push(cell);
        }

        data.push(row);
    }

    file.close();
    unlock(filePath/ schem_name/table_name, table_name);
}


void appendToCsv(const filesystem::path& filename, List<string>& newRow, string table_name, string& schem_name) {

    filesystem::path filePath = ".";
    while(!isunlocked(filePath/schem_name/table_name, table_name)) {
        cout << "Waiting for unlock" << endl;
    }
    lock(filePath/ schem_name/table_name, table_name);
    
    ofstream file(filename, ios::app);

    for (size_t i = 0; i < newRow.length; ++i) {
        file << newRow[i];
        if (i < newRow.length - 1) {
            file << ",";
        }
    }
    file << "\n";

    file.close();
    unlock(filePath/ schem_name/table_name, table_name);
}
