#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>

using namespace std;

class File {
    public: 
        string name;
        bool isFile;
        map<string, File*> children;
        string content;
        File* parent;
        File(string n, bool f, File* p = nullptr) : name(n), isFile(f), parent(p) {}
};
class directory {
    private:
        File* root;// Root directory
        File* cwd;// Current working directory

        vector<string> splitPath(const string& path) {
            vector<string> parts;
            stringstream ss(path);
            string part;
            while (getline(ss, part, '/')) {
                if (!part.empty()) parts.push_back(part);
            }
            return parts;
        }

        File* navigate(const string& path, bool mustExist = true) {
            // 如果路径以斜杠开始，表示从根目录开始，否则从当前工作目录开始
            File* current = (path[0] == '/') ? root : cwd;  
            vector<string> parts = splitPath(path);

            for (const string& part : parts) {
                if (part == ".") continue;  // 忽略当前目录
                if (part == "..") {         // 返回父目录
                    if (current->parent) current = current->parent;
                } else {
                    if (current->children.find(part) != current->children.end()) {
                        current = current->children[part];  // 进入子目录
                    } else if (!mustExist) {
                        // 如果目录不存在且不要求一定存在，创建新目录
                        File* newDir = new File(part, false, current);
                        current->children[part] = newDir;
                        current = newDir;
                    } else {
                        return nullptr;  // 如果必须存在且找不到目录，返回 nullptr
                    }
                }
            }
            return current;
        }
        void findFiles(File* file, const string& pattern, const string& relativePath, vector<string>& results) {
            if (regex_match(file->name, regex(pattern))) {
                results.push_back(relativePath + "/" + file->name);
            }
            if (!file->isFile) {
                for (auto& [name, child] : file->children) {
                    findFiles(child, pattern, relativePath + "/" + file->name, results);
                }
            }
        }
    public:
    directory() {
        root = new File("/", false, nullptr);// 创建根目录
        cwd = root;
    }
    void mkdir(const string& path) {
        vector<string> parts = splitPath(path);  // 获取路径的各个部分
        File* current = cwd;  // 从当前工作目录开始

        // 遍历路径中的每一部分（目录）
        for (const string& part : parts) {
            // 如果当前目录下没有这个子目录，创建它
            if (current->children.find(part) == current->children.end()) {
                File* newDir = new File(part, false, current);  // 创建新目录
                current->children[part] = newDir;  // 将新目录加入当前目录的子目录中
            }
            current = current->children[part];  // 移动到下一级目录
        }
    }
    void pwd() {
        File* current = cwd;
        string path = "";
        while (current && current->parent) {  // 跳过根目录的斜杠
            path = "/" + current->name + path;
            current = current->parent;
        }
        if (path.empty()) {
            path = "/";
        }
        cout << path << endl;
    }

    void cat(const string& path) {
        File* file = navigate(path);
        if (file && file->isFile) {
            cout << file->content << endl;
        } else {
            cout << "error" << endl;
        }
    }

    void cd(const string& path) {
        File* dir = navigate(path);
        if (dir && !dir->isFile) {
            cwd = dir;// 如果导航成功且目标是目录，将当前工作目录切换到目标目录
        }else {
            cout << "error" << endl;  // 如果导航失败或目标不是目录，则输出错误
        }
    }

    void rm(const vector<string>& args) {
        if (args.size() == 1) {
            File* file = navigate(args[0]);
            if (file && file->isFile) {
                file->parent->children.erase(file->name);
                delete file;
            } else {
                cout << "error" << endl;
            }
        } else if (args.size() == 2 && args[0] == "-r") {
            File* dir = navigate(args[1]);
            if (dir && !dir->isFile) {
                dir->parent->children.erase(dir->name);
                delete dir;
            } else {
                cout << "error" << endl;
            }
        } else {
            cout << "error" << endl;
        }
    }



};

int main() {
    directory fs;
    int n;
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        string command;
        getline(cin, command);
        stringstream ss(command);
        string cmd;
        ss >> cmd;

        if (cmd == "mkdir") {
            string option, path;
            ss >> option >> path;
            if (option == "-p") {
                fs.mkdir(path);
            } else {
                cout << "error" << endl;
            }
        }else if (cmd == "cd") {
            string path;
            ss >> path;
            fs.cd(path);
        }
        else if (cmd == "pwd") {
            fs.pwd();
        }
    }

    return 0;
}
