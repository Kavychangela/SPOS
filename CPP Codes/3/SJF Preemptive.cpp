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
    int remainingTime;
    int startTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
};

// Function to display the results table
void displayResults(vector<Process>& processes) {
    cout << "\n=== SJF PREEMPTIVE SCHEDULING RESULTS ===" << endl;
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

// SJF Preemptive Scheduling Algorithm
void SJF_Preemptive(vector<Process> processes) {
    cout << "\n=== SHORTEST JOB FIRST (PREEMPTIVE) SCHEDULING ===" << endl;

    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    vector<bool> isCompleted(n, false);
    vector<pair<int, int>> gantt;
    vector<int> ganttTimes;
    ganttTimes.push_back(0);

    // Initialize remaining time
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
        p.startTime = -1;
    }

    while (completed != n) {
        int idx = -1;
        int minBurst = INT_MAX;

        // Find process with minimum remaining time that has arrived
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && !isCompleted[i] &&
                processes[i].remainingTime > 0 && processes[i].remainingTime < minBurst) {
                minBurst = processes[i].remainingTime;
                idx = i;
            }
        }

        if (idx != -1) {
            if (processes[idx].startTime == -1) {
                processes[idx].startTime = currentTime;
                processes[idx].responseTime = processes[idx].startTime - processes[idx].arrivalTime;
            }

            // Execute for 1 unit of time
            processes[idx].remainingTime--;
            currentTime++;

            // Add to Gantt chart
            if (!gantt.empty() && gantt.back().first == processes[idx].id) {
                gantt.back().second++;
            } else {
                gantt.push_back({processes[idx].id, 1});
                ganttTimes.push_back(currentTime);
            }

            if (processes[idx].remainingTime == 0) {
                processes[idx].completionTime = currentTime;
                processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
                isCompleted[idx] = true;
                completed++;
            }
        } else {
            currentTime++;
            gantt.push_back({-1, 1});
            ganttTimes.push_back(currentTime);
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
        processes[i].remainingTime = processes[i].burstTime;
    }
    return processes;
}

int main() {
    cout << "=== SJF PREEMPTIVE CPU SCHEDULING ALGORITHM ===" << endl;

    vector<Process> processes = getProcessData();
    SJF_Preemptive(processes);

    cout << "\nSJF Preemptive Scheduling completed!" << endl;
    return 0;
}
