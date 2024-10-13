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
