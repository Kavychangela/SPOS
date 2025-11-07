#include <iostream>
#include <vector>
#include <iomanip>
#include <climits>

using namespace std;

struct MemoryBlock {
    int id;
    int size;
    bool allocated;
    int processId;
    string processName;
};

struct Process {
    int id;
    string name;
    int size;
    bool allocated;
    int blockId;
};

void displayMemory(const vector<MemoryBlock>& memory) {
    cout << "\nMemory Blocks:" << endl;
    cout << string(60, '-') << endl;
    cout << setw(10) << "Block ID" << setw(15) << "Size" << setw(15) << "Status" 
         << setw(15) << "Process" << setw(15) << "Fragmentation" << endl;
    cout << string(60, '-') << endl;
    
    for(const auto& block : memory) {
        cout << setw(10) << block.id << setw(15) << block.size;
        if(block.allocated) {
            cout << setw(15) << "Allocated" << setw(15) << block.processName 
                 << setw(15) << (block.size - block.processId); // Simplified fragmentation
        } else {
            cout << setw(15) << "Free" << setw(15) << "-" << setw(15) << "-";
        }
        cout << endl;
    }
}

void displayProcesses(const vector<Process>& processes) {
    cout << "\nProcesses:" << endl;
    cout << string(50, '-') << endl;
    cout << setw(10) << "Process ID" << setw(15) << "Name" << setw(15) << "Size" 
         << setw(15) << "Status" << endl;
    cout << string(50, '-') << endl;
    
    for(const auto& process : processes) {
        cout << setw(10) << process.id << setw(15) << process.name 
             << setw(15) << process.size;
        if(process.allocated) {
            cout << setw(15) << "Allocated to Block " << process.blockId;
        } else {
            cout << setw(15) << "Waiting";
        }
        cout << endl;
    }
}

void bestFit(vector<MemoryBlock>& memory, vector<Process>& processes) {
    cout << "\n=== BEST FIT MEMORY ALLOCATION ===" << endl;
    
    for(auto& process : processes) {
        if(process.allocated) continue;
        
        int bestBlockIndex = -1;
        int minFragmentation = INT_MAX;
        
        // Find the best fit block
        for(int i = 0; i < memory.size(); i++) {
            if(!memory[i].allocated && memory[i].size >= process.size) {
                int fragmentation = memory[i].size - process.size;
                if(fragmentation < minFragmentation) {
                    minFragmentation = fragmentation;
                    bestBlockIndex = i;
                }
            }
        }
        
        if(bestBlockIndex != -1) {
            // Allocate the best fit block to the process
            memory[bestBlockIndex].allocated = true;
            memory[bestBlockIndex].processId = process.id;
            memory[bestBlockIndex].processName = process.name;
            
            process.allocated = true;
            process.blockId = memory[bestBlockIndex].id;
            
            cout << "Process " << process.name << " (" << process.size 
                 << "KB) allocated to Block " << memory[bestBlockIndex].id 
                 << " (" << memory[bestBlockIndex].size << "KB)" << endl;
            cout << "Internal Fragmentation: " << minFragmentation << "KB" << endl;
        } else {
            cout << "Process " << process.name << " (" << process.size 
                 << "KB) could not be allocated - No suitable block found!" << endl;
        }
    }
}

vector<MemoryBlock> getMemoryBlocks() {
    int n;
    cout << "Enter number of memory blocks: ";
    cin >> n;
    
    vector<MemoryBlock> memory(n);
    for(int i = 0; i < n; i++) {
        memory[i].id = i + 1;
        cout << "Enter size of Block " << (i + 1) << " (KB): ";
        cin >> memory[i].size;
        memory[i].allocated = false;
        memory[i].processId = -1;
        memory[i].processName = "";
    }
    return memory;
}

vector<Process> getProcesses() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;
    
    vector<Process> processes(n);
    for(int i = 0; i < n; i++) {
        processes[i].id = i;
        cout << "Enter name of Process " << (i + 1) << ": ";
        cin >> processes[i].name;
        cout << "Enter size of Process " << processes[i].name << " (KB): ";
        cin >> processes[i].size;
        processes[i].allocated = false;
        processes[i].blockId = -1;
    }
    return processes;
}

int main() {
    cout << "=== BEST FIT MEMORY PLACEMENT STRATEGY ===" << endl;
    
    vector<MemoryBlock> memory = getMemoryBlocks();
    vector<Process> processes = getProcesses();
    
    cout << "\nInitial State:" << endl;
    displayMemory(memory);
    displayProcesses(processes);
    
    bestFit(memory, processes);
    
    cout << "\nFinal State:" << endl;
    displayMemory(memory);
    displayProcesses(processes);
    
    // Calculate statistics
    int totalMemory = 0, allocatedMemory = 0, internalFragmentation = 0;
    int unallocatedProcesses = 0;
    
    for(const auto& block : memory) {
        totalMemory += block.size;
        if(block.allocated) {
            allocatedMemory += processes[block.processId].size;
            internalFragmentation += (block.size - processes[block.processId].size);
        }
    }
    
    for(const auto& process : processes) {
        if(!process.allocated) unallocatedProcesses++;
    }
    
    cout << "\n=== STATISTICS ===" << endl;
    cout << "Total Memory: " << totalMemory << "KB" << endl;
    cout << "Allocated Memory: " << allocatedMemory << "KB" << endl;
    cout << "Internal Fragmentation: " << internalFragmentation << "KB" << endl;
    cout << "Unallocated Processes: " << unallocatedProcesses << endl;
    cout << "Memory Utilization: " << (allocatedMemory * 100.0 / totalMemory) << "%" << endl;
    
    return 0;
}