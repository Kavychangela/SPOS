#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int priority;
    int startTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
};

// Function to display the results table
void displayResults(vector<Process>& processes) {
    cout << "\n=== PRIORITY NON-PREEMPTIVE SCHEDULING RESULTS ===" << endl;
    cout << string(100, '-') << endl;
    cout << setw(5) << "PID" << setw(12) << "Arrival" << setw(10) << "Burst" 
         << setw(10) << "Priority" << setw(12) << "Start" << setw(15) << "Completion" 
         << setw(15) << "Turnaround" << setw(12) << "Waiting" << setw(12) << "Response" << endl;
    cout << string(100, '-') << endl;

    double avgTurnaround = 0, avgWaiting = 0, avgResponse = 0;

    for (auto& p : processes) {
        cout << setw(5) << p.id << setw(12) << p.arrivalTime << setw(10) << p.burstTime 
             << setw(10) << p.priority << setw(12) << p.startTime << setw(15) << p.completionTime 
             << setw(15) << p.turnaroundTime << setw(12) << p.waitingTime << setw(12) << p.responseTime << endl;
        
        avgTurnaround += p.turnaroundTime;
        avgWaiting += p.waitingTime;
        avgResponse += p.responseTime;
    }

    int n = processes.size();
    cout << string(100, '-') << endl;
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

// Priority Non-Preemptive Scheduling Algorithm
void Priority_NonPreemptive(vector<Process> processes) {
    cout << "\n=== PRIORITY SCHEDULING (NON-PREEMPTIVE) ===" << endl;
    
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    vector<bool> isCompleted(n, false);
    vector<pair<int, int>> gantt;
    vector<int> ganttTimes;
    ganttTimes.push_back(0);

    while (completed != n) {
        int idx = -1;
        int highestPriority = INT_MAX;

        // Find process with highest priority (lower number = higher priority)
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && !isCompleted[i] && 
                processes[i].priority < highestPriority) {
                highestPriority = processes[i].priority;
                idx = i;
            }
        }

        if (idx != -1) {
            if (currentTime < processes[idx].arrivalTime) {
                currentTime = processes[idx].arrivalTime;
            }

            processes[idx].startTime = currentTime;
            processes[idx].responseTime = processes[idx].startTime - processes[idx].arrivalTime;
            processes[idx].completionTime = currentTime + processes[idx].burstTime;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;

            gantt.push_back({processes[idx].id, processes[idx].burstTime});
            ganttTimes.push_back(processes[idx].completionTime);

            currentTime = processes[idx].completionTime;
            isCompleted[idx] = true;
            completed++;
        } else {
            currentTime++;
        }
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
        cout << "Enter Priority: ";
        cin >> processes[i].priority;
    }
    return processes;
}

int main() {
    cout << "=== PRIORITY NON-PREEMPTIVE CPU SCHEDULING ALGORITHM ===" << endl;
    
    vector<Process> processes = getProcessData();
    Priority_NonPreemptive(processes);

    cout << "\nPriority Non-Preemptive Scheduling completed!" << endl;
    return 0;
}