#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int startTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
};

// Function to display the results table
void displayResults(vector<Process>& processes) {
    cout << "\n=== FCFS SCHEDULING RESULTS ===" << endl;
    cout << string(90, '-') << endl;
    cout << setw(5) << "PID" << setw(12) << "Arrival" << setw(10) << "Burst" 
         << setw(12) << "Start" << setw(15) << "Completion" 
         << setw(15) << "Turnaround" << setw(12) << "Waiting" << setw(12) << "Response" << endl;
    cout << string(90, '-') << endl;

    double avgTurnaround = 0, avgWaiting = 0, avgResponse = 0;

    for (auto& p : processes) {
        cout << setw(5) << p.id << setw(12) << p.arrivalTime << setw(10) << p.burstTime 
             << setw(12) << p.startTime << setw(15) << p.completionTime 
             << setw(15) << p.turnaroundTime << setw(12) << p.waitingTime << setw(12) << p.responseTime << endl;
        
        avgTurnaround += p.turnaroundTime;
        avgWaiting += p.waitingTime;
        avgResponse += p.responseTime;
    }

    int n = processes.size();
    cout << string(90, '-') << endl;
    cout << "Average Turnaround Time: " << avgTurnaround / n << endl;
    cout << "Average Waiting Time: " << avgWaiting / n << endl;
    cout << "Average Response Time: " << avgResponse / n << endl;
}

// Function to display Gantt Chart
void displayGanttChart(const vector<pair<int, int>>& gantt, const vector<int>& times) {
    cout << "\nGANTT CHART:" << endl;
    cout << "Time: ";
    for (size_t i = 0; i < times.size(); ++i) {
        cout << setw(4) << times[i];
    }
    cout << endl;
    
    cout << "Proc: ";
    for (size_t i = 0; i < gantt.size(); ++i) {
        if (gantt[i].first == -1) {
            cout << setw(4) << "IDLE";
        } else {
            cout << setw(4) << "P" << gantt[i].first;
        }
    }
    cout << endl;
}

// FCFS Scheduling Algorithm
void FCFS(vector<Process> processes) {
    cout << "\n=== FIRST COME FIRST SERVED (FCFS) SCHEDULING ===" << endl;
    
    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    vector<pair<int, int>> gantt;
    vector<int> ganttTimes;
    ganttTimes.push_back(0);

    for (auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            gantt.push_back({-1, p.arrivalTime - currentTime});
            currentTime = p.arrivalTime;
        }

        p.startTime = currentTime;
        p.responseTime = p.startTime - p.arrivalTime;
        p.completionTime = currentTime + p.burstTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;

        gantt.push_back({p.id, p.burstTime});
        ganttTimes.push_back(p.completionTime);
        currentTime = p.completionTime;
    }

    displayResults(processes);
    displayGanttChart(gantt, ganttTimes);
}

// Function to get process data from user
vector<Process> getProcessData() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "\nProcess P" << (i + 1) << ":\n";
        cout << "Enter Arrival Time: ";
        cin >> processes[i].arrivalTime;
        cout << "Enter Burst Time: ";
        cin >> processes[i].burstTime;
    }
    return processes;
}

int main() {
    cout << "=== FCFS CPU SCHEDULING ALGORITHM ===" << endl;
    
    vector<Process> processes = getProcessData();
    FCFS(processes);

    cout << "\nFCFS Scheduling completed!" << endl;
    return 0;
}