#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <limits>

using namespace std;


struct CompareVotes {
    bool operator()(pair<int, string>& a, pair<int, string>& b) {
        return a.first < b.first;
    }
};


class AuthSystem {
private:
    const string adminID = "admin";
    const string adminPass = "admin@secure123";

public:
    bool login(string& usernameOut, bool& isAdmin) {
        string username, password;
        cout << "Login Page\nUsername: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        if (username == adminID && password == adminPass) {
            usernameOut = username;
            isAdmin = true;
            cout << "Admin login successful.\n";
            return true;
        }

        ifstream file("users.txt");
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string storedUser, storedPass;
            iss >> storedUser >> storedPass;
            if (storedUser == username && storedPass == password) {
                usernameOut = username;
                isAdmin = false;
                cout << " User login successful. Welcome, " << username << "!\n";
                return true;
            }
        }

        cout << "Invalid credentials.\n";
        return false;
    }

    void registerUser() {
        string username, password;
        cout << "Registration Page\nChoose a username: ";
        cin >> username;
        cout << "Choose a password: ";
        cin >> password;

        ifstream infile("users.txt");
        string line;
        while (getline(infile, line)) {
            istringstream iss(line);
            string existingUser, existingPass;
            iss >> existingUser >> existingPass;
            if (existingUser == username) {
                cout << "Username already exists. Try a different one.\n";
                return;
            }
        }

        ofstream file("users.txt", ios::app);
        file << username << " " << password << "\n";
        cout << "Registration successful. You can now log in.\n";
    }
};
class VotingSystem {
private:
    unordered_map<int, string> candidateMap = {
        {1, "BJP"},
        {2, "CON"},
        {3, "JDU"},
        {4, "RJD"}
    };
    unordered_map<string, int> voteMap;

public:
    VotingSystem() {
        loadVotes();
    }

    void loadVotes() {
        ifstream file("votes.txt");
        string name;
        int count;
        while (file >> name >> count) {
            voteMap[name] = count;
        }
    }

    void saveVotes() {
        ofstream file("votes.txt");
        for (auto& it : voteMap) {
            file << it.first << " " << it.second << "\n";
        }
    }

    bool hasUserVoted(const string& username) {
        ifstream file("voted_users.txt");
        string line;
        while (getline(file, line)) {
            if (line == username) {
                return true;
            }
        }
        return false;
    }

    void markUserVoted(const string& username) {
        ofstream file("voted_users.txt", ios::app);
        file << username << "\n";
    }

    void showCandidates() {
        cout << "\n Available Candidates:\n";
        for (auto& pair : candidateMap) {
            cout << pair.first << ". " << pair.second << "\n";
        }
    }

    void castVote(int candidateNumber) {
        if (candidateMap.find(candidateNumber) == candidateMap.end()) {
            cout << "Invalid candidate number. Please choose a valid option.\n";
            return;
        }
        string candidate = candidateMap[candidateNumber];
        voteMap[candidate]++;
        saveVotes();
        cout << "Vote casted for " << candidate << "!\n";
    }

    void showLeaderboard(int k) {
        priority_queue<pair<int, string>, vector<pair<int, string>>, CompareVotes> heap;
        for (auto& it : voteMap) {
            heap.push(make_pair(it.second, it.first));
        }

        cout << "\n Top " << k << " Candidates:\n";
        for (int i = 0; i < k && !heap.empty(); ++i) {
            pair<int, string> top = heap.top();
            heap.pop();
            cout << i + 1 << ". " << top.second << " - " << top.first << " votes\n";
        }
    }
};


void showMenu() {
    cout << "\n===== Voting System Menu =====\n";
    cout << "1. Cast Vote\n";
    cout << "2. Show Leaderboard (Admin Only)\n";
    cout << "3. Exit\n";
    cout << "==============================\n";
    cout << "Enter your choice: ";
}


int main() {
    AuthSystem auth;
    string currentUser;
    bool isAdmin = false;

    cout << "Welcome to the Voting System\n";
    cout << "1. Login\n2. Register\nChoose: ";
    int authChoice;
    cin >> authChoice;

    if (authChoice == 2) {
        auth.registerUser();
    }

    if (!auth.login(currentUser, isAdmin)) {
        return 0;
    }

    VotingSystem system;
    int choice;

    while (true) {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }

        if (choice == 1) {
            if (system.hasUserVoted(currentUser)) {
                cout << " You have already voted. Multiple votes are not allowed.\n";
            } else {
                system.showCandidates();
                int candidateNumber;
                cout << "Enter candidate number: ";
                cin >> candidateNumber;
                system.castVote(candidateNumber);
                if (!system.hasUserVoted(currentUser)) {
                    system.markUserVoted(currentUser);
                }
            }
        } else if (choice == 2) {
            if (!isAdmin) {
                cout << " Access denied. Only admin can view the leaderboard.\n";
            } else {
                int k;
                cout << "Enter number of top candidates to display: ";
                cin >> k;
                system.showLeaderboard(k);
            }
        } else if (choice == 3) {
            cout << "Exiting Voting System. Goodbye!\n";
            break;
        } else {
            cout << " Invalid choice. Please select from the menu.\n";
        }
    }

    return 0;
}