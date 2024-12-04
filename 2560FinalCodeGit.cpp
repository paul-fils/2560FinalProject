#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <thread>
#include <chrono>
#include <iomanip> // Required for setprecision

using namespace std;

// List of injuries and their severities
map<string, int> injuryList = {
        {"Gunshot Wound", 1},
        {"Heart Attack", 1},
        {"Stroke", 1},
        {"Severe Allergic Reaction", 1},
        {"Traumatic Brain Injury", 1},
        {"Severe Burn", 1},
        {"Sepsis", 1},
        {"Major Bleeding", 2},
        {"Pneumothorax (Collapsed Lung)", 2},
        {"Compound Fracture", 2},
        {"Severe Asthma Attack", 2},
        {"Severe Dehydration", 2},
        {"Appendicitis", 3},
        {"Kidney Stone", 3},
        {"Severe Migraine", 3},
        {"Broken Bone", 3},
        {"Laceration Requiring Stitches", 3},
        {"High Fever (Adult)", 4},
        {"Mild Concussion", 4},
        {"Sprained Ankle", 4},
        {"Dislocated Shoulder", 4},
        {"Nosebleed (Severe)", 4},
        {"Ear Infection", 5},
        {"Minor Cut", 5},
        {"Skin Rash", 5},
        {"Mild Food Poisoning", 5},
        {"Mild Allergic Reaction", 5},
        {"Cold or Flu", 5},
        {"Minor Burn", 5},
        {"Muscle Strain", 5}
};

// Struct to store all the patient info
struct ERPatient {
    string fullName;        // Patient's full name
    string injuryType;      // Type of injury
    int conditionSeverity;  // Lower = more critical
    time_t checkInTime;     // When the patient showed up (UNIX timestamp)

    // Constructor to initialize a new patient
    ERPatient(string name, string injury, int severity, time_t arrivalTime)
            : fullName(name), injuryType(injury), conditionSeverity(severity), checkInTime(arrivalTime) {}

    // Formats the arrival time into something more readable
    string readableCheckInTime() const {
        char buffer[80];
        struct tm* timeinfo = localtime(&checkInTime);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }
};

// Comparator for sorting patients by urgency and arrival time
struct CompareERPatients {
    bool operator()(const ERPatient& p1, const ERPatient& p2) const {
        if (p1.conditionSeverity == p2.conditionSeverity) {
            return p1.checkInTime > p2.checkInTime; // If severity is the same, earlier gets priority
        }
        return p1.conditionSeverity > p2.conditionSeverity; // More critical conditions come first
    }
};

// This class handles the ER queue and all patient interactions
class ERQueueHandler {
private:
    priority_queue<ERPatient, vector<ERPatient>, CompareERPatients> patientQueue; // The main patient line
    vector<pair<ERPatient, time_t>> treatedPatients; // List of treated patients with their treatment time

public:
    void admitNewPatient(const string& name, const string& injury, time_t checkIn);
    void treatNextInLine();
    void showQueueStatus() const;
    void showTreatedLog() const;
    bool promptForNewPatients();
    bool isQueueEmpty() const; // Added method to check if the queue is empty
};

// Adds a new patient to the queue
void ERQueueHandler::admitNewPatient(const string& name, const string& injury, time_t checkIn) {
    int severity = injuryList[injury];
    ERPatient newPatient(name, injury, severity, checkIn);
    patientQueue.push(newPatient);
    cout << "Added patient: " << name << " (Injury: " << injury
         << ", Severity: " << severity
         << ", Check-in: " << newPatient.readableCheckInTime() << ")." << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
}

// Treats the patient with the highest priority
void ERQueueHandler::treatNextInLine() {
    if (patientQueue.empty()) {
        cout << "Queue is empty. No one left to treat!" << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return;
    }
    ERPatient nextPatient = patientQueue.top();
    patientQueue.pop();
    time_t treatmentTime = time(nullptr); // Record the treatment time
    treatedPatients.push_back({nextPatient, treatmentTime});
    cout << "Treating patient: " << nextPatient.fullName
         << " (Injury: " << nextPatient.injuryType
         << ", Severity: " << nextPatient.conditionSeverity
         << ", Check-in: " << nextPatient.readableCheckInTime() << ")." << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
}

// Displays the current queue
void ERQueueHandler::showQueueStatus() const {
    if (patientQueue.empty()) {
        cout << "Queue is empty. All good here!" << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return;
    }

    priority_queue<ERPatient, vector<ERPatient>, CompareERPatients> tempQueue = patientQueue;
    cout << "=== Current ER Queue ===" << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
    while (!tempQueue.empty()) {
        ERPatient current = tempQueue.top();
        tempQueue.pop();
        cout << "Patient: " << current.fullName
             << ", Injury: " << current.injuryType
             << ", Severity: " << current.conditionSeverity
             << ", Check-in: " << current.readableCheckInTime() << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
    }
    cout << "=========================" << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
}

// Displays the log of treated patients
void ERQueueHandler::showTreatedLog() const {
    if (treatedPatients.empty()) {
        cout << "No patients have been treated yet." << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return;
    }

    cout << "=== Treated Patients Log ===" << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
    for (const auto& record : treatedPatients) {
        const ERPatient& patient = record.first;
        time_t treatmentTime = record.second;
        double waitingTimeMinutes = difftime(treatmentTime, patient.checkInTime) / 60.0; // Convert to minutes

        cout << "Patient: " << patient.fullName
             << ", Injury: " << patient.injuryType
             << ", Severity: " << patient.conditionSeverity
             << ", Waiting Time: " << fixed << setprecision(2) << waitingTimeMinutes << " minutes" << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
    }
    cout << "=============================" << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
}

// Prompts the user to admit new patients
bool ERQueueHandler::promptForNewPatients() {
    char choice;
    while (true) {
        cout << "Do you want to admit a new patient? (y/n): ";
        cin >> choice;

        if (tolower(choice) == 'y' || tolower(choice) == 'n') {
            break; // Valid input
        } else {
            cout << "Invalid input. Please enter 'y' or 'n'." << endl;
        }
    }

    if (tolower(choice) == 'y') {
        string name, injury;
        cout << "Enter the name of the new patient: ";
        cin.ignore();
        getline(cin, name);

        // Show dropdown for injuries
        cout << "Select an injury from the following options:\n";
        int count = 1;
        for (const auto& entry : injuryList) {
            cout << count++ << ". " << entry.first << endl;
        }

        int injuryChoice;
        while (true) {
            cout << "Enter the number corresponding to the injury: ";
            cin >> injuryChoice;

            if (injuryChoice >= 1 && injuryChoice <= injuryList.size()) {
                auto it = injuryList.begin();
                advance(it, injuryChoice - 1);
                injury = it->first;
                break; // Valid input
            } else {
                cout << "Invalid input. Please enter a number between 1 and " << injuryList.size() << "." << endl;
            }
        }

        admitNewPatient(name, injury, time(nullptr));
        return true; // New patient added
    }
    return false; // No patient added
}

// Checks if the queue is empty
bool ERQueueHandler::isQueueEmpty() const {
    return patientQueue.empty();
}

// Main function to run the simulation
int main() {
    ERQueueHandler erSystem;

    cout << "Emergency Room Simulation Starting...\n";
    this_thread::sleep_for(chrono::milliseconds(1500));

    time_t now = time(nullptr);

    // Admit initial patients
    erSystem.admitNewPatient("Paarth Soni", "Broken Bone", now - 10);
    erSystem.admitNewPatient("Zach Hasan", "Sprained Ankle", now - 20);
    erSystem.admitNewPatient("Kian Zarkani", "Heart Attack", now - 5);
    erSystem.admitNewPatient("Jason Ie", "Severe Burn", now - 15);
    erSystem.admitNewPatient("Ronin Lee", "Mild Concussion", now - 2);

    // Show the initial queue
    erSystem.showQueueStatus();

    // Treat patients and prompt for new admissions
    while (!erSystem.isQueueEmpty()) {
        if (erSystem.promptForNewPatients()) {
            erSystem.showQueueStatus(); // Update queue if new patients are added
        }
        erSystem.treatNextInLine();
    }

    erSystem.showTreatedLog();
    cout << "Simulation Complete." << endl;
    return 0;
}

