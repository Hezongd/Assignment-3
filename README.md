# 描述
本题旨在考察您对C++类/C结构体及内存管理的熟练程度，同时也测试您是否过度依赖GUI。
Linux/Unix系统拥有强大的文件系统，共享一套相似的工具集，如 `cat`、`cd`、`find`、`ls`、`mkdir`、`rm` 等。
对于此任务，您需要实现其中的一部分，具体如下：

- **cat**
  将文件内容连接到标准输出。
  用法：`cat /path/to/file`

- **cd**
  更改shell的工作目录。
  用法：`cd /path/to/directory`

- **echo**
  重复以下参数，用作文件的输入源。
  用法：
  - `echo <text content> > /path/to/file` 将给定文件的内容替换为参数，带换行符。
  - `echo <text content> >> /path/to/file` 将参数追加到给定文件的末尾，带换行符。

- **find**
  在目录层次结构中搜索文件。
  用法：`find /path-name <pattern>`

- **ls**
  列出有关文件的信息（默认为当前目录）。
  用法：`ls -a /path`

- **mkdir**
  创建目录（如果它们尚不存在）。
  用法：`mkdir -p <dirname>`

- **mv**
  重命名源文件/目录或将源文件/目录移动到目标目录。
  用法：
  - `mv /path/to/src /path/to/dst_file` 将源目录/文件移动为目标文件/目录。
  - `mv /path/to/src /path/to/dst_dir/` 将源目录/文件移动到目标目录下。

- **rm**
  删除（取消链接）文件。
  用法：`rm -r /path/to/dir` 或 `rm /path/to/file`

- **pwd**
  打印当前工作目录的名称。
  用法：`pwd`

## 输入
总共有20个测试用例。
每个测试用例的第一行给出要执行的指令数量。
接下来的几行，每行是一个按顺序执行的命令，可能的命令类型包括：

- **cat**
  将文件内容连接到标准输出。
  参数数量：1
  第一个参数表示文件系统中的路径。

- **cd**
  更改shell的工作目录。
  参数数量：1
  第一个参数表示文件系统中的路径。

- **echo**
  重复以下参数，用作文件的输入源。
  参数数量：1 或 3
  - 第一个参数表示要回显的内容，用单引号括起来或不括起来。
  - 如果适用，第二个参数可以是 `>` 或 `>>`，这是重定向操作符。
  - 第三个参数表示文件系统中的路径。
  - 如果给定路径中的文件不存在，但其父目录存在，则在该路径处创建新文件；如果其父目录也不存在，则抛出错误。

- **find**
  在目录层次结构中搜索文件。
  参数数量：3
  - 第一个参数表示文件系统中的路径。
  - 第二个参数是 `-name`。
  - 第三个参数是一个用单引号括起来的正则表达式。

- **ls**
  列出有关文件的信息（默认为当前目录）。
  参数数量：2
  - 第一个参数是 `-a`。
  - 第二个参数表示文件系统中的路径。

- **mkdir**
  创建目录及其必要的父目录（如果它们尚不存在）。
  参数数量：2
  - 第一个参数是 `-p`。
  - 第二个参数表示文件系统中的路径。

- **mv**
  重命名源文件/目录或将源文件/目录移动到目标目录。
  参数数量：2
  - 第一个参数是源文件/目录的路径。
  - 第二个参数是目标路径，如果以 `/` 结尾，表示将源文件/目录移动到该目标目录下。
  - 我们保证测试用例不会移动工作目录或其父目录。

- **rm**
  删除（取消链接）文件。
  参数数量：1 或 2
  - 如果接收到1个参数，表示文件系统中的文件路径。
  - 如果接收到2个参数，第一个参数是 `-r`，第二个参数表示文件系统中的目录路径。
  - 我们保证测试用例不会删除工作目录或其父目录。

- **pwd**
  打印当前工作目录的绝对路径。
  参数数量：0

## 输出
在一个测试用例中，对于每个输入指令，生成必要的标准输出。以下是输出格式的描述：

- **cat**
  输出文件内容，不要输出任何额外的换行符（因为文件内容可能包含换行符，由 `echo` 写入）。

- **cd**
  除非发生错误，否则不输出任何内容。

- **echo**
  如果没有指定重定向操作符，输出参数并带换行符。
  如果指定了重定向操作符，不输出任何内容，而是写入文件。

- **find**
  如果找到结果，按字典顺序输出相对于给定路径的结果，每个结果占一行。
  如果未找到结果，不输出任何内容。

- **ls -a**
  不同于标准的UNIX输出，输出给定路径下的所有项，用制表符 `\t` 分隔，按字典顺序排列。
  注意：“所有项”包括像 `.` 和 `..` 这样的链接，可以在自己的UNIX系统上尝试 `find` 查看效果。

- **mkdir**
  除非发生错误，否则不输出任何内容。

- **mv**
  除非发生错误，否则不输出任何内容。

- **rm**
  除非发生错误，否则不输出任何内容。

- **pwd**
  输出当前工作目录的绝对路径。

对于所有类型的错误，只需输出单词 `error` 占一行。错误可能包括：
- 参数数量错误，如缺少或过多的参数。
- 无效的参数，如 `find` 命令中的 `whatever-wrong` 参数。
- 访问不存在的文件/目录。
- 错误的路径，如进入文件路径或向目录路径写入等。
- 名称冲突，如创建/重命名文件为已存在的文件名。
- 其他可能的错误。

## 示例输入 #1
    20
    pwd
    mkdir -p /BanGDream/MyGO/Live20250426/songs
    mkdir -p /BanGDream/AveMujica/Live20250426/songs
    cd /BanGDream/MyGO/Live20250426/songs
    pwd
    echo 'Kajikanda kokoro furueru manazashi' > Haruhikage
    echo 'Sekai de boku wa hitoribocchi datta' >> ./Haruhikage
    echo 'Chiru koto shika shiranai haru wa' >> ../../Live20250426/songs/Haruhikage
    echo 'Maitoshi tsumetaku ashirau' >> /BanGDream/MyGO/Live20250426/songs/Haruhikage
    find /BanGDream -name 'Haru.*'
    mkdir -p /BanGDream/CRYCHIC/songs
    cd /BanGDream/CRYCHIC/songs
    mv /BanGDream/MyGO/Live20250426/songs/Haruhikage ./
    ls -a .
    cat Haruhikage
    cd /BanGDream
    rm -r ./CRYCHIC
    find /BanGDream -name 'Haru.*'
    echo 'Haruhikage deleted'
    cd CRYCHIC

## 示例输出 #1
    /
    /BanGDream/MyGO/Live20250426/songs
    Kajikanda kokoro furueru manazashi
    Sekai de boku wa hitoribocchi datta
    Chiru koto shika shiranai haru wa
    Maitoshi tsumetaku ashirau
    Haruhikage deleted
    error

## 测试用例
总共有20个测试用例，每个测试用例占5分。鼓励您实现部分功能并提交半成品代码以测试已实现部分是否正确。对于测试用例1-18，我们保证不会发生错误。您不必过于担心效率，简单的深度优先搜索就足够用于 `find` 命令。

### 测试用例描述
| 测试用例编号 | 使用的命令 | 数据量 | 描述 | 描述 | 描述 |
| --- | --- | --- | --- | --- | --- |
| 1 | echo, cat | 1e4 | | | |
| 2 | mkdir, cd, pwd | 1e4 | 仅绝对路径 | 仅绝对路径 | 仅绝对路径 |
| 3 | mkdir, cd, pwd | 1e4 | 相对/绝对路径 | 相对/绝对路径 | 相对/绝对路径 |
| 4 | cat, cd, echo, ls, mkdir, rm | 1e3 | | | |
| 5 | cat, cd, echo, ls, mkdir, rm | 1e4 | | | |
| 6 | cat, cd, echo, ls, mkdir, mv | 1e3 | | | |
| 7 | cat, cd, echo, ls, mkdir, mv | 1e4 | | | |
| 8 | cat, cd, echo, ls, mkdir, find | 1e3 | | | |
| 9 | cat, cd, echo, ls, mkdir, find | 1e4 | | | |
| 10 | cat, cd, echo, ls, mkdir, rm, mv | 1e5 | | | |
| 11 | cat, cd, echo, ls, mkdir, rm, find | 1e5 | | | |
| 12 | cat, cd, echo, ls, mkdir, mv, find | 1e4 | | | |
| 13 | 以上所有命令的混合 - 1 | 1e3 | | | |
| 14 | 以上所有命令的混合 - 2 | 1e3 | | | |
| 15 | 以上所有命令的混合 - 3 | 1e4 | | | |
| 16 | 以上所有命令的混合 - 4 | 1e4 | | | |
| 17 | mkdir, echo, find, rm | 2e4 | `find` 的效率测试 | | |
| 18 | cd, mkdir, echo, rm | 1e4 | `rm` 的内存泄漏测试 | | |
| 19 | 以上所有命令的混合 - 3（带错误） | 1e4 | 参数可能错误 | 参数可能错误 | 参数可能错误 |
| 20 | 以上所有命令的混合 - 4（带错误） | 1e4 | 路径、名称和依赖关系可能错误 | 路径、名称和依赖关系可能错误 | 路径、名称和依赖关系可能错误 |

### 提示
注意路径格式的许多细节，例如目录路径可能以 `/` 结尾，但有时这个斜杠可能会省略。
大多数上述命令的描述和文档可以在GNU Coreutils中找到。
对于 `find` 命令的正则表达式，可以尝试使用C++11中的正则表达式库。
在自己的UNIX PC上尝试这些命令是理解它们如何与文件系统交互的好方法。
小心处理内存分配。不正确地分配、访问和释放资源将导致各种运行时错误和超出内存限制的错误。
使用C++实现并使用派生类和智能指针比使用C更容易。然而，您仍然被鼓励尝试一下。我相信无论您之前对UNIX、内存和指针了解多少，解决这个问题后您都可以成为它们的高手。
