# Task Tracker CLI

## 简介

一个使用 C++17 编写的命令行任务管理工具。它不依赖第三方库，使用当前工作目录中的 JSON 文件保存任务，支持添加、修改、删除、更新状态和按状态查询任务。

每个任务包含以下信息：

- `id`
- `description`
- `status`（`todo`、`in-progress` 或 `done`）
- `createdAt`
- `updatedAt`

## 构建

在项目目录中执行：

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp Task.cpp Storage.cpp -o task-cli.exe
```

## 使用方法

```powershell
# 添加任务
.\task-cli.exe add "<description>"

# 修改任务描述
.\task-cli.exe update <id> "<description>"

# 删除任务
.\task-cli.exe delete <id>

# 更新任务状态
.\task-cli.exe mark-in-progress <id>
.\task-cli.exe mark-done <id>

# 列出任务
.\task-cli.exe list
.\task-cli.exe list todo
.\task-cli.exe list in-progress
.\task-cli.exe list done
```

## 数据存储

任务保存在程序当前工作目录的 `tasks.json` 中。文件不存在时，程序会自动创建一个空任务列表。

## 项目结构

- `main.cpp`：命令行参数处理和任务操作
- `Task.h` / `Task.cpp`：任务模型、状态转换
- `Storage.h` / `Storage.cpp`：JSON 文件的读取与写入

## 当前限制

- 任务描述不能为空。
- 为了配合当前手写的 JSON 读写逻辑，任务描述不能包含双引号、反斜杠、换行或花括号。
