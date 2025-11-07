#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// Data structures for MNT and MDT
struct MacroEntry {
    string name;
    vector<string> parameters;
    int startIndex;
    int endIndex;
};

vector<MacroEntry> MNT;
vector<string> MDT;

// Function to trim whitespace from a string
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to split a string into tokens
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

// Function to display MNT
void displayMNT() {
    cout << "\n=== MACRO NAME TABLE (MNT) ===" << endl;
    cout << setw(15) << "Macro Name" << setw(20) << "Parameters"
         << setw(10) << "Start" << setw(10) << "End" << endl;
    cout << string(55, '-') << endl;

    for (const auto& entry : MNT) {
        cout << setw(15) << entry.name;

        string params;
        for (size_t i = 0; i < entry.parameters.size(); ++i) {
            if (i > 0) params += ", ";
            params += entry.parameters[i];
        }
        cout << setw(20) << params;
        cout << setw(10) << entry.startIndex << setw(10) << entry.endIndex << endl;
    }
}

// Function to display MDT
void displayMDT() {
    cout << "\n=== MACRO DEFINITION TABLE (MDT) ===" << endl;
    cout << setw(10) << "Index" << setw(30) << "Content" << endl;
    cout << string(40, '-') << endl;

    for (size_t i = 0; i < MDT.size(); ++i) {
        cout << setw(10) << i << setw(30) << MDT[i] << endl;
    }
}

// Function to display file content
void displayFile(const string& filename, const string& title) {
    ifstream file(filename);
    cout << "\n=== " << title << " ===" << endl;
    cout << "File: " << filename << endl;
    cout << string(50, '-') << endl;

    string line;
    int lineNo = 1;
    while (getline(file, line)) {
        cout << setw(3) << lineNo++ << ": " << line << endl;
    }
    file.close();
}

// Pass-I: Process macro definitions and generate MNT, MDT, and intermediate code
void passI(const string& inputFile, const string& intermediateFile) {
    ifstream inFile(inputFile);
    ofstream outFile(intermediateFile);
    string line;
    bool insideMacro = false;
    MacroEntry currentMacro;

    cout << "Processing Pass-I..." << endl;

    while (getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line == "MACRO") {
            insideMacro = true;
            getline(inFile, line);
            line = trim(line);
            vector<string> tokens = split(line, ' ');
            currentMacro.name = tokens[0];
            currentMacro.parameters.assign(tokens.begin() + 1, tokens.end());
            currentMacro.startIndex = MDT.size();
            MNT.push_back(currentMacro);
            MDT.push_back(line);
            cout << "Found macro definition: " << currentMacro.name << endl;
        } else if (line == "MEND") {
            MDT.push_back(line);
            MNT.back().endIndex = MDT.size() - 1;
            insideMacro = false;
            cout << "End of macro: " << MNT.back().name << endl;
        } else if (insideMacro) {
            MDT.push_back(line);
        } else {
            outFile << line << endl;
        }
    }
    inFile.close();
    outFile.close();

    cout << "Pass-I completed successfully!" << endl;
}

// Pass-II: Expand macro calls using MNT and MDT
void passII(const string& intermediateFile, const string& outputFile) {
    ifstream inFile(intermediateFile);
    ofstream outFile(outputFile);
    string line;

    cout << "\nProcessing Pass-II..." << endl;

    while (getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;

        vector<string> tokens = split(line, ' ');
        string firstToken = tokens[0];

        // Check if the line is a macro call
        auto it = find_if(MNT.begin(), MNT.end(), [&](const MacroEntry& entry) {
            return entry.name == firstToken;
        });

        if (it != MNT.end()) {
            // Macro call found
            vector<string> arguments(tokens.begin() + 1, tokens.end());
            cout << "Expanding macro: " << it->name << " with arguments: ";
            for (const auto& arg : arguments) cout << arg << " ";
            cout << endl;

            for (int i = it->startIndex + 1; i < it->endIndex; ++i) {
                string macroLine = MDT[i];
                // Replace parameters with arguments
                for (size_t j = 0; j < it->parameters.size(); ++j) {
                    string param = it->parameters[j];
                    string arg = j < arguments.size() ? arguments[j] : "";
                    size_t pos = macroLine.find(param);
                    while (pos != string::npos) {
                        macroLine.replace(pos, param.length(), arg);
                        pos = macroLine.find(param, pos + arg.length());
                    }
                }
                outFile << macroLine << endl;
            }
        } else {
            outFile << line << endl;
        }
    }
    inFile.close();
    outFile.close();

    cout << "Pass-II completed successfully!" << endl;
}

int main() {
    string inputFile = "input.asm";
    string intermediateFile = "intermediate.asm";
    string outputFile = "output.asm";

    // Create sample input file
    ofstream createInput(inputFile);
    createInput << "START 100" << endl;
    createInput << "MACRO" << endl;
    createInput << "INCR &A,&B,&C" << endl;
    createInput << "MOVER A,&A" << endl;
    createInput << "ADD A,&B" << endl;
    createInput << "MOVEM A,&C" << endl;
    createInput << "MEND" << endl;
    createInput << "MACRO" << endl;
    createInput << "DECR &X,&Y" << endl;
    createInput << "MOVER B,&X" << endl;
    createInput << "SUB B,&Y" << endl;
    createInput << "MOVEM B,&X" << endl;
    createInput << "MEND" << endl;
    createInput << "INCR DATA1,DATA2,RESULT1" << endl;
    createInput << "DECR VAL1,VAL2" << endl;
    createInput << "STOP" << endl;
    createInput << "DATA1 DC 5" << endl;
    createInput << "DATA2 DC 10" << endl;
    createInput << "RESULT1 DS 1" << endl;
    createInput << "VAL1 DC 20" << endl;
    createInput << "VAL2 DC 8" << endl;
    createInput << "END" << endl;
    createInput.close();

    cout << "=== TWO-PASS MACRO PROCESSOR ===" << endl;

    // Display input file
    displayFile(inputFile, "INPUT SOURCE CODE");

    // Perform Pass-I
    passI(inputFile, intermediateFile);

    // Display Pass-I results
    displayMNT();
    displayMDT();
    displayFile(intermediateFile, "INTERMEDIATE CODE (After Pass-I)");

    // Perform Pass-II
    passII(intermediateFile, outputFile);

    // Display final output
    displayFile(outputFile, "FINAL EXPANDED CODE (After Pass-II)");

    cout << "\nMacro processing complete!" << endl;
    cout << "Output saved in: " << outputFile << endl;

    return 0;
}
