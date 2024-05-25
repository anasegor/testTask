#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <chrono>
#include <iomanip>
#include <queue>
#include <map>
#include <cmath>

struct Table // структура стола
{
    int earnings{};                                                        // прибыль
    std::chrono::minutes duration = static_cast<std::chrono::minutes>(0);  // длительность, на которую стол был занят
    std::chrono::minutes time_from = static_cast<std::chrono::minutes>(0); // время, с которого стол занят
    std::string name_client = "";                                          // имя клиента. Пустая строка, если стол не занят
};
struct Action
{
    std::queue<std::string> queue;      // хранение очереди клиентов
    std::map<std::string, int> clients; // для клиентов, которые пришли и номер занятого ими стола.
                                        // если клиент не сел, номер стола=0
    std::vector<Table> tables;          // столы
    int count_tables{};                 // счетчик занятых столов
    std::chrono::minutes time_temp;     // переменная для проверки последовательности событий во времени
    int num_tables{};                   // кол-во столов
    std::string timeStr1 = "";          // дата открытия в формате строки
    std::string timeStr2 = "";          // дата закрытия в формате строки
    std::chrono::minutes time1;         // дата открытия в формате длительности от начала дня (от 00:00)
    std::chrono::minutes time2;         // дата закрытия в формате длительности от начала дня (от 00:00)
    int price{};                        // цена за час

    // установка времени открытия и закрытия клуба
    void setTime(const std::string &tiStr1, const std::string &tiStr2);
    // установка количества столов
    void setNumTables(const std::string &numTablesStr);
    // установка прайса
    void setPrice(const std::string &priceStr);
    // клиент меняет свой стол
    void changeTable(const std::string &nameClient, const int &num_table, const std::chrono::minutes &time_action);
    // клиент садится за стол
    void putClientAtTable(const std::string &nameClient, const int &num_table, const std::chrono::minutes &time_action);
    // клиент уходит
    void clientHasLeft(const std::string &nameClient, const int &num_table, const std::chrono::minutes &time_action, std::ostringstream &oss);
    // клуб закрывается
    void closingClub(std::ostringstream &oss);
};