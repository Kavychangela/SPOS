#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>

using namespace std;

/* ---------- Config ---------- */
const int MAX_SYM = 100;
const int MAX_LINE = 128;

/* ---------- Symbol Table ---------- */
struct Symbol {
    string name;
    int addr;
    Symbol(const string& n, int a) : name(n), addr(a) {}
};

vector<Symbol> sym;
int LC = 0;            /* location counter */
int START_ADDR = 0;

/* ---------- MOT & REG ---------- */
struct MOT {
    string op;
    int code;
    int needReg;
    int needMem;

    MOT(const string& o, int c, int nr, int nm) : op(o), code(c), needReg(nr), needMem(nm) {}
};

vector<MOT> mot = {
    {"MOVER", 1, 1, 1}, {"MOVEM", 2, 1, 1}, {"ADD", 3, 1, 1},
    {"SUB",   4, 1, 1}, {"COMP", 5, 1, 1}, {"READ",6, 0, 1},
    {"PRINT", 7, 0, 1}, {"STOP", 0, 0, 0}
};

struct REG {
    string r;
    int code;

    REG(const string& reg, int c) : r(reg), code(c) {}
};

vector<REG> regtab = {
    {"A",1}, {"B",2}, {"C",3}, {"D",4}
};

/* ---------- Helpers ---------- */
bool isblankline(const string& s) {
    for(char c : s) {
        if(!isspace(static_cast<unsigned char>(c)))
            return false;
    }
    return true;
}

int regcode(const string& r) {
    for(const auto& reg : regtab) {
        if(reg.r == r)
            return reg.code;
    }
    return 0;
}

int find_mot(const string& op) {
    for(size_t i = 0; i < mot.size(); i++) {
        if(mot[i].op == op)
            return static_cast<int>(i);
    }
    return -1;
}

int sym_lookup(const string& name) {
    for(const auto& symbol : sym) {
        if(symbol.name == name)
            return symbol.addr;
    }
    return -1;
}

void sym_insert(const string& name, int addr) {
    if(sym_lookup(name) != -1) return; /* ignore duplicate silently */
    if(sym.size() < MAX_SYM) {
        sym.emplace_back(name, addr);
    }
}

// Trim whitespace from both ends of string
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    if(start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

/* Split at first comma (for R,OPR) */
void split_reg_mem(const string& s, string& r, string& m) {
    size_t comma_pos = s.find(',');
    if(comma_pos != string::npos) {
        r = trim(s.substr(0, comma_pos));
        m = trim(s.substr(comma_pos + 1));
    } else {
        r = "";
        m = trim(s);
    }
}

/* ---------- PASS 1 ---------- */
void pass1(const string& fname) {
    ifstream fp(fname);
    ofstream out("inter.txt");
    ofstream stab("symtab.txt");

    if(!fp.is_open() || !out.is_open() || !stab.is_open()) {
        cout << "File error!" << endl;
        exit(0);
    }

    string line;
    LC = 0;
    sym.clear();

    cout << "\n--- PASS 1 ---" << endl;

    while(getline(fp, line)) {
        if(isblankline(line)) continue;

        string t1, t2, t3;
        istringstream iss(line);
        iss >> t1 >> t2;
        getline(iss, t3);
        t3 = trim(t3);

        if(t1.empty()) continue;

        if(t1 == "START") {
            START_ADDR = LC = atoi(t2.c_str());
            out << "(AD,01)(C," << LC << ")" << endl;
            cout << "(AD,01)(C," << LC << ")" << endl;
            continue;
        }
        if(t1 == "END") {
            out << "(AD,02)" << endl;
            cout << "(AD,02)" << endl;
            break;
        }

        /* DS/DC with label in t1 */
        if(t2 == "DS") {
            sym_insert(t1, LC);
            out << "(DL,02)(C," << t3 << ")" << endl;
            cout << "(DL,02)(C," << t3 << ")" << endl;
            LC += atoi(t3.c_str());
            continue;
        }
        if(t2 == "DC") {
            sym_insert(t1, LC);
            out << "(DL,01)(C," << t3 << ")" << endl;
            cout << "(DL,01)(C," << t3 << ")" << endl;
            LC += 1;
            continue;
        }

        /* Otherwise: Instruction line (label optional) */
        string op, opr;
        istringstream line_stream(line);
        vector<string> tokens;
        string token;

        while(line_stream >> token) {
            tokens.push_back(token);
        }

        if(tokens.size() == 1) {
            op = tokens[0];
            opr = "";
        } else if(tokens.size() == 2) {
            op = tokens[0];
            opr = tokens[1];
        } else if(tokens.size() >= 3) {
            op = tokens[1];
            opr = tokens[2];
            for(size_t i = 3; i < tokens.size(); i++) {
                opr += " " + tokens[i];
            }
        }

        int mi = find_mot(op);
        if(mi == -1) {
            out << "(IS)(S)" << endl;
            cout << "(IS)(S)" << endl;
            LC += 1;
            continue;
        }

        /* Record a compact IS line */
        out << "(IS,";
        if(mot[mi].code < 10) out << "0";
        out << mot[mi].code << ")" << endl;
        cout << "(IS,";
        if(mot[mi].code < 10) cout << "0";
        cout << mot[mi].code << ")" << endl;
        LC += 1;
    }

    /* dump symbol table */
    for(const auto& symbol : sym) {
        stab << symbol.name << " " << symbol.addr << endl;
    }

    fp.close();
    out.close();
    stab.close();

    cout << "\nSymbol Table:" << endl;
    for(const auto& symbol : sym) {
        cout << symbol.name << "\t" << symbol.addr << endl;
    }
    cout << "\nPASS 1 completed. Generated inter.txt and symtab.txt" << endl;
}

/* ---------- PASS 2 ---------- */
void pass2(const string& fname) {
    /* Load SYMTAB */
    ifstream stab("symtab.txt");
    if(!stab.is_open()) {
        cout << "symtab.txt not found. Run Pass 1 first." << endl;
        return;
    }

    sym.clear();
    string name;
    int addr;
    while(stab >> name >> addr) {
        sym.emplace_back(name, addr);
    }
    stab.close();

    ifstream src(fname);
    ofstream obj("object.txt");

    if(!src.is_open() || !obj.is_open()) {
        cout << "File error in Pass 2!" << endl;
        return;
    }

    string line;
    LC = START_ADDR;

    cout << "\n--- PASS 2 ---" << endl;

    while(getline(src, line)) {
        if(isblankline(line)) continue;

        string t1, t2, t3;
        istringstream iss(line);
        iss >> t1 >> t2;
        getline(iss, t3);
        t3 = trim(t3);

        if(t1.empty()) continue;

        if(t1 == "START") { continue; }
        if(t1 == "END") { break; }

        /* DS/DC with label in t1 */
        if(t2 == "DS") {
            int k = atoi(t3.c_str());
            obj << "; " << LC << " DS " << t3 << endl;
            cout << "; " << LC << " DS " << t3 << endl;
            LC += k;
            continue;
        }
        if(t2 == "DC") {
            int val = atoi(t3.c_str());
            obj << LC << " 00 0 " << val << endl;
            cout << LC << " 00 0 " << val << endl;
            LC += 1;
            continue;
        }

        /* Instruction */
        string op, opr;
        istringstream line_stream(line);
        vector<string> tokens;
        string token;

        while(line_stream >> token) {
            tokens.push_back(token);
        }

        if(tokens.size() == 1) {
            op = tokens[0];
            opr = "";
        } else if(tokens.size() == 2) {
            op = tokens[0];
            opr = tokens[1];
        } else if(tokens.size() >= 3) {
            op = tokens[1];
            opr = tokens[2];
            for(size_t i = 3; i < tokens.size(); i++) {
                opr += " " + tokens[i];
            }
        }

        int mi = find_mot(op);
        if(mi == -1) {
            obj << "; " << LC << " UNKNOWN " << op << endl;
            cout << "; " << LC << " UNKNOWN " << op << endl;
            LC += 1;
            continue;
        }

        int rcode = 0, maddr = 0;
        if(mot[mi].needReg || mot[mi].needMem) {
            string r, m;
            split_reg_mem(opr, r, m);
            if(mot[mi].needReg) rcode = regcode(r);
            if(mot[mi].needMem) {
                if(m.empty()) {
                    maddr = 0;
                } else {
                    int a = sym_lookup(m);
                    if(a == -1) {
                        try {
                            a = atoi(m.c_str()); /* allow numeric literal fallback */
                        } catch(const exception& e) {
                            a = 0;
                        }
                    }
                    maddr = a;
                }
            }
        }

        obj << LC << " ";
        if(mot[mi].code < 10) obj << "0";
        obj << mot[mi].code << " " << rcode << " " << maddr << endl;

        cout << LC << " ";
        if(mot[mi].code < 10) cout << "0";
        cout << mot[mi].code << " " << rcode << " " << maddr << endl;

        LC += 1;
    }

    src.close();
    obj.close();
    cout << "\nPASS 2 completed. Generated object.txt" << endl;
}

/* ---------- Viewer ---------- */
void show(const string& f) {
    ifstream p(f);
    if(!p.is_open()) return;

    string line;
    while(getline(p, line)) {
        cout << line << endl;
    }
    p.close();
}

/* ---------- main ---------- */
int main() {
    string fname;
    cout << "Enter source file name: ";
    cin >> fname;

    cout << "\nSource Code:" << endl;
    show(fname);

    pass1(fname);
    pass2(fname);

    cout << "\n--- inter.txt ---" << endl;
    show("inter.txt");
    cout << "\n--- symtab.txt ---" << endl;
    show("symtab.txt");
    cout << "\n--- object.txt ---" << endl;
    show("object.txt");

    return 0;
}
