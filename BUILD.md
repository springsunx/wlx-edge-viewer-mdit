# wlx-edge-viewer 构建指南

## 前置要求

### 必需工具

- **MS Visual Studio 2022** (v143 toolset)
  - 工作负载: "Desktop development with C++"
  - 组件: Windows 10/11 SDK
  
- **vcpkg** (C++ 包管理器)
  - 已集成到 MSBuild
  - 环境变量 `VCPKG_ROOT` 已设置

- **Windows SDK 10.0**

### 可选工具

- **Git** (用于克隆仓库)
- **7-Zip** (用于手动打包)

## 快速构建

### 方法 1: 使用构建脚本 (推荐)

1. 打开 `Developer Command Prompt for VS 2022`
2. 进入项目目录
3. 运行:

```batch
build.bat
```

构建完成后，发布包在 `Release-YYYYMMDD.zip`

### 方法 2: 使用 Visual Studio

1. 打开 `EdgeViewer.sln`
2. 选择 `Release | Win32` 配置，生成
3. 选择 `Release | x64` 配置，生成
4. 运行 `BuildMakeSetup.bat` 打包

### 方法 3: 命令行手动构建

```batch
:: 设置环境
call "%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" x86

:: 编译 Win32
msbuild EdgeViewer.sln /t:Build /p:Configuration=Release;Platform=Win32;UseEnv=true

:: 编译 x64
call "%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" x64
msbuild EdgeViewer.sln /t:Build /p:Configuration=Release;Platform=x64;UseEnv=true

:: 打包
mkdir Build\Release
xcopy /E /I /Q Resources\* Build\Release\
copy Build\EdgeViewer_Win32_Release\EdgeViewer-Win32.dll Build\Release\EdgeViewer.wlx
copy Build\EdgeViewer_x64_Release\EdgeViewer-x64.dll Build\Release\EdgeViewer.wlx64
```

## 自动构建 (GitHub Actions)

本项目已配置 GitHub Actions，推送到 `v*` 标签时自动构建：

```bash
git tag v1.0.0
git push origin v1.0.0
```

Actions 会自动:
1. 编译 Win32 和 x64 版本
2. 创建发布包
3. 上传到 GitHub Releases

## 构建产物

| 文件 | 说明 |
|------|------|
| `EdgeViewer.wlx` | 32-bit Total Commander 插件 |
| `EdgeViewer.wlx64` | 64-bit Total Commander 插件 |
| `edgeviewer.ini` | 配置文件 |
| `assets/` | 渲染资源目录 |

## 目录结构

```
wlx-edge-viewer/
├── .github/workflows/
│   └── build.yml           # GitHub Actions 工作流
├── EdgeViewer/             # C++ 源码
│   ├── Processors/         # 文件处理器
│   │   ├── MdProcessor.cpp # Markdown 处理器 (已更新)
│   │   └── ...
│   └── ...
├── mINI/                   # INI 解析库
├── Resources/              # 资源文件
│   ├── assets/
│   │   ├── markdown/       # Markdown 渲染资源
│   │   ├── markdown-it/    # mdit-plugins 框架
│   │   │   ├── markdown-it.min.js
│   │   │   ├── mditPluginRegistry.js
│   │   │   └── mdit-plugins/
│   │   └── ...
│   └── edgeviewer.ini      # 默认配置
├── EdgeViewer.sln          # Visual Studio 解决方案
├── build.bat               # 构建脚本
├── BuildMakeSetup.bat      # 原始构建脚本
└── BUILD.md                # 本文档
```

## vcpkg 依赖

项目使用 vcpkg 管理以下依赖:

- `wil` - Windows Implementation Libraries
- `webview2` - Microsoft Edge WebView2

vcpkg 会自动通过 `vcpkg.json` 清单文件安装依赖。

## 故障排除

### msbuild 找不到

确保从 Developer Command Prompt 运行，或手动设置环境:
```batch
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```

### vcpkg 错误

确保 vcpkg 已正确安装并集成:
```batch
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install
```

### WebView2 编译错误

确保安装了 Windows SDK 10.0，并在 Visual Studio Installer 中启用 "C++ ATL" 组件。

## 清理

```batch
build.bat clean
```

或手动删除 `Build/` 目录。
