#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> 
#include <string>

class Contact {
private:
    char* name;
    std::string homePhone;
    std::string workPhone;
    std::string mobilePhone;
    std::string additionalInfo;

public:
    Contact(const char* name, const std::string& home, const std::string& work,
        const std::string& mobile, const std::string& info)
        : homePhone(home), workPhone(work), mobilePhone(mobile), additionalInfo(info) {
        this->name = new char[strlen(name) + 1];
        strcpy_s(this->name, strlen(name) + 1, name);
    }

    Contact() : name(nullptr) {}

    Contact(const Contact& other) {
        name = new char[strlen(other.name) + 1];
        strcpy_s(name, strlen(other.name) + 1, other.name);
        homePhone = other.homePhone;
        workPhone = other.workPhone;
        mobilePhone = other.mobilePhone;
        additionalInfo = other.additionalInfo;
    }

    Contact& operator=(const Contact& other) {
        if (this == &other) return *this;
        delete[] name;
        name = new char[strlen(other.name) + 1];
        strcpy_s(name, strlen(other.name) + 1, other.name);
        homePhone = other.homePhone;
        workPhone = other.workPhone;
        mobilePhone = other.mobilePhone;
        additionalInfo = other.additionalInfo;
        return *this;
    }

    ~Contact() {
        delete[] name;
    }

    inline const char* getName() const { return name; }
    inline const std::string& getHomePhone() const { return homePhone; }
    inline const std::string& getWorkPhone() const { return workPhone; }
    inline const std::string& getMobilePhone() const { return mobilePhone; }
    inline const std::string& getAdditionalInfo() const { return additionalInfo; }

    void display() const {
        std::cout << "ФИО: " << name << "\n"
            << "Домашний телефон: " << homePhone << "\n"
            << "Рабочий телефон: " << workPhone << "\n"
            << "Мобильный телефон: " << mobilePhone << "\n"
            << "Дополнительная информация: " << additionalInfo << "\n\n";
    }

    void saveToFile(std::ofstream& out) const {
        size_t len = strlen(name);
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(name, len);
        out << homePhone << '\n' << workPhone << '\n'
            << mobilePhone << '\n' << additionalInfo << '\n';
    }

    void loadFromFile(std::ifstream& in) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        delete[] name;
        name = new char[len + 1];
        in.read(name, len);
        name[len] = '\0';

        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        std::getline(in, homePhone);
        std::getline(in, workPhone);
        std::getline(in, mobilePhone);
        std::getline(in, additionalInfo);
    }
};

class PhoneBook {
private:
    std::vector<Contact> contacts;

public:
    void addContact(const Contact& contact) {
        contacts.push_back(contact);
    }

    void deleteContact(const char* name) {
        auto it = std::remove_if(contacts.begin(), contacts.end(),
            [name](const Contact& c) { return strcmp(c.getName(), name) == 0; });
        if (it != contacts.end()) {
            contacts.erase(it, contacts.end());
            std::cout << "Контакт удален.\n";
        }
        else {
            std::cout << "Контакт не найден.\n";
        }
    }

    void findContact(const char* name) const {
        for (const auto& contact : contacts) {
            if (strcmp(contact.getName(), name) == 0) {
                contact.display();
                return;
            }
        }
        std::cout << "Контакт не найден.\n";
    }

    void showAllContacts() const {
        if (contacts.empty()) {
            std::cout << "Список контактов пуст.\n";
            return;
        }
        for (const auto& contact : contacts) {
            contact.display();
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            std::cerr << "Ошибка открытия файла для записи.\n";
            return;
        }
        size_t count = contacts.size();
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (const auto& contact : contacts) {
            contact.saveToFile(out);
        }
        std::cout << "Контакты сохранены в файл.\n";
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            std::cerr << "Ошибка открытия файла для чтения.\n";
            return;
        }
        size_t count;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        contacts.clear();
        for (size_t i = 0; i < count; ++i) {
            Contact contact;
            contact.loadFromFile(in);
            contacts.push_back(contact);
        }
        std::cout << "Контакты загружены из файла.\n";
    }
};

int main() {
    PhoneBook phoneBook;

    phoneBook.addContact(Contact("Иван Иванов", "123-456", "234-567", "345-678", "Друг"));
    phoneBook.addContact(Contact("Петр Петров", "456-789", "567-890", "678-901", "Коллега"));

    std::cout << "Все контакты:\n";
    phoneBook.showAllContacts();

    std::cout << "Поиск контакта 'Иван Иванов':\n";
    phoneBook.findContact("Иван Иванов");

    phoneBook.deleteContact("Петр Петров");

    std::cout << "Все контакты после удаления:\n";
    phoneBook.showAllContacts();

    phoneBook.saveToFile("contacts.dat");
    phoneBook.loadFromFile("contacts.dat");

    std::cout << "Все контакты после загрузки из файла:\n";
    phoneBook.showAllContacts();

    return 0;
}
