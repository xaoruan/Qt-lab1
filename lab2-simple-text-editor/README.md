# 实验二 简单文本编辑器

本项目实现 Qt Widgets 多窗口文本编辑器。主窗口由 Qt Designer 布局，使用
QAction 组织菜单和工具栏；关于、查找、替换使用独立对话框；编辑区采用从
QPlainTextEdit 派生的 `CodeEditor`，自行绘制行号并高亮当前行。

## 功能

- 新建、打开、保存、另存为，以及未保存关闭提示
- 文件不存在、不可读和非 UTF-8/UTF-16 内容提示
- 撤销、恢复、剪切、复制、粘贴、全选
- 动作使能状态随选择、撤销栈和剪贴板实时更新
- 字体、字体颜色、字体背景色和编辑器背景色
- 自动换行、显示/隐藏行号、显示/隐藏工具栏和状态栏
- 状态栏显示字符数、行数、光标行号和列号
- 向上或向下查找，区分大小写，到达边界后循环查找
- 替换当前匹配和全部替换，并处理空查找、未找到等情况
- 每 30 秒自动保存已有文件，关闭或首次保存前不会自动覆盖当前位置

## 构建

当前环境使用 Qt 6.11.2、MinGW 13.1.0、CMake 和 Ninja：

```powershell
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH=D:\qt\6.11.2\mingw_64 `
  -DCMAKE_CXX_COMPILER=D:\qt\Tools\mingw1310_64\bin\g++.exe `
  -DCMAKE_MAKE_PROGRAM=D:\qt\Tools\Ninja\ninja.exe
cmake --build build --target simple_text_editor
```

CMake 同时保留 Qt 5/Qt 6 双版本检测；本机按现有 Qt 版本直接构建，未安装或
切换 Qt 版本。

## 目录

```text
src/
  codeeditor.*       行号编辑控件
  mainwindow.*       主窗口与业务联动
  finddialog.*       查找对话框
  replacedialog.*    替换对话框
  aboutdialog.*      关于对话框
  fileservice.*      UTF-8/UTF-16 文件读写
  textsearch.*       查找与全部替换
resources/            图标与样式表
```
