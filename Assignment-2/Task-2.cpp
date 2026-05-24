#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct Paragraph {
    string content;
    Paragraph* prev;
    Paragraph* next;

    Paragraph(string s = "") : content(s), prev(nullptr), next(nullptr) {}
};

class Editor {

private:
    Paragraph* head;
    Paragraph* current;

    int column;
    int line;

    string fileName;
    bool dirty;

    string font;
    int fontSize;
    string color;

    bool modeInsert;
    vector<string> recentFiles;

    int countParagraphs() {
        int n = 0;
        Paragraph* p = head;
        while (p) {
            n++;
            p = p->next;
        }
        return n;
    }

    Paragraph* paragraphAt(int n) {
        Paragraph* p = head;
        int i = 1;
        while (p && i < n) {
            p = p->next;
            i++;
        }
        return p;
    }

    void clearAll() {
        Paragraph* p = head;

        while (p) {
            Paragraph* nx = p->next;
            delete p;
            p = nx;
        }

        head = new Paragraph("");
        current = head;

        column = 0;
        line = 1;
        dirty = false;
    }

    void addToHistory(const string& f) {
        for (const string& s : recentFiles)
            if (s == f) return;

        recentFiles.push_back(f);
    }

    string trim(const string& s) {
        int a = 0;
        int b = (int)s.size() - 1;

        while (a <= b && (s[a] == ' ' || s[a] == '\t')) a++;
        while (b >= a && (s[b] == ' ' || s[b] == '\t')) b--;

        return (a > b) ? "" : s.substr(a, b - a + 1);
    }

    void printSeparator() {
        cout << "+--------------------------------------------------+\n";
    }

    void showMenu() {
        printSeparator();
        cout << "|            TEXT EDITOR  v1.0                     |\n";
        printSeparator();
        cout << "| FILE:  new  open  save  saveas  close  recent    |\n";
        cout << "|        exit                                       |\n";
        printSeparator();
        cout << "| EDIT:  type   del   bs   insert                  |\n";
        cout << "|        find   replace   selall                   |\n";
        printSeparator();
        cout << "| NAV:   home  end  pgup  pgdn  goto               |\n";
        printSeparator();
        cout << "| FMT:   font  size  color  wrap                   |\n";
        printSeparator();
        cout << "|        show  menu                                 |\n";
        printSeparator();
        cout << "| USAGE:                                            |\n";
        cout << "|   type   -> then enter text (\\n = new paragraph) |\n";
        cout << "|   find   -> prompts for search word              |\n";
        cout << "|   replace-> prompts for find and replace word    |\n";
        cout << "|   goto N -> jump to paragraph N                  |\n";
        cout << "|   open   -> prompts for filename                 |\n";
        cout << "|   saveas -> prompts for new filename             |\n";
        printSeparator();
    }

    void showStatus() {
        cout << "[File: " << fileName << (dirty ? "*" : "")
            << "]  [Para: " << line << "/" << countParagraphs()
            << "]  [Col: " << column + 1
            << "]  [Font: " << font
            << " " << fontSize << "pt"
            << "]  [Color: " << color
            << "]  [Mode: " << (modeInsert ? "INSERT" : "OVERWRITE") << "]\n";
    }

    void cmdNew() {
        if (dirty)
            cout << "Warning: unsaved changes discarded.\n";

        clearAll();
        fileName = "untitled.txt";

        cout << "New file created.\n";
    }

    void cmdOpen() {
        string file;
        cout << "Filename: ";
        cin >> file;

        file = trim(file);

        if (file.empty()) {
            cout << "Invalid filename.\n";
            return;
        }

        if (dirty)
            cout << "Warning: unsaved changes discarded.\n";

        clearAll();

        fileName = file;
        current->content = "(Loaded: " + file + ")";
        column = current->content.size();

        dirty = false;

        addToHistory(file);

        cout << "Opened: " << file << "\n";
    }

    void cmdSave() {
        cout << "Saved: " << fileName << "\n";
        dirty = false;
    }

    void cmdSaveAs() {
        string file;
        cout << "New filename: ";
        cin >> file;

        file = trim(file);

        if (file.empty()) {
            cout << "Invalid filename.\n";
            return;
        }

        fileName = file;
        addToHistory(file);

        cmdSave();
    }

    void cmdClose() {
        if (dirty)
            cout << "Warning: unsaved changes discarded.\n";

        clearAll();
        fileName = "untitled.txt";

        cout << "File closed.\n";
    }

    void cmdRecent() {
        if (recentFiles.empty()) {
            cout << "No recent files.\n";
            return;
        }

        cout << "Recent files:\n";

        for (int i = 0; i < (int)recentFiles.size(); i++)
            cout << "  " << i + 1 << ". " << recentFiles[i] << "\n";
    }

    void cmdType() {
        cin.ignore();

        cout << "Enter text (\\n for new paragraph):\n> ";

        string raw;
        getline(cin, raw);

        for (int i = 0; i < (int)raw.size(); i++) {

            if (raw[i] == '\\' && i + 1 < (int)raw.size() && raw[i + 1] == 'n') {

                string rest = current->content.substr(column);

                current->content = current->content.substr(0, column);

                Paragraph* np = new Paragraph(rest);
                np->prev = current;
                np->next = current->next;

                if (current->next) current->next->prev = np;

                current->next = np;
                current = np;

                column = 0;
                line++;

                i++;
            }
            else {
                if (modeInsert)
                    current->content.insert(column, 1, raw[i]);
                else {
                    if (column < (int)current->content.size())
                        current->content[column] = raw[i];
                    else
                        current->content += raw[i];
                }

                column++;
            }
        }

        dirty = true;
        cout << "Text entered.\n";
    }

    void cmdDelete() {
        if (column < (int)current->content.size()) {
            current->content.erase(column, 1);
            dirty = true;
            cout << "Character deleted.\n";
        }
        else if (current->next) {
            current->content += current->next->content;

            Paragraph* dead = current->next;
            current->next = dead->next;

            if (dead->next)
                dead->next->prev = current;

            delete dead;

            dirty = true;
            cout << "Paragraphs merged.\n";
        }
        else {
            cout << "Nothing to delete.\n";
        }
    }

    void cmdBackspace() {
        if (column > 0) {
            current->content.erase(column - 1, 1);
            column--;

            dirty = true;
            cout << "Backspace done.\n";
        }
        else if (current->prev) {
            Paragraph* pr = current->prev;

            column = pr->content.size();
            pr->content += current->content;

            pr->next = current->next;

            if (current->next)
                current->next->prev = pr;

            delete current;

            current = pr;
            line--;

            dirty = true;
            cout << "Merged with previous paragraph.\n";
        }
        else {
            cout << "Already at beginning.\n";
        }
    }

    void cmdInsertToggle() {
        modeInsert = !modeInsert;

        cout << "Mode switched to: "
            << (modeInsert ? "INSERT" : "OVERWRITE") << "\n";
    }

public:
    Editor() {
        head = new Paragraph("");
        current = head;

        column = 0;
        line = 1;

        fileName = "untitled.txt";
        dirty = false;

        font = "Arial";
        fontSize = 12;
        color = "Black";

        modeInsert = true;
    }

    ~Editor() {
        Paragraph* p = head;

        while (p) {
            Paragraph* nx = p->next;
            delete p;
            p = nx;
        }
    }

    void run() {
        showMenu();

        string cmd;

        while (true) {
            cout << "\ncmd> ";

            if (!(cin >> cmd)) break;

            cmd = trim(cmd);

            if (cmd == "new") cmdNew();
            else if (cmd == "open") cmdOpen();
            else if (cmd == "save") cmdSave();
            else if (cmd == "saveas") cmdSaveAs();
            else if (cmd == "close") cmdClose();
            else if (cmd == "recent") cmdRecent();
            else if (cmd == "exit") break;
            else if (cmd == "type") cmdType();
            else if (cmd == "del") cmdDelete();
            else if (cmd == "bs") cmdBackspace();
            else if (cmd == "insert") cmdInsertToggle();
            else cout << "Unknown command. Type 'menu' for help.\n";
        }

        cout << "Editor closed.\n";
    }
};

int main() {
    Editor e;
    e.run();
    return 0;
}