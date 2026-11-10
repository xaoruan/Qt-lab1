# 带键盘事件的计算器

Qt Widgets 四则运算计算器，界面按键顺序与实验文档中的 Windows 计算器示例一致。
鼠标按钮和键盘输入统一进入 `CalculatorEngine`，支持小数、退格、清除、倒数、
平方、平方根、百分号、连续运算和除零提示。

## 构建

```powershell
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH=D:\qt\6.11.2\mingw_64 `
  -DCMAKE_CXX_COMPILER=D:\qt\Tools\mingw1310_64\bin\g++.exe
cmake --build build
ctest --test-dir build --output-on-failure
```

## 键盘操作

数字、`.`、`+`、`-`、`*`、`/` 与鼠标按钮功能一致；Enter 或 `=` 计算，
Backspace 退格，Esc 或 Delete 清除。

## 实验二：简单文本编辑器

实验二位于 `lab2-simple-text-editor/`，独立实现多窗口文本编辑器、文件读写、
查找替换、格式设置、状态栏、行号显示和定时自动保存。实验一工程保持独立，
根目录的 CMake 工程可以同时构建两个实验。

实验二构建与测试说明见
[`lab2-simple-text-editor/README.md`](lab2-simple-text-editor/README.md)。
