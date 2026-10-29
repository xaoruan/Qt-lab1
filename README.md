# 带键盘事件的计算器

Qt Widgets 四则运算计算器，鼠标按钮和键盘输入统一进入 `CalculatorEngine`，
支持小数、退格、清除、连续运算和除零提示。

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
