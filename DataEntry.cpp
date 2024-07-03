#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct DataEntry {
    std::string name;
    int age;
    double marks;
};

DataEntry parseCSVLine(const std::string& line) {
    std::stringstream ss(line);
    std::string name, age_str, marks_str;
    std::getline(ss, name, ',');
    std::getline(ss, age_str, ',');
    std::getline(ss, marks_str, ',');
    int age = std::stoi(age_str);
    double marks = std::stod(marks_str);
    return DataEntry{name, age, marks};
}

std::vector<DataEntry> parseCSVFile(const std::string& filename) {
    std::vector<DataEntry> entries;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            entries.emplace_back(parseCSVLine(line));
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
    return entries;
}

int main() {
    std::string filename = "data.csv";
    std::vector<DataEntry> entries = parseCSVFile(filename);
    
    if (entries.empty()) {
        std::cerr << "No data entries found in the CSV file." << std::endl;
        return 1;
    }
    
    for (const auto& entry : entries) {
        std::cout << "Name: " << entry.name << ", Age: " << entry.age << ", Marks: " << entry.marks << std::endl;
    }
    
    return 0;
}
