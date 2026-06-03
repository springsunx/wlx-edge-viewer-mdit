# mdit-plugins 集成框架

## 概述

本框架将 [mdit-plugins](https://github.com/mdit-plugins/mdit-plugins) 集成到 wlx-edge-viewer 中，
提供 **声明式插件管理** 和 **运行时启用/禁用** 功能。

## 架构

```
Resources/assets/
├── markdown-it/
│   ├── markdown-it.min.js              # markdown-it 核心 (替换 marked.js)
│   ├── mditPluginRegistry.js           # 插件注册表框架 (核心)
│   └── mdit-plugins/                   # 插件 UMD 包目录
│       ├── mdit-plugin-abbr.umd.js
│       ├── mdit-plugin-alert.umd.js
│       ├── mdit-plugin-container.umd.js
│       ├── mdit-plugin-footnote.umd.js
│       ├── mdit-plugin-tasklist.umd.js
│       └── ... (更多插件)
└── markdown/
    ├── loader.html                     # 主加载页 (使用 markdown-it)
    ├── mdit-plugin-alert.css           # 插件样式
    ├── mdit-plugin-tab.css
    ├── mdit-plugin-spoiler.css
    ├── mdit-plugin-tasklist.css
    └── ... (现有样式保留)
```

## 已集成插件列表

### 文本格式化
| 插件 | 语法 | 说明 |
|------|------|------|
| Abbr | `*[HTML]: Hyper Text Markup Language` | 缩写定义，鼠标悬停显示全称 |
| Mark | `==marked text==` | 高亮标记 |
| Sub | `H~2~O` | 下标 |
| Sup | `X^2^` | 上标 |
| Ins | `++inserted++` | 插入文本 |
| Ruby | `{base}(annotation)` | 注音标注 |

### 块级元素
| 插件 | 语法 | 说明 |
|------|------|------|
| Alert | `> [!NOTE]` `> [!TIP]` 等 | GitHub 风格告警框 |
| Container | `::: warning` ... `:::` | 自定义容器 |
| Dl | `Term` 换行 `: Definition` | 定义列表 |
| Footnote | `text[^1]` + `[^1]: content` | 脚注 |
| Tasklist | `- [x] done` `- [ ] todo` | 任务列表 |
| Align | `->=` `<-=` `<->=` | 文本对齐 |
| Tab | `::: tabs` ... `:::` | 标签页 |

### 图片与媒体
| 插件 | 语法 | 说明 |
|------|------|------|
| Figure | 图片自动转 `<figure>` | 图片容器 |
| ImgSize | `![alt](url =100x200)` | 指定图片尺寸 |
| ImgLazyload | 自动添加 `loading="lazy"` | 图片懒加载 |
| ImgMark | 图片标记语法 | 图片标记 |

### 属性与样式
| 插件 | 语法 | 说明 |
|------|------|------|
| Attrs | `{.class #id key=value}` | 通用属性语法 |
| Stylize | 自定义内联样式 | 需配合 Attrs |
| Spoiler | `\|\|hidden text\|\|` | 隐藏文本 (鼠标悬停显示) |

### 图表与代码
| 插件 | 语法 | 说明 |
|------|------|------|
| Uml | ` ```mermaid` | Mermaid 图表 (保留兼容) |
| Plantuml | `@startuml` ... `@enduml` | PlantUML 图表 |

### 数学公式
| 插件 | 语法 | 说明 |
|------|------|------|
| Katex | `$...$` 和 `$$...$$` | KaTeX 数学公式 |

### 嵌入与链接
| 插件 | 语法 | 说明 |
|------|------|------|
| Embed | `@[type](id)` | 嵌入语法 |
| Icon | `:icon-name:` | 图标语法 |
| Emoji | `:smile:` `:heart:` | Emoji 表情 |

### 布局
| 插件 | 语法 | 说明 |
|------|------|------|
| Layout | 布局语法 | 页面布局 |
| Field | 字段语法 | 字段定义 |

### 开发/演示
| 插件 | 语法 | 说明 |
|------|------|------|
| Demo | 演示块 | 演示代码 |
| InlineRule | 自定义内联规则 | 内联规则工具 |

## 使用方法

### 1. 启用/禁用插件

编辑 `EdgeViewer.ini` 的 `[Markdown]` section：

```ini
[Markdown]
CSS=github.css
CSSDark=github.dark.css

; 设置为 0 禁用，1 启用 (默认启用)
EnableAbbr=1
EnableAlert=1
EnableKatex=0       ; 禁用 KaTeX
EnableTab=0         ; 禁用标签页
```

**注意**: 修改后需重新打开文件才能生效。

### 2. 添加新插件

#### 步骤 1: 下载插件 UMD 包

```bash
# 从 npm CDN 下载
curl -L "https://cdn.jsdelivr.net/npm/@mdit/plugin-<name>/dist/cdn.umd.js" \
  -o "Resources/assets/markdown-it/mdit-plugins/mdit-plugin-<name>.umd.js"

# 如果有 CSS
curl -L "https://cdn.jsdelivr.net/npm/@mdit/plugin-<name>/<name>.css" \
  -o "Resources/assets/markdown/mdit-plugin-<name>.css"
```

#### 步骤 2: 在 loader.html 中添加 script 标签

```html
<!-- 在对应分类区域添加 -->
<script src="http://assets.example/markdown-it/mdit-plugins/mdit-plugin-<name>.umd.js"></script>
```

#### 步骤 3: 在 mditPluginRegistry.js 中添加定义

在 `PLUGINS` 数组中添加一条记录：

```javascript
{
    name: 'MyPlugin',                    // 唯一标识 (对应 INI Enable<Name>)
    globalVar: 'mdItPluginMyPlugin',     // UMD 全局变量名
    exportKey: 'myPlugin',               // 从全局变量中取插件的 key
    jsFile: 'mdit-plugin-myplugin.umd.js',
    cssFile: 'mdit-plugin-myplugin.css', // 或 null
    cssId: 'mdit-css-myplugin',          // 或 null
    initFn: null,                        // 或自定义初始化函数
    deps: [],                            // 依赖的其他插件 name 列表
    description: '我的自定义插件'
}
```

#### 步骤 4: 在 EdgeViewer.ini 中添加开关

```ini
[Markdown]
; ...
EnableMyPlugin=1
```

#### 步骤 5: 更新 MdProcessor.cpp

在 `BuildPluginConfig()` 函数的 `pluginNames` 数组中添加新插件名：

```cpp
static const char* pluginNames[] = {
    "Abbr", "Alert", /* ... */ "MyPlugin"  // 添加新插件
};
```

### 3. 添加自定义插件

如果插件不在 mdit-plugins 仓库中，可以在 `mditPluginRegistry.js` 的 `CUSTOM_PLUGINS` 数组中添加：

```javascript
const CUSTOM_PLUGINS = [
    {
        name: 'MyLocalPlugin',
        globalVar: 'myLocalPlugin',
        exportKey: 'default',
        jsFile: 'my-local-plugin.umd.js',  // 放在 mdit-plugins 目录
        cssFile: 'my-local-plugin.css',    // 放在 markdown 目录
        cssId: 'mdit-css-mylocal',
        initFn: function(md) {
            // 自定义初始化逻辑
            md.use(window.myLocalPlugin.default, { /* options */ });
        },
        deps: [],
        description: '我的本地插件'
    }
];
```

### 4. 插件带配置初始化

如果插件需要配置参数，使用 `initFn`：

```javascript
{
    name: 'Container',
    globalVar: 'mdItPluginContainer',
    exportKey: 'container',
    jsFile: 'mdit-plugin-container.umd.js',
    cssFile: null,
    cssId: null,
    initFn: function(md) {
        // 注册多个容器类型
        var plugin = window.mdItPluginContainer.container;
        md.use(plugin, 'tip', { validate: function(p) { return p.trim().startsWith('tip'); } });
        md.use(plugin, 'warning', { validate: function(p) { return p.trim().startsWith('warning'); } });
        md.use(plugin, 'danger', { validate: function(p) { return p.trim().startswith('danger'); } });
    },
    deps: [],
    description: '自定义容器'
}
```

### 5. 插件依赖管理

如果插件 A 依赖插件 B，在 `deps` 中声明：

```javascript
{
    name: 'Stylize',
    // ...
    deps: ['Attrs'],  // 依赖 Attrs 插件
}
```

框架会自动按拓扑顺序加载，确保依赖先于被依赖者加载。

## 从 marked.js 迁移说明

### 主要差异

| 特性 | marked.js (旧) | markdown-it (新) |
|------|---------------|-----------------|
| 插件 API | `extensions` 数组 | `md.use(plugin)` |
| 渲染器 | `renderer` 对象 | `md.renderer.rules` |
| 表格 | 内置 | 内置 |
| GFM | 需扩展 | 内置 (linkify 等) |
| 数学公式 | MathJax 后处理 | KaTeX 插件原生支持 |

### 兼容性

- 基础 Markdown 语法完全兼容
- GitHub Alerts 从自定义实现迁移到 @mdit/plugin-alert
- Mermaid 图表保留兼容 (通过自定义 fence 规则)
- MathJax 保留作为后备渲染器
- 所有 CSS 主题保持不变

## 调试

打开浏览器开发者工具 (F12)，查看控制台输出：

```
[mdit] Registered plugins: (30) ['Abbr', 'Alert', 'Align', ...]
[mdit] Skipped plugins: []
[mdit] Plugin errors: []
```

如果有插件加载失败，会在 `Skipped` 或 `Errors` 中显示原因。

## 文件清单

### 新增文件
- `Resources/assets/markdown-it/markdown-it.min.js` - markdown-it 核心
- `Resources/assets/markdown-it/mditPluginRegistry.js` - 插件注册表框架
- `Resources/assets/markdown-it/mdit-plugins/mdit-plugin-*.umd.js` - 插件 UMD 包 (30个)
- `Resources/assets/markdown/mdit-plugin-alert.css` - 告警框样式
- `Resources/assets/markdown/mdit-plugin-tab.css` - 标签页样式
- `Resources/assets/markdown/mdit-plugin-spoiler.css` - 隐藏文本样式
- `Resources/assets/markdown/mdit-plugin-tasklist.css` - 任务列表样式

### 修改文件
- `Resources/assets/markdown/loader.html` - 使用 markdown-it + 插件框架
- `Resources/EdgeViewer.ini` - 添加插件启用/禁用配置
- `EdgeViewer/Processors/MdProcessor.cpp` - 传递插件配置到 HTML

### 保留文件 (不变)
- `Resources/assets/markdown/markdownExtension.js` - 可保留作为参考
- 所有现有 CSS 主题文件
- highlight.js, mermaid, MathJax 资源
