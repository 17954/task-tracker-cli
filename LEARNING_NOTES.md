# Task Tracker 必要知识速记

## 命令行参数

```cpp
int main(int argc, char* argv[])
```

- `argc`：参数数量。
- `argv`：参数数组，元素是 C 风格字符串。
- `argv[0]` 是程序路径，`argv[1]` 是命令。
- 使用 `argv[n]` 前先检查 `argc`；带空格的描述需要命令行引号。
- `std::string command = argv[1];` 将 `char*` 转成 C++ 字符串，之后可以用 `command == "add"` 比较。

## 数据模型与容器

```cpp
enum class Status { Todo, InProgress, Done };
struct Task { /* 一条任务的所有字段 */ };
std::vector<Task> tasks;
```

- `enum class` 限制状态的取值，使用 `Status::Todo`。
- `struct` 将一条任务相关的字段放在一起。
- `vector` 是可增长的任务列表：`push_back` 新增、`erase(it)` 删除、`empty` 判空。
- 遍历只读任务用 `const Task&`；要修改任务用 `Task&`。
- `vector::size()` 返回 `std::size_t`，下标循环也优先用 `std::size_t`。

## 引用、指针与迭代器

- `const T&`：只读地使用原对象，不复制。
- `T&`：直接修改原对象。
- `T*`：对象地址；`nullptr` 表示未找到。
- `task->status` 等价于 `(*task).status`，用于指针或迭代器指向的对象。
- `it` 是迭代器，`erase(it)` 会删除它所指的 `vector` 元素。

## `std::optional`

`std::optional<T>` 表示“有一个 `T` 或没有结果”。

```cpp
auto id = parseId(text);
if (!id) {
    // 无值
} else {
    int value = *id;
}
```

- `std::nullopt`：明确返回“无值”。
- 只有确认有值后才能用 `*id` 取出内容。
- 适合表示解析失败、找不到字段、非法状态等情况。

## 文本解析与异常

- `std::stoi` 把字符串转为 `int`，非法文本或超范围时可能抛出异常。
- `consumed` 可确认整段文本都被解析，避免把 `12abc` 当作 `12`。
- `try / catch` 把转换异常变成调用者可处理的失败结果。
- id 应在输入边界验证：可解析、完全消耗、且大于 0。

## 文件与 JSON

```text
tasks.json → loadTasks → vector<Task> → 命令操作 → saveTasks → tasks.json
```

- `std::ifstream` 读文件，`std::ofstream` 写文件。
- `is_open()` 判断能否打开；写完后 `if (!file)` 可检查写入是否失败。
- `ostringstream` 可把 `file.rdbuf()` 读到字符串：文件流不是文件内容本身。
- 文件不存在时创建 `[]`；文件损坏时应报错并停止，避免覆盖原数据。
- JSON：数组用 `[]`，对象用 `{}`，字符串与键用双引号，最后一项后不加逗号。
- 手写解析器不是通用 JSON 解析器；本项目因此拒绝描述中的双引号、反斜杠、换行、花括号。

## 时间

`std::time` 取得当前时间，`std::localtime` 转换为本地时间，`std::put_time` 按 `%Y-%m-%d %H:%M:%S` 写入输出流。

创建任务时设置 `createdAt` 和 `updatedAt`；更新任务时只改变 `updatedAt`。

## 头文件、实现文件与编译

- `.h`：类型和函数声明（接口）。
- `.cpp`：函数实现和内部细节。
- `#ifndef / #define / #endif`：包含保护，防止同一头文件重复包含。
- `undefined reference`：通常是函数实现所在的 `.cpp` 没有参加链接。

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp Task.cpp Storage.cpp -o task-cli.exe
```

- `g++` 编译 C++；`-std=c++17` 启用 C++17（本项目的 `optional` 需要它）。
- `-Wall -Wextra -Wpedantic` 开启常见警告。
- `-o` 指定输出文件。

## Git 基本工作流

```powershell
git status
git add <文件名>
git commit -m "说明本次改动"
git push
```

- 文件修改后需要再次 `git add` 才会进入下一次提交。
- `.gitignore` 表示不追踪匹配文件；本项目忽略 `*.exe` 和个人数据 `tasks.json`。
- LF/CRLF 是换行格式警告，通常不影响代码或 Markdown。
