#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <limits>
#include <dirent.h>

using namespace std;

// Function to get the current date and time as a string
string getDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char dt[20];
    strftime(dt, sizeof(dt), "%Y-%m-%d_%H-%M", ltm);
    return string(dt);
}

// Function to authenticate the admin
bool login() {
    string username, password;
    int attempts = 3; // Allow 3 attempts

    while (attempts > 0) {
        cout << "\n===== Admin Login =====\n";
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (username == "admin" && password == "hitms123") {
            cout << "Login successful!\n";
            return true;
        } else {
            attempts--;
            cout << "Invalid credentials! Attempts left: " << attempts << "\n";
            if (attempts == 0) {
                cout << "Too many failed attempts! Exiting...\n";
                return false;
            }
        }
    }
    return false;
}

// Function to load roll numbers from a file
vector<string> loadRollNumbers(string filename) {
    vector<string> rolls;
    ifstream file(filename);
    string roll;
    while (getline(file, roll)) {
        if (!roll.empty()) rolls.push_back(roll);
    }
    return rolls;
}

// Function to mark attendance
void markAttendance(const vector<string>& students, string section) {
    string folder = "attendance_records/" + section + "/";
    string filename = folder + "attendance_" + section + "_" + getDateTime() + ".txt";
    ofstream file(filename);
    int present = 0, absent = 0;
    char status;

    cout << "\nPress 'P' for Present and 'A' for Absent\n";
    for (const auto& roll : students) {
        do {
            cout << roll << ": ";
            cin >> status;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            status = toupper(status);
        } while (status != 'P' && status != 'A');

        file << roll << " - " << (status == 'P' ? "Present" : "Absent") << "\n";
        (status == 'P') ? present++ : absent++;
    }

    cout << "\nAttendance saved in " << filename << "\n";
    cout << "Present: " << present << " | Absent: " << absent << "\n";
}

// Function to view attendance
void viewAttendance(string secName) {
    string folder = "attendance_records/" + secName + "/";
    string latestFile;
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(folder.c_str())) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            string fileName = entry->d_name;
            if (fileName.find("attendance_" + secName) != string::npos) {
                latestFile = fileName;
            }
        }
        closedir(dir);
    }

    if (latestFile.empty()) {
        cout << "No attendance records found for " << secName << "!\n";
        return;
    }

    string filename = folder + latestFile;
    ifstream file(filename);
    if (!file) {
        cout << "Error: File not found!\n";
        return;
    }

    string line;
    cout << "\nAttendance Records from " << latestFile << ":\n";
    while (getline(file, line)) cout << line << "\n";
}

// Fixed Function to generate an attendance report for a student
void generateReport() {
    string rollNumber;
    cout << "Enter Roll Number: ";
    cin >> rollNumber;

    int totalDays = 0, presentDays = 0;
    string sections[] = {"SoftwareEng1", "SoftwareEng2"};

    for (const auto& secName : sections) {
        string folder = "attendance_records/" + secName + "/";
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(folder.c_str())) != NULL) {
            while ((entry = readdir(dir)) != NULL) {
                string fileName = entry->d_name;
                if (fileName.find("attendance_" + secName) != string::npos) {
                    ifstream file(folder + fileName);
                    string line;
                    while (getline(file, line)) {
                        if (line.rfind(rollNumber + " -", 0) == 0) { // Fixed condition
                            totalDays++;
                            if (line.find("Present") != string::npos) {
                                presentDays++;
                            }
                        }
                    }
                }
            }
            closedir(dir);
        }
    }

    if (totalDays == 0) {
        cout << "No records found for Roll Number: " << rollNumber << "\n";
    } else {
        cout << "Attendance Report for " << rollNumber << ": " << presentDays << "/" << totalDays << " days present.\n";
    }
}

int main() {
    if (!login()) return 0;

    vector<string> se1 = loadRollNumbers("jeesy_odd_roll_numbers.txt");
    vector<string> se2 = loadRollNumbers("jeesy_even_roll_numbers.txt");

    if (se1.empty() || se2.empty()) {
        cout << "Error: Roll number files missing or empty.\n";
        return 1;
    }

    int choice;
    while (true) {
        cout << "\n====== Student Attendance Management System ======\n";
        cout << "Date & Time: " << getDateTime() << "\n";
        cout << "1. Mark Attendance\n2. View Attendance\n3. Generate Report\n4. Exit\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(1000, '\n');
            cout << "Invalid input! Enter a number.\n";
            continue;
        }

        if (choice == 4) break;
        if (choice == 1) {
            int section;
            cout << "Select Section:\n1. Software Eng 1\n2. Software Eng 2\nChoice: ";
            if (!(cin >> section) || (section != 1 && section != 2)) {
                cin.clear(); cin.ignore(1000, '\n');
                cout << "Invalid input!\n";
                continue;
            }
            string secName = (section == 1) ? "SoftwareEng1" : "SoftwareEng2";
            markAttendance((section == 1) ? se1 : se2, secName);
        } else if (choice == 2) {
            int section;
            cout << "Select Section:\n1. Software Eng 1\n2. Software Eng 2\nChoice: ";
            if (!(cin >> section) || (section != 1 && section != 2)) {
                cin.clear(); cin.ignore(1000, '\n');
                cout << "Invalid input!\n";
                continue;
            }
            string secName = (section == 1) ? "SoftwareEng1" : "SoftwareEng2";
            viewAttendance(secName);
        } else if (choice == 3) {
            generateReport();
        } else {
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}
