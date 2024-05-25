#include "main.h"

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
    std::regex pattern("^(0[1-9]|1[0-9]|2[0-3]):[0-5][0-9]$");
    return regex_match(timeStr, pattern);
}
bool isName(const std::string &name) // проверка соответсвия формату имени
{
    std::regex pattern("[a-zA-Z0-9_-]+");
    return regex_match(name, pattern);
}
void Action::setTime(const std::string &tiStr1, const std::string &tiStr2)
{
    timeStr1 = tiStr1;
    timeStr2 = tiStr2;
    time1 = convertTimeStrToTimeT(tiStr1);
    time2 = convertTimeStrToTimeT(tiStr2);
}
void Action::setNumTables(const std::string &numTablesStr)
{
    num_tables = stoi(numTablesStr);
    tables.resize(num_tables);
}
void Action::setPrice(const std::string &priceStr)
{
    price = stoi(priceStr);
}
void Action::changeTable(const std::string &nameClient, const int &num_table, const std::chrono::minutes &time_action)
{
    int temp_num_table = clients[nameClient];
    tables[temp_num_table - 1].name_client = "";
    tables[temp_num_table - 1].duration += time_action - tables[temp_num_table - 1].time_from;
    tables[temp_num_table - 1].earnings += static_cast<int>(ceil(static_cast<double>((time_action - tables[temp_num_table - 1].time_from).count()) / 60)) * price;
    clients[nameClient] = num_table;
    tables[num_table - 1].name_client = nameClient;
    tables[num_table - 1].time_from = time_action;
}
void Action::putClientAtTable(const std::string &nameClient, const int &num_table, const std::chrono::minutes &time_action)
{
    clients[nameClient] = num_table;
    tables[num_table - 1].name_client = nameClient;
    tables[num_table - 1].time_from = time_action;
    count_tables++;
}
void Action::clientHasLeft(const std::string &nameClient, const int &num_table, const std::chrono::minutes &time_action, std::ostringstream &oss)
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
            oss << convertTimeTToTimeStr(time_action) << " 12 " << new_client << " " << new_table << std::endl;
            queue.pop();
            count_tables++;
        }
    }
}

void Action::closingClub(std::ostringstream &oss)
{
    for (const auto &[client, table] : clients)
    {
        oss << timeStr2 << " 11 " << client << std::endl;
        tables[table - 1].duration += time2 - tables[table - 1].time_from;
        tables[table - 1].earnings += static_cast<int>(ceil(static_cast<double>((time2 - tables[table - 1].time_from).count()) / 60)) * price;
    }
    oss << timeStr2 << std::endl;
    for (int i{}; i < tables.size(); i++)
        oss << i + 1 << " " << tables[i].earnings << " " << convertTimeTToTimeStr(tables[i].duration) << std::endl;
}

void parse(std::string path)
{
    std::ostringstream oss;
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "The file could not be opened." << std::endl;
        return;
    }
    std::string line;               // считанная строка
    std::vector<std::string> words; // хранение слов в строке
    int count_lines{};              // счётчик строк
    Action act;
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
                act.setNumTables(words[0]);
            else
            {
                std::cout << line << std::endl;
                return;
            }
        }
        else if (count_lines == 1) // обработка 2й строки
        {
            if (isTime(words[0]) && isTime(words[1]) && (words.size() == 2))
                act.setTime(words[0], words[1]);
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
                act.setPrice(words[0]);
                oss << act.timeStr1 << std::endl;
            }
            else
            {
                std::cout << line << std::endl;
                return;
            }
        }
        else // обработка остальных строк
        {
            oss << line << std::endl;
            std::string time_actionStr = ""; // время события
            std::chrono::minutes time_action;
            int action{};                // id входного события
            std::string nameClient = ""; // имя клиента
            int num_table{};             // номер стола
            if ((words.size() == 3) || (words.size() == 4))
            {
                if (isTime(words[0]) && isNumber(words[1]) && isName(words[2]))
                {
                    time_actionStr = words[0];
                    time_action = convertTimeStrToTimeT(words[0]);
                    action = stoi(words[1]);
                    nameClient = words[2];
                }
                else
                {
                    std::cout << line << std::endl;
                    return;
                }
            }
            else
            {
                std::cout << line << std::endl;
                return;
            }
            if (words.size() == 4)
            {
                if (isNumber(words[3]))
                    num_table = stoi(words[3]);
                else
                {
                    std::cout << line << std::endl;
                    return;
                }
            }
            if (count_lines == 3)
                act.time_temp = time_action;
            else if (time_action < act.time_temp)
            {
                std::cout << line << std::endl;
                return;
            }
            switch (action) // обработка в зависимости от id события
            {
            case 1:
                if (time_action < act.time1)
                    oss << time_actionStr << " 13 NotOpenYet" << std::endl;
                else if (act.clients.count(nameClient) == 1)
                    oss << time_actionStr << " 13 YouShallNotPass" << std::endl;
                else
                    act.clients[nameClient] = 0;
                break;
            case 2:
                if (act.clients.count(nameClient) == 0)
                    oss << time_actionStr << " 13 ClientUnknown" << std::endl;
                else if (act.tables[num_table - 1].name_client != "")
                    oss << time_actionStr << " 13 PlaceIsBusy" << std::endl;
                else if (act.clients[nameClient] != 0)
                    act.changeTable(nameClient, num_table, time_action);
                else
                    act.putClientAtTable(nameClient, num_table, time_action);
                break;
            case 3:
                if (act.clients.count(nameClient) == 0)
                    oss << time_actionStr << " 13 ClientUnknown" << std::endl;
                else if (act.count_tables != act.num_tables)
                    oss << time_actionStr << " 13 ICanWaitNoLonger!" << std::endl;
                else if (act.queue.size() >= act.num_tables)
                {
                    oss << time_actionStr << " 11 " << nameClient << std::endl;
                    act.clients.erase(nameClient);
                }
                else
                    act.queue.push(nameClient);
                break;
            case 4:
                if (act.clients.count(nameClient) == 0)
                    oss << time_actionStr << " 13 ClientUnknown" << std::endl;
                else
                    act.clientHasLeft(nameClient, num_table, time_action, oss);
                break;
            default:
                std::cout << line << std::endl;
                return;
            }
            act.time_temp = time_action;
        }
        count_lines++;
    }
    file.close();
    act.closingClub(oss);
    std::cout << oss.str();
}
int main(int argc, char *argv[])
{
    if (argc > 1)
        parse(argv[1]);
    else
        std::cerr << "No file name provided." << std::endl;
    getchar();
    return 0;
}
