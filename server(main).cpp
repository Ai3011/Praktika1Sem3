#include <iostream> 
#include <thread> 
#include <mutex> 
#include <memory> 
#include <netinet/in.h> // Для работы с сокетами 
#include <unistd.h> // Для close() 
#include <cstring> // Для memset 
#include <stdexcept> 
#include "Commands.h" // Подключение вашей реализации команд 

using namespace std; 
 
std::mutex db_mutex; // Мьютекс для синхронизации доступа к БД 
 
// Функция для обработки запросов клиента 
void handle_client(int client_socket, string& schem_name,  
                   shared_ptr<HashTable<List<string>>> tables,  
                   shared_ptr<List<string>> tables_names, int limit) { 
    char buffer[1024] = {0}; 
 
    // Получаем данные от клиента 
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0); 
    if (bytes_received < 0) { 
        cerr << "Error receiving data" << endl; 
        close(client_socket); 
        return; 
    } 
    buffer[bytes_received] = '\0'; // Завершаем строку 
    string command(buffer); 
 
    try { 
        std::lock_guard<std::mutex> lock(db_mutex); // Защищаем работу с БД 
 
        // Обработка команды 
        console_parse(schem_name, *tables, *tables_names, limit); // Обработка через функцию консоли 
 
        // Ответ клиенту 
        string response = "Query executed successfully"; 
        send(client_socket, response.c_str(), response.size(), 0); 
    } catch (const std::exception& e) { 
        string response = "Error: " + string(e.what()); 
        send(client_socket, response.c_str(), response.size(), 0); 
    } 
 
    close(client_socket); 
} 
 
int main() { 
    auto tables = make_shared<HashTable<List<string>>>(); 
    auto tables_names = make_shared<List<string>>(); 
    string schem_name; 
    int limit; 
 
    // Загрузка данных из JSON-файла 
    try { 
        getJSON(*tables, *tables_names, schem_name, limit); 
    } catch (const std::exception& e) { 
        cerr << "Failed to load JSON data: " << e.what() << endl; 
        return EXIT_FAILURE; // Завершение программы, если загрузка данных не удалась 
    } 
 
    // Настройка сервера 
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_socket < 0) throw runtime_error("Unable to create socket"); 
 
    sockaddr_in server_addr{}; 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(7432); 
 
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { 
        close(server_socket); 
        throw runtime_error("Bind failed"); 
    } 
    if (listen(server_socket, 5) < 0) { 
        close(server_socket); 
        throw runtime_error("Listen failed"); 
    } 
 
    cout << "Server is listening on port 7432..." << endl; 
 
    while (true) { 
        int client_socket = accept(server_socket, nullptr, nullptr); 
        if (client_socket < 0) { 
            cerr << "Accept failed" << endl; 
            continue; // Продолжаем принимать других клиентов 
        } 
         
        // Создаем поток для каждого клиента 
        thread t(handle_client, client_socket, ref(schem_name), tables, tables_names, limit); 
        t.detach(); // Запускаем поток в фоновом режиме 
    } 
 
    close(server_socket); // Хотя этот код никогда не будет достигнут, хорошо его иметь 
    return 0; 
}
