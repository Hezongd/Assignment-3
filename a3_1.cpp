#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>
#include <sys/stat.h>

using namespace std;

class File {
    public: 
        string name;
        bool isFile;
        map<string, File*> children;
        string content;
        File* parent;
        File(string n, bool f, File* p = nullptr) : name(n), isFile(f), parent(p) {};
                // Method to append content to a file}
        void appendContent(const string& text) {
                content += text;
        }

            // Method to overwrite content of the file
        void overwriteContent(const string& text) {
            content = text;
        }
        void removeContent() {
            content.clear();
        }

        // Method to recursively remove all children (for directories)
        void removeChildren() {
            for (auto& [name, child] : children) {
                delete child;
            }
            children.clear();
        }
        File* getFile(string name) {
            if (children.find(name) != children.end()) {
                return children[name];
            }
            return nullptr;
        }
        void addFile(File *node) {
            if (!isFile) { // Only directories can have children
                children[node->name] = node;
            } else {
                cout << "error\n";
        }
    }
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
                if (!part.empty() && part != ".") { // 忽略空段和 "."
                    parts.push_back(part);
                }
            }
            return parts;
        }



        File* navigate(const string& path, bool mustExist = true) {
            // 从根目录或当前目录开始
            File* current = (path.empty() || path[0] != '/') ? cwd : root;  
            vector<string> parts = splitPath(path);

            for (const string& part : parts) {
                if (part == ".") {
                    continue;  // 忽略当前目录标志
                }
                else if (part == "..") {
                    if (current->parent) {
                        current = current->parent;  // 返回父目录
                    }
                } else {
                    if (current->children.find(part) != current->children.end()) {
                        current = current->children[part];  // 进入子目录
                    } else if (!mustExist) {
                        // 如果目录不存在且不要求必须存在，创建目录
                        File* newDir = new File(part, false, current);
                        current->children[part] = newDir;
                        current = newDir;
                    } else {
                        return nullptr;  // 找不到目录
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
        void createFile(const string& path, const string& content, bool append = false) {
            vector<string> parts = splitPath(path);
            File* current = cwd;

            for (size_t i = 0; i < parts.size() - 1; ++i) {
                string part = parts[i];
                if (current->children.find(part) == current->children.end()) {
                    File* newDir = new File(part, false, current);
                    current->children[part] = newDir;
                }
                current = current->children[part];
            }

            string fileName = parts.back();
            File* file = nullptr;

            if (current->children.find(fileName) == current->children.end()) {
                file = new File(fileName, true, current);
                current->children[fileName] = file;
            } else {
                file = current->children[fileName];
            }

            if (append) {
                file->content += content + "\n";
            } else {
                file->content = content + "\n";
            }
        }

    public:
    directory() {
        root = new File("/", false, nullptr);// 创建根目录
        cwd = root;
    }
    void mkdir(const string& path) {
        vector<string> parts = splitPath(path);  // 获取路径的各个部分
        File* current = path.empty() || path[0] != '/' ? cwd : root;  // 从当前工作目录开始

        // 遍历路径中的每一部分（目录）
        for (const string& part : parts) {
            // 如果当前目录下没有这个子目录且路径为-p，创建它
            if (current->children.find(part) == current->children.end()) {
                File* newDir = new File(part, false, current);  // 创建新目录
                current->children[part] = newDir;  // 将新目录加入当前目录的子目录中
            }
            current = current->children[part];  // 移动到下一级目录
        }
    }
    void ls(const string& path) {
        File* dir = navigate(path);
        if (!dir || dir->isFile) {
            cout << "error" << endl;
            return;
        }
        vector<string> entries = {".", ".."};
        for (const auto& [name, _] : dir->children) {
            entries.push_back(name);
        }
        sort(entries.begin(), entries.end());
        for (size_t i = 0; i < entries.size(); ++i) {
            cout << entries[i] << (i + 1 < entries.size() ? "\t" : "\n");
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
            cout << file->content;
        } else {
            cout << "error" << endl;
        }
    }
   void echo(const vector<string>& args) {
        auto removeQuotes = [](const string& str) -> string {
            if ((str.front() == '\'' && str.back() == '\'') || 
                (str.front() == '"' && str.back() == '"')) {
                return str.substr(1, str.size() - 2); // 去掉首尾引号
            }
            return str;
        };

        if (args.size() == 1) {
            // 情况 1: 仅回显内容到控制台
            cout << removeQuotes(args[0]) << endl;
            return;
        }

        if (args.size() == 3) {
            string text = removeQuotes(args[0]); // 去掉引号的内容
            string redirect = args[1];          // 重定向操作符
            string path = args[2];              // 文件路径

            if (redirect != ">" && redirect != ">>") {
                cout << "error1" << endl;        // 非法操作符
                return;
            }
            // 修正路径为空或非法的情况
            if (path.empty()) {
                cout << "error2" << endl;       // 空路径报错
                return;
            }
            vector<string> parts = splitPath(path);  // 获取路径的各个部分
            File* current = path.empty() || path[0] != '/' ? cwd : root;  // 从当前工作目录开始


            for (size_t i = 0; i < parts.size(); ++i) {
                const string& part = parts[i];
                bool isLast = (i == parts.size() - 1); // 判断是否是最后一次迭代

                // 如果当前节点不存在，创建它
                if (current->children.find(part) == current->children.end()) {
                    File* newNode = new File(part, isLast, current);  // 最后一次创建文件
                    current->children[part] = newNode;               // 将新节点加入当前目录的子目录中
                }

                current = current->children[part]; // 进入下一级
            }
            // 当前节点必须是文件
            if (!current->isFile) {
                cout << "error3" << endl; // 如果最终节点是目录，报错
                return;
            }

            // // 修正路径为空或非法的情况
            // if (path.empty()) {
            //     cout << "error" << endl;       // 空路径报错
            //     return;
            // }

            // // 使用 navigate 定位父目录
            // size_t lastSlash = path.find_last_of('/');
            // string parentPath = lastSlash != string::npos ? path.substr(0, lastSlash) : ".";
            // string fileName = path.substr(lastSlash + 1);

            // if (fileName.empty()) {
            //     cout << "error" << endl;       // 非法路径（文件名为空）
            //     return;
            // }

            // File* parentDir = nullptr;

            // if (parentPath.empty() || parentPath == "/") {
            //     // 如果路径为空或是根目录，直接操作根目录
            //     parentDir = root;
            // } else {
            //     // 否则导航至父目录
            //     parentDir = navigate(parentPath, false); // 不强制要求路径必须存在
            // }


            // if (!parentDir || parentDir->isFile) {
            //     cout << "error" << endl;       // 父目录不存在或非法
            //     return;
            // }

            // 获取或创建目标文件
            // File* file = parentDir->getFile(fileName);
            // if (!file) {
            //     file = new File(fileName, true, parentDir);
            //     parentDir->addFile(file);
            // }

            // 执行重定向操作
            if (redirect == ">") {
                current->overwriteContent(text + "\n");
            } else if (redirect == ">>") {
                current->appendContent(text + "\n");
            }
        } else {
            cout << "error4" << endl; // 参数错误
        }
    }




    void cd(const string& path) {
        File* dir = navigate(path);
        if (dir && !dir->isFile) {
            cwd = dir;  // 切换工作目录
        } else {
            cout << "error" << endl;  // 无法切换
        }
    }

    
    void rm(const vector<string>& args) {
        if (args.size() == 1) {
            // Remove a single file
            File* file = navigate(args[0]);
            if (file && file->isFile) {
                file->parent->children.erase(file->name);
                delete file;
            } else {
                cout << "error" << endl;
            }
        } else if (args.size() == 2 && args[0] == "-r") {
            // Recursively remove a directory and its contents
            File* dir = navigate(args[1]);
            if (dir && !dir->isFile) {
                // Remove all children recursively
                dir->removeChildren();
                // Then remove the directory itself
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
            ss >> option ;
            if (option == "-p") {
                ss >> path;
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
        else if (cmd == "cat") {
            string path;
            ss >> path;
            fs.cat(path);
        }
        else if (cmd == "rm") {
            vector<string> args;
            string arg;
            while (ss >> arg) args.push_back(arg);
            fs.rm(args);
        }
        else if (cmd == "echo") {
            string remaining;
            getline(ss, remaining); // 获取echo命令后所有内容
            stringstream argStream(remaining);

            vector<string> args;
            string temp;
            bool inQuotes = false;
            string quotedText;

        while (argStream >> temp) {
            if ((temp.front() == '\'' || temp.front() == '"') && !inQuotes) {
                // 开始一个带引号的文本
                inQuotes = true;
                quotedText = temp;
                // 如果引号里没有结束，继续处理
                if (temp.back() == '\'' || temp.back() == '"') {
                    inQuotes = false;
                    args.push_back(quotedText);  // 处理完一个带引号的文本
                }
            } else if (inQuotes) {
                // 处理引号中的内容
                quotedText += " " + temp;
                if (temp.back() == '\'' || temp.back() == '"') {
                    inQuotes = false;
                    args.push_back(quotedText); // 添加完整的引号内容
                }
            } else {
                // 普通参数
                args.push_back(temp);
            }
        }


            if (inQuotes) {
                cout << "error6" << endl; // 引号不匹配
            } else {
                fs.echo(args);
            }
        }



    }


    return 0;
}
