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
using namespace std;
struct Table // структура стола
{
    int earnings{};                                                        // прибыль
    std::chrono::minutes duration = static_cast<std::chrono::minutes>(0);  // длительность, на который стол был занят
    std::chrono::minutes time_from = static_cast<std::chrono::minutes>(0); // время, с которого стол занят
    std::string name_client = "";                                          // имя клиента. Пустая строка, если стол не занят
};

std::chrono::minutes convertTimeStrToTimeT(const std::string &timeStr) // перевод времени строки hh:mm в формат длительности
{
    size_t pos = timeStr.find(':');
    int hours = std::stoi(timeStr.substr(0, pos));
    int minutes = std::stoi(timeStr.substr(pos + 1));
    std::chrono::minutes duration(hours * 60 + minutes);
    return duration;
}
std::string convertTimeTToTimeStr(const std::chrono::minutes duration) // перевод времени в формат строки hh:mm
{
    int hours = duration.count() / 60;
    int minut = duration.count() % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minut;
    return oss.str();
}
bool isNumber(const std::string &str) // проверка соответсвия формату числа
{
    try
    {
        stoi(str);
        return true;
    }
    catch (std::invalid_argument &)
    {
        return false;
    }
    catch (std::out_of_range &)
    {
        return false;
    }
}
bool isTime(const std::string &timeStr) // проверка соответсвия формату времени
{
    std::regex timePattern("^\\d{2}:\\d{2}$");
    return regex_match(timeStr, timePattern);
}
bool isName(const std::string &name) // проверка соответсвия формату имени
{
    std::regex pattern("[a-zA-Z0-9_-]+");
    return regex_match(name, pattern);
}

void parse(std::string path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return;
    }
    std::string line;               // считанная строка
    std::vector<std::string> words; // хранение слов в строке

    std::queue<std::string> queue;      // хранение очереди клиентов
    std::map<std::string, int> clients; // для клиентов, которые пришли и номер занятого ими стола.
                                        // если клиент не сел, номер стола=0
    std::vector<Table> tables;          // столы
    int count_tables{};                 // счетчик занятых столов
    std::chrono::minutes time_temp;     // переменная для проверки последовательности событий во времени
    int count_lines{};                  // счетчик строк
    int num_tables{};                   // кол-во столов
    std::string timeStr1 = "";          // дата открытия в формате строки
    std::string timeStr2 = "";          // дата закрытия в формате строки
    std::chrono::minutes time1;         // дата открытия в формате длительности от начала дня( от 00:00)
    std::chrono::minutes time2;         // дата закрытия в формате длительности от начала дня( от 00:00)
    int price{};                        // цена за час

    while (getline(file, line)) // чтение строки из файла
    {
        words.clear();
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) // чтение слов из строки
            words.push_back(word);
        if (count_lines == 0) // обработка 1й строки
        {
            if (isNumber(words[0]) && (words.size() == 1))
            {
                num_tables = stoi(words[0]);
                tables.resize(num_tables);
            }
            else
            {
                std::cout << line << std::endl;
                return;
            }
        }
        else if (count_lines == 1) // обработка 2й строки
        {
            if (isTime(words[0]) && isTime(words[1]) && (words.size() == 2))
            {
                timeStr1 = words[0];
                timeStr2 = words[1];
                time1 = convertTimeStrToTimeT(words[0]);
                time2 = convertTimeStrToTimeT(words[1]);
            }
            else
            {
                std::cout << line << std::endl;
                return;
            }
        }
        else if (count_lines == 2) // обработка 3й строки
        {
            if (isNumber(words[0]) && (words.size() == 1))
            {
                price = stoi(words[0]);
                std::cout << timeStr1 << std::endl;
            }
            else
            {
                std::cout << line << std::endl;
                return;
            }
        }
        else // обработка остальных строк
        {
            std::cout << line << std::endl;
            if ((words.size() < 3) && (words.size() > 4))
                return;
            std::string time_actionStr = ""; // время события
            std::chrono::minutes time_action;
            int action{};                // id входного события
            std::string nameClient = ""; // имя клиента
            int num_table{};             // номер стола
            if ((words.size() >= 3) && (words.size() <= 4))
            {
                if (isTime(words[0]) && isNumber(words[1]) && isName(words[2]))
                {
                    time_actionStr = words[0];
                    time_action = convertTimeStrToTimeT(words[0]);
                    action = stoi(words[1]);
                    nameClient = words[2];
                }
                else
                    return;
            }
            else
                return;
            if (words.size() == 4)
            {
                if (isNumber(words[3]))
                    num_table = stoi(words[3]);
                else
                    return;
            }
            if (count_lines == 3)
                time_temp = time_action;
            else if (time_action < time_temp)
                return;
            switch (action) // обработка в зависимости от id события
            {
            case 1:
                if (time_action < time1)
                    std::cout << time_actionStr << " " << 13 << " NotOpenYet" << std::endl;
                else if (clients.count(nameClient) == 1)
                    std::cout << time_actionStr << " " << 13 << " YouShallNotPass" << std::endl;
                else
                    clients[nameClient] = 0;
                break;
            case 2:
                if (clients.count(nameClient) == 0)
                    std::cout << time_actionStr << " " << 13 << " ClientUnknown" << std::endl;
                else if (tables[num_table - 1].name_client != "")
                    std::cout << time_actionStr << " " << 13 << " PlaceIsBusy" << std::endl;
                else if (clients[nameClient] != 0)
                {
                    int temp_num_table = clients[nameClient];
                    tables[temp_num_table - 1].name_client = "";
                    tables[temp_num_table - 1].duration += time_action - tables[temp_num_table - 1].time_from;
                    tables[temp_num_table - 1].earnings += static_cast<int>(ceil(static_cast<double>((time_action - tables[temp_num_table - 1].time_from).count()) / 60)) * price;
                    clients[nameClient] = num_table;
                    tables[num_table - 1].name_client = nameClient;
                    tables[num_table - 1].time_from = time_action;
                }
                else
                {
                    clients[nameClient] = num_table;
                    tables[num_table - 1].name_client = nameClient;
                    tables[num_table - 1].time_from = time_action;
                    count_tables++;
                }
                break;
            case 3:
                if (clients.count(nameClient) == 0)
                    std::cout << time_actionStr << " " << 13 << " ClientUnknown" << std::endl;
                else if (count_tables != num_tables)
                    std::cout << time_actionStr << " " << 13 << " ICanWaitNoLonger!" << std::endl;
                else if (queue.size() >= num_tables)
                {
                    std::cout << time_actionStr << " " << 11 << " " << nameClient << std::endl;
                    clients.erase(nameClient);
                }
                else
                    queue.push(nameClient);
                break;
            case 4:
                if (clients.count(nameClient) == 0)
                    std::cout << time_actionStr << " " << 13 << " ClientUnknown" << std::endl;
                else
                {
                    int new_table = clients[nameClient];
                    clients.erase(nameClient);
                    if (new_table != 0)
                    {
                        tables[new_table - 1].name_client = "";
                        tables[new_table - 1].duration += time_action - tables[new_table - 1].time_from;
                        tables[new_table - 1].earnings += static_cast<int>(ceil(static_cast<double>((time_action - tables[new_table - 1].time_from).count()) / 60)) * price;
                        count_tables--;
                        if (!queue.empty())
                        {
                            std::string new_client = queue.front();
                            tables[new_table - 1].name_client = new_client;
                            tables[new_table - 1].time_from = time_action;
                            clients[new_client] = new_table;
                            std::cout << time_actionStr << " " << 12 << " " << new_client << " " << new_table << std::endl;
                            queue.pop();
                            count_tables++;
                        }
                    }
                }
                break;
            default:
                return;
            }
            time_temp = time_action;
        }
        count_lines++;
    }
    file.close();

    for (const auto &[client, table] : clients)
    {
        std::cout << timeStr2 << " " << 11 << " " << client << std::endl;
        tables[table - 1].duration += time2 - tables[table - 1].time_from;
        tables[table - 1].earnings += static_cast<int>(ceil(static_cast<double>((time2 - tables[table - 1].time_from).count()) / 60)) * price;
    }
    std::cout << timeStr2 << std::endl;
    for (int i{}; i < tables.size(); i++)
        std::cout << i + 1 << " " << tables[i].earnings << " " << convertTimeTToTimeStr(tables[i].duration) << std::endl;
}
int main(int argc, char *argv[])
{
    if (argc == 1)
        parse(argv[1]);
    else
        std::cerr << "Не предоставлено имя файла." << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    getchar();
    return 0;
}