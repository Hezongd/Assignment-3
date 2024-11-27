# Update at 14:46 2024-11-27
# Todo: mv, find
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <regex>

using namespace std;

class FileSystemNode {
public:
    string name;
    bool isFile;
    FileSystemNode *parent;
    map<string, FileSystemNode*> children;
    string content;

    FileSystemNode(string name, bool isFile = false, FileSystemNode *parent = nullptr)
        : name(name), isFile(isFile), parent(parent) {}

    ~FileSystemNode() {
        for (auto &child : children) {
            delete child.second;
        }
    }

    void addNode(FileSystemNode *node) {
        if (!isFile) { // Only directories can have children
            children[node->name] = node;
        } else {
            cout << "error\n";
        }
    }

    FileSystemNode* getNode(string name) {
        if (children.find(name) != children.end()) {
            return children[name];
        }
        return nullptr;
    }
};

class FileSystem {
private:
    FileSystemNode *root;
    FileSystemNode *currentDir;

public:
    FileSystem() : root(new FileSystemNode("/")), currentDir(root) {}

    ~FileSystem() {
        delete root;
    }

    void cd(const string &path) {
        vector<string> parts = splitPath(path);
        FileSystemNode *temp = path.empty() || path[0] != '/' ? currentDir : root;

        for (const auto &part : parts) {
            if (part == "..") {
                if (temp->parent != nullptr) temp = temp->parent;
            } else if (part == ".") {
                continue;
            } else {
                temp = temp->getNode(part);
                if (temp == nullptr) {
                    cout << "error\n";
                    return;
                }
            }
        }
        currentDir = temp;
    }

    void mkdir(const string &path, bool createParents = false) {
        vector<string> parts = splitPath(path);
        FileSystemNode *temp = path.empty() || path[0] != '/' ? currentDir : root;

        for (const auto &part : parts) {
            FileSystemNode *next = temp->getNode(part);
            if (next == nullptr) {
                if (createParents) {
                    next = new FileSystemNode(part, false, temp);
                    temp->addNode(next);
                } else {
                    cout << "error\n";
                    return;
                }
            }
            temp = next;
        }
    }

    void ls(const string &path) {
        vector<string> parts = splitPath(path);
        FileSystemNode *temp = path.empty() || path[0] != '/' ? currentDir : root;

        for (const auto &part : parts) {
            if (part == "..") {
                if (temp->parent != nullptr) temp = temp->parent;
            } else if (part == ".") {
                continue;
            } else {
                temp = temp->getNode(part);
                if (temp == nullptr) {
                    cout << "error\n";
                    return;
                }
            }
        }

        vector<string> entries;
        
        entries.push_back(".");
        entries.push_back("..");
        

        for (auto &child : temp->children) {
            entries.push_back(child.first);
        }

        sort(entries.begin(), entries.end());
        for (size_t i = 0; i < entries.size(); ++i) {
            cout << entries[i];
            if (i < entries.size() - 1) {
                cout << "\t";
            }
        }
        cout << endl;
    }

    void cat(const string &path) {
        vector<string> parts = splitPath(path);
        FileSystemNode *temp = path.empty() || path[0] != '/' ? currentDir : root;

        for (const auto &part : parts) {
            temp = temp->getNode(part);
            if (temp == nullptr) {
                cout << "error\n";
                return;
            }
        }

        if (temp->isFile) {
            cout << temp->content ;
        } else {
            cout << "error\n";
        }
    }

    void echo(const string &content, const string &path, bool append) {
        vector<string> parts = splitPath(path);
        FileSystemNode *temp = path.empty() || path[0] != '/' ? currentDir : root;

        // Traverse to the parent directory of the target file
        for (size_t i = 0; i < parts.size() - 1; ++i) {
            if (parts[i] == "..") {
                if (temp->parent != nullptr) {
                    temp = temp->parent;
                } else {
                    cout << "error\n";
                    return;
                }
        } else {
                temp = temp->getNode(parts[i]);
                if (temp == nullptr) {
                    cout << "error\n";
                    return;
                }
            }
        }

        // Get the target file node
        FileSystemNode *file = temp->getNode(parts.back());

        // Create the file if it doesn't exist
        if (file == nullptr) {
            file = new FileSystemNode(parts.back(), true, temp);
            temp->addNode(file);
        } else if (!file->isFile) {
            cout << "error\n"; // The target is not a file
            return;
        }

        // Append or overwrite the content
        if (append) {
            file->content += content + "\n";
        } else {
            file->content = content + "\n";
        }
}

    void find(const string &path, const string &pattern) {
        vector<string> parts = splitPath(path);
        FileSystemNode *temp = path.empty() || path[0] != '/' ? currentDir : root;

        for (const auto &part : parts) {
            temp = temp->getNode(part);
            if (temp == nullptr) {
                cout << "error\n";
                return;
            }
        }

        vector<string> matches;
        findHelper(temp, "", pattern, matches);

        sort(matches.begin(), matches.end());
        for (const auto &match : matches) {
            cout << match << endl;
        }
    }

    void findHelper(FileSystemNode *node, string path, const string &pattern, vector<string> &matches) {
        if (regex_match(node->name, regex(pattern))) {
            matches.push_back(path + node->name);
        }

        for (auto &child : node->children) {
            findHelper(child.second, path + node->name + "/", pattern, matches);
        }
    }

    void pwd() {
        FileSystemNode *temp = currentDir;
        string path = "";
        while (temp != root) {
            path = "/" + temp->name + path;
            temp = temp->parent;
        }
        if(path.empty()){
            path = "/";
        }
        cout  << path << endl;
    }

    void mv(const string &src, const string &dest) {
        vector<string> srcParts = splitPath(src);
        vector<string> destParts = splitPath(dest);
        FileSystemNode *srcNode = currentDir;
        FileSystemNode *destNode = currentDir;

        if (src[0] == '/') {
            srcNode = root;
        }
        if (dest[0] == '/') {
            destNode = root;
        }

        for (const auto &part : srcParts) {
            srcNode = srcNode->getNode(part);
            if (srcNode == nullptr) {
                cout << "error\n";
                return;
            }
        }

        for (size_t i = 0; i < destParts.size() - 1; ++i) {
            destNode = destNode->getNode(destParts[i]);
            if (destNode == nullptr) {
                cout << "error\n";
                return;
            }
        }

        if (destNode->getNode(destParts.back()) != nullptr) {
            cout << "error\n";
            return;
        }

        srcNode->parent->children.erase(srcNode->name);
        srcNode->parent = destNode;
        srcNode->name = destParts.back();
        destNode->addNode(srcNode);
    }

    void rm(const string &path, bool recursive) {
        vector<string> parts = splitPath(path);
        FileSystemNode *temp = path.empty() || path[0] != '/' ? currentDir : root;

        // Traverse to the target node
        for (const auto &part : parts) {
            if (part == "..") {
                if (temp->parent != nullptr) temp = temp->parent;
            } else if (part == ".") {
                continue;
            } else {
                temp = temp->getNode(part);
                if (temp == nullptr) {
                    //cout << "error\n";
                    return;
                }
            }
        }
        if(recursive && !temp->isFile){
            if(!temp->children.empty()){
                for (auto &child : temp->children) {
                    rm(child.first, temp->getNode(child.first)->isFile);
                }
            }
        }
        if (temp->parent != nullptr) {
            temp->parent->children.erase(temp->name);
        }

        // Delete the node and free memory
        delete temp;
            
    }
    string trimQuotes(const string &str) {
        if (str.front() == '\'' && str.back() == '\'') {
            return str.substr(1, str.size() - 2);
        }
        return str;
    }

    void parseEchoCommand(const string &commandLine, string &content, string &op, string &path) {
        vector<string> tokens;
        stringstream ss(commandLine);
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        if (tokens.size() == 1) {
            content = tokens[0];
            if(content[0] == '\'' && content[content.size() - 1] == '\''){
                content = content.substr(1, content.size() - 2);
            }
            op = "";
            path = "";
            return;
        }
        if (tokens[tokens.size() - 2] == ">" || tokens[tokens.size() - 2] == ">>") {
            content = "";
            for (int i = 0; i < tokens.size() - 2; i++) {
                content += tokens[i] + " ";
            }
            content = trimTrailingSpaces(content);
            if(content[0] == '\'' && content[content.size() - 1] == '\''){
                content = content.substr(1, content.size() - 2);
            }
            op = tokens[tokens.size() - 2];
            path = tokens[tokens.size() - 1];
        }else {
            content = "";
            for (int i = 0; i < tokens.size(); i++) {
                content += tokens[i] + " ";
            }
            content = trimTrailingSpaces(content);
            if(content[0] == '\'' && content[content.size() - 1] == '\''){
                content = content.substr(1, content.size() - 2);
            }
            op = "";
            path = "";
        }
    }

    string trimTrailingSpaces(const string &str) {
        size_t end = str.find_last_not_of(' ');
        return (end == string::npos) ? "" : str.substr(0, end + 1);
    }

private:
    vector<string> splitPath(const string &path) {
        vector<string> parts;
        stringstream ss(path);
        string item;
        while (getline(ss, item, '/')) {
        if (item == "." || item.empty()) continue; // 忽略当前目录和空部分
        if (item == "..") {
            if (!parts.empty() && parts.back() != "..") {
                parts.pop_back(); // 回退到上一级目录
            } else {
                parts.push_back(item); // 保留多余的 ../
            }
        } else {
            parts.push_back(item);
        }
    }
        return parts;
    }
    
};

int main() {
    FileSystem fs;
    int n;
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; ++i) {
        string commandLine;
        getline(cin, commandLine);

        istringstream iss(commandLine);
        string command;
        iss >> command;

        if (command == "cd") {
            string path;
            iss >> path;
            fs.cd(path);
        } else if (command == "mkdir") {
            string flag, path;
            iss >> flag;
            if (flag == "-p") {
                iss >> path;
                fs.mkdir(path, true);
            } else {
                //path = flag;
                //fs.mkdir(path);
                cout << "error\n";
            }
        } else if (command == "ls") {
            /*
            string path = "/";
            if (iss >> path) {
                fs.ls(path);
            } else {
                fs.ls("");
            }*/
           string flag, path;
           iss >> flag;
           if (flag == "-a") {
               iss >> path;
               fs.ls(path);
           }else{
               cout << "error\n";
           }
        } else if (command == "cat") {
            string path;
            iss >> path;
            fs.cat(path);
        } else if (command == "echo") {
            string content, op, path;
            string remainingLine;
            getline(iss, remainingLine); // Read the rest of the line

            fs.parseEchoCommand(remainingLine, content, op, path);
            /*
            cout << "\nremainingLine: " << remainingLine << endl;
            cout << "content: " << content << endl;
            cout << "op: " << op << endl;
            cout << "path: " << path << endl;
            */
            
            if (op == "") {
                cout << content << endl;
            } else {
                fs.echo(content, path, op == ">>");
            }
        } else if (command == "find") {
            string startPath, pattern;
            iss >> startPath >> pattern;
            fs.find(startPath, pattern);
        } else if (command == "pwd") {
            fs.pwd();
        } else if (command == "mv") {
            string src, dest;
            iss >> src >> dest;
            fs.mv(src, dest);
        } else if (command == "rm") {
            string path;
            bool recursive = false;
            if (iss >> path) {
                if (path == "-r") {
                    iss >> path;
                    recursive = true;
                }
                fs.rm(path, recursive);
            } else {
                cout << "error\n";
            }
        } else {
            cout << "error\n";
        }
    }

    return 0;
}

