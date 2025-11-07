#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <queue>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int priority;
    int remainingTime;
    int startTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
};

// Function to display the results table
void displayResults(vector<Process>& processes) {
    cout << "\n=== ROUND ROBIN SCHEDULING RESULTS ===" << endl;
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

// Round Robin Preemptive Scheduling Algorithm
void RoundRobin(vector<Process> processes, int timeQuantum) {
    cout << "\n=== ROUND ROBIN (PREEMPTIVE) SCHEDULING - Time Quantum: " << timeQuantum << " ===" << endl;
    
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    vector<bool> isCompleted(n, false);
    vector<int> remainingTime(n);
    vector<pair<int, int>> gantt;
    vector<int> ganttTimes;
    ganttTimes.push_back(0);

    // Initialize remaining time and start times
    for (int i = 0; i < n; i++) {
        remainingTime[i] = processes[i].burstTime;
        processes[i].startTime = -1;
    }

    queue<int> readyQueue;
    vector<bool> inQueue(n, false);

    // Add processes that arrive at time 0
    for (int i = 0; i < n; i++) {
        if (processes[i].arrivalTime == 0) {
            readyQueue.push(i);
            inQueue[i] = true;
        }
    }

    while (completed != n) {
        if (!readyQueue.empty()) {
            int idx = readyQueue.front();
            readyQueue.pop();
            inQueue[idx] = false;

            if (processes[idx].startTime == -1) {
                processes[idx].startTime = currentTime;
                processes[idx].responseTime = processes[idx].startTime - processes[idx].arrivalTime;
            }

            int executionTime = min(timeQuantum, remainingTime[idx]);
            remainingTime[idx] -= executionTime;

            // Add to Gantt chart
            gantt.push_back({processes[idx].id, executionTime});
            currentTime += executionTime;
            ganttTimes.push_back(currentTime);

            // Add newly arrived processes to ready queue
            for (int i = 0; i < n; i++) {
                if (!isCompleted[i] && !inQueue[i] && processes[i].arrivalTime <= currentTime && remainingTime[i] > 0) {
                    readyQueue.push(i);
                    inQueue[i] = true;
                }
            }

            if (remainingTime[idx] > 0) {
                readyQueue.push(idx);
                inQueue[idx] = true;
            } else {
                processes[idx].completionTime = currentTime;
                processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
                isCompleted[idx] = true;
                completed++;
            }
        } else {
            currentTime++;
            // Check for new arrivals
            for (int i = 0; i < n; i++) {
                if (!isCompleted[i] && !inQueue[i] && processes[i].arrivalTime <= currentTime && remainingTime[i] > 0) {
                    readyQueue.push(i);
                    inQueue[i] = true;
                    break;
                }
            }
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
        processes[i].remainingTime = processes[i].burstTime;
    }
    return processes;
}

int main() {
    cout << "=== ROUND ROBIN CPU SCHEDULING ALGORITHM ===" << endl;
    
    int timeQuantum;
    cout << "Enter Time Quantum: ";
    cin >> timeQuantum;
    
    vector<Process> processes = getProcessData();
    RoundRobin(processes, timeQuantum);

    cout << "\nRound Robin Scheduling completed!" << endl;
    return 0;
}