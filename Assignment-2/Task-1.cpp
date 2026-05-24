#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

struct Window {
    string title;
    string color;
    int width;
    int height;
    Window* prev;
    Window* next;

    Window(const string& t, const string& c, int w, int h)
        : title(t), color(c), width(w), height(h), prev(nullptr), next(nullptr) {
    }
};

class WindowManager {
    Window* head;
    Window* tail;
    int count;

    static const int DEFAULT_WIDTH = 800;
    static const int DEFAULT_HEIGHT = 600;
    static const char* DEFAULT_COLOR;

public:
    WindowManager() : head(nullptr), tail(nullptr), count(0) {}

    ~WindowManager() {
        Window* cur = head;
        while (cur) {
            Window* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
    }

    void createWindow(const string& title, const string& color = "") {
        string c = color.empty() ? DEFAULT_COLOR : color;
        Window* w = new Window(title, c, DEFAULT_WIDTH, DEFAULT_HEIGHT);
        if (!head) {
            head = tail = w;
        }
        else {
            tail->next = w;
            w->prev = tail;
            tail = w;
        }
        ++count;
        cout << "Created window \"" << title << "\" (" << c
            << ", " << DEFAULT_WIDTH << "x" << DEFAULT_HEIGHT << ")\n";
    }

    void activateWindow(const string& title) {
        Window* w = find(title);
        if (!w) { cout << "Window \"" << title << "\" not found.\n"; return; }
        if (w == head) { cout << "Window \"" << title << "\" is already active.\n"; return; }

        if (w->prev) w->prev->next = w->next;
        if (w->next) w->next->prev = w->prev;
        if (w == tail) tail = w->prev;

        w->prev = nullptr;
        w->next = head;
        if (head) head->prev = w;
        head = w;

        cout << "Activated window \"" << title << "\"\n";
    }

    void resizeWindow(const string& title, int w, int h) {
        if (w <= 0 || h <= 0) {
            cout << "Invalid dimensions. Width and height must be positive.\n";
            return;
        }
        Window* win = find(title);
        if (!win) { cout << "Window \"" << title << "\" not found.\n"; return; }
        win->width = w;
        win->height = h;
        cout << "Resized window \"" << title << "\" to " << w << "x" << h << "\n";
    }

    void deleteWindow(const string& title) {
        Window* w = find(title);
        if (!w) { cout << "Window \"" << title << "\" not found.\n"; return; }

        if (w->prev) w->prev->next = w->next;
        if (w->next) w->next->prev = w->prev;
        if (w == head) head = w->next;
        if (w == tail) tail = w->prev;

        cout << "Deleted window \"" << title << "\"\n";
        delete w;
        --count;
    }

    void listWindows() const {
        if (!head) { cout << "No windows open.\n"; return; }
        cout << "Windows (front = active):\n";
        Window* cur = head;
        int idx = 1;
        while (cur) {
            cout << "  " << idx++ << ". [" << cur->title << "] "
                << cur->color << " " << cur->width << "x" << cur->height;
            if (cur == head) cout << "  <-- ACTIVE";
            cout << "\n";
            cur = cur->next;
        }
    }

private:
    Window* find(const string& title) const {
        Window* cur = head;
        while (cur) {
            if (cur->title == title) return cur;
            cur = cur->next;
        }
        return nullptr;
    }
};

const char* WindowManager::DEFAULT_COLOR = "white";

void printMenu() {
    cout << "\n--- Window Manager ---\n"
        << "1. Create window\n"
        << "2. Activate window\n"
        << "3. Resize window\n"
        << "4. Delete window\n"
        << "5. List windows\n"
        << "0. Exit\n"
        << "Choice: ";
}

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == string::npos) ? "" : s.substr(a, b - a + 1);
}

int readInt(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        }
        catch (...) {}
        cout << "Please enter a valid integer.\n";
    }
}

int main() {
    WindowManager wm;
    string line;

    while (true) {
        printMenu();
        if (!getline(cin, line)) break;

        size_t a = line.find_first_not_of(" \t\r\n");
        if (a == string::npos) continue;
        line = line.substr(a, line.find_last_not_of(" \t\r\n") - a + 1);

        if (line == "0") {
            cout << "Exiting.\n";
            break;
        }
        else if (line == "1") {
            string title = readLine("Title: ");
            if (title.empty()) { cout << "Title cannot be empty.\n"; continue; }
            string color = readLine("Color (leave blank for default): ");
            wm.createWindow(title, color);
        }
        else if (line == "2") {
            string title = readLine("Title to activate: ");
            wm.activateWindow(title);
        }
        else if (line == "3") {
            string title = readLine("Title to resize: ");
            int w = readInt("New width: ");
            int h = readInt("New height: ");
            wm.resizeWindow(title, w, h);
        }
        else if (line == "4") {
            string title = readLine("Title to delete: ");
            wm.deleteWindow(title);
        }
        else if (line == "5") {
            wm.listWindows();
        }
        else {
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}