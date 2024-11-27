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

        // 包括 "." 和 ".." 在内的所有项
        vector<string> entries = {".", ".."};

        // 将所有目录项加入 entries
        for (const auto& [name, _] : dir->children) {
            entries.push_back(name);
        }

        // 按字典顺序排序
        sort(entries.begin(), entries.end());

        // 输出所有项，用制表符分隔
        for (size_t i = 0; i < entries.size(); ++i) {
            cout << entries[i];
            if (i + 1 < entries.size()) {
                cout << "\t";  // 制表符分隔
            } else {
                cout << "\n";  // 最后一个项后换行
            }
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
    
    void cd(const string& path) {
        File* dir = navigate(path);
        if (dir && !dir->isFile) {
            cwd = dir;  // 切换工作目录
        } else {
            cout << "error" << endl;  // 无法切换
        }
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
                cout << "error" << endl;        // 非法操作符
                return;
            }
            // 修正路径为空或非法的情况
            if (path.empty()) {
                cout << "error" << endl;       // 空路径报错
                return;
            }

            vector<string> parts = splitPath(path);  // 获取路径的各个部分
            File* current = path.empty() || path[0] != '/' ? cwd : root;  // 从当前工作目录开始

            // 遍历路径的每个部分，除了最后一个部分之外，都要确保路径存在
            for (size_t i = 0; i < parts.size() - 1; ++i) {
                const string& part = parts[i];
 
                // 如果当前目录没有这个子目录，报错并返回
                if (current->children.find(part) == current->children.end()) {
                    cout << "error" << endl;
                    return;
                }

                current = current->children[part]; // 进入下一级目录
            }
            //如果最后一个路径是文件夹，报错并返回

            // 最后一个部分是文件，要确保其存在，如果不存在则创建它
            const string& fileName = parts.back();
            File* file = nullptr;

            if (current->children.find(fileName) == current->children.end()) {
                file = new File(fileName, true, current); // 创建新文件
                current->children[fileName] = file;
            } else {
                file = current->children[fileName]; // 文件已经存在
            }

            // 执行重定向操作
            if (redirect == ">") {
                file->overwriteContent(text + "\n");
            } else if (redirect == ">>") {
                file->appendContent(text + "\n");
            }
        } else {
            cout << "error" << endl; // 参数错误
        }
    }



    
    void rm(const vector<string>& args) {
        if (args.size() == 1) {
            // Remove a single file
            File* file = navigate(args[0]);
            if (file && file->isFile) {
                file->parent->children.erase(file->name);
                delete file;
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
            } 
        } 
    }
    // find 函数
    void find(const string& startPath, const string& pattern) {//find 要加上文件目录本身
        File* startDir = navigate(startPath);
        if (!startDir) {
            return;  // 如果起始路径不存在，直接返回
        }

        vector<string> results;
        regex re(pattern);

        findHelper(startDir, re, ".", results);

        sort(results.begin(), results.end());
        for (const string& result : results) {
            if (startPath.back() == '/') {
                cout << startPath << result.substr(2) << endl;  // 去掉 "./"
            } else {
                cout << startPath << "/" << result.substr(2) << endl;  // 去掉 "./"
            }
        }
    }

    // findHelper 函数
    void findHelper(File* dir, const regex& re, const string& path, vector<string>& results) {
        for (const auto& child : dir->children) {
            string childPath = path + "/" + child.second->name;
            if (regex_match(child.second->name, re)) {
                results.push_back(childPath);
            }
            if (!child.second->isFile) {
                findHelper(child.second, re, childPath, results);
            }
        }
    }



    // void mv(const string& srcPath, const string& dstPath) {
    //     File* srcFile = navigate(srcPath);
    //     if (!srcFile) {
    //         cout << "error: source path does not exist" << endl;
    //         return;
    //     }

    //     File* dstDir = nullptr;
    //     string newName = dstPath;

    //     if (dstPath.back() == '/') {
    //         // 移动到目标目录下
    //         dstDir = navigate(dstPath.substr(0, dstPath.size() - 1));
    //         if (!dstDir) {
    //             // 目标目录不存在，创建目录
    //             dstDir = new File(dstPath.substr(0, dstPath.size() - 1), false, cwd);
    //             cwd->children[dstDir->name] = dstDir;
    //         }
    //         newName = srcFile->name;
    //     } else {
    //         // 重命名或移动为目标文件/目录
    //         size_t pos = dstPath.find_last_of('/');
    //         if (pos != string::npos) {
    //             dstDir = navigate(dstPath.substr(0, pos));
    //             if (!dstDir) {
    //                 // 目标目录不存在，创建目录
    //                 dstDir = new File(dstPath.substr(0, pos), false, cwd);
    //                 cwd->children[dstDir->name] = dstDir;
    //             }
    //             newName = dstPath.substr(pos + 1);
    //         } else {
    //             dstDir = cwd;
    //             newName = dstPath;
    //         }
    //     }

    //     // if (dstDir->children.find(newName) != dstDir->children.end()) {
    //     //     cout << "error: destination path already exists" << endl;
    //     //     return;
    //     // }

    //     // 从源目录中删除
    //     if (srcFile->parent) {
    //         srcFile->parent->children.erase(srcFile->name);
    //     }

    //     // 更新文件名和父目录
    //     srcFile->name = newName;
    //     srcFile->parent = dstDir;
    //     dstDir->children[newName] = srcFile;

    //     // 如果是文件，更新内容
    //     if (srcFile->isFile) {
    //         dstDir->children[newName]->content = srcFile->content;
    //     } else {
    //         // 如果是目录，更新子目录的父指针
    //         for (auto& [name, child] : srcFile->children) {
    //             child->parent = dstDir->children[newName];
    //         }
    //     }
    // }
    void mv(const string& srcPath, const string& dstPath) {
        File* srcFile = navigate(srcPath);
        if (!srcFile) {
            cout << "error: source path does not exist" << endl;
            return;
        }
        File* dstDir = navigate(dstPath);

        if (dstPath.back() == '/') {
            srcFile->parent->children.erase(srcFile->name);
            dstDir->children[srcFile->name] = srcFile;
            srcFile->parent = dstDir;
        }else{
            if(dstDir){
                File* p = dstDir->parent;
                //删除dstpath的文件
                dstDir->parent->children.erase(dstDir->name);
            }
            vector<string> parts = splitPath(dstPath);  // 获取路径的各个部分
            File* current = dstPath.empty() || dstPath[0] != '/' ? cwd : root;  // 从当前工作目录开始
            for (size_t i = 0; i < parts.size() - 1; ++i) {
                const string& part = parts[i];
                if (current->children.find(part) == current->children.end()) {
                    File* newDir = new File(part, false, current);  // 创建新目录
                    current->children[part] = newDir;  // 将新目录加入当前目录的子目录中
                    return;
                }
                current = current->children[part]; // 进入下一级目录
            }
            srcFile->parent->children.erase(srcFile->name);
            srcFile->parent = current;
            srcFile->name = parts.back();
            current->children[srcFile->name] = srcFile;
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
                cout << "error" << endl; // 引号不匹配
            } else {
                fs.echo(args);
            }
        }else if (cmd == "ls") {
            string option, path = "";
            if (ss >> option) {  // 读取第一个参数
                if (option == "-a") {
                    if (ss >> path) {  // 如果有路径，读取路径
                        fs.ls(path);  // 调用ls，传入路径
                    } else {
                        fs.ls("");  // 如果没有路径，使用当前目录
                    }
                } else {
                    cout << "error" << endl;  // 如果没有 -a 选项，或者有其他不合法的选项
                }
            } else {
                fs.ls("");  // 默认无参数时，列出当前目录
            }
        }else if (cmd == "find") {
            string startPath, option, pattern;
            ss >> startPath >> option >> pattern;
            string p = pattern.substr(1, pattern.size() - 2);
            if (option == "-name") {
                fs.find(startPath, p);
            }
        }else if (cmd == "mv") {
            string srcPath, dstPath;
            ss >> srcPath >> dstPath;
            fs.mv(srcPath, dstPath);
        }
    }

    return 0;
}
