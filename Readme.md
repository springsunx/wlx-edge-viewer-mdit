# WLX Edge Viewer

A general-purpose lister plugin for Total Commander (32/64-bit version).

The plugin uses a modern Chromium-based [WebView2](https://developer.microsoft.com/en-us/microsoft-edge/webview2/) component to display documents. Configuration files are processed with [mINI](https://github.com/pulzed/mINI).

The following rendering libraries are used:

- Markdown: [markdown-it](https://github.com/markdown-it/markdown-it), [mdit-plugins](https://github.com/mdit-plugins/mdit-plugins), [highlight.js](https://highlightjs.org), [mermaid](https://mermaid.js.org), and [detect-charset](https://github.com/treyhunner/detect-charset).
- ReStructuredText: [restructured](https://github.com/seikichi/restructured) (converted via [Browserify](https://browserify.org)).
- AsciiDoc: [Asciidoctor.js](https://docs.asciidoctor.org/asciidoctor.js/latest/).
- MHTML: [mhtml2html](https://github.com/rg-contributions/mhtml2html).
- Directory: [Thumbnail viewer](https://github.com/rg-contributions/thumbnail-viewer).

## mdit-plugins 集成

本项目集成了 [mdit-plugins](https://github.com/mdit-plugins/mdit-plugins) 框架，支持 30+ 个 Markdown 扩展插件。

### 支持的插件

| 类别 | 插件 | 语法示例 |
|------|------|---------|
| 文本格式 | Abbr, Mark, Sub, Sup, Ins, Ruby | `==高亮==`, `H~2~O`, `X^2^` |
| 块级元素 | Alert, Container, Dl, Footnote, Tasklist, Align, Tab | `> [!NOTE]`, `::: tip` |
| 图片 | Figure, ImgSize, ImgLazyload, ImgMark | `![alt](url =100x200)` |
| 属性样式 | Attrs, Stylize, Spoiler | `{.class #id}`, `\|\|隐藏\|\|` |
| 图表 | Uml (Mermaid), Plantuml | ` ```mermaid` |
| 数学 | KaTeX | `$E=mc^2$` |
| 嵌入 | Embed, Icon, Emoji | `:smile:`, `:heart:` |
| 布局 | Layout, Field | 布局语法 |

### 启用/禁用插件

编辑 `edgeviewer.ini` 的 `[Markdown]` section：

```ini
[Markdown]
; 设置为 0 禁用，1 启用 (默认启用)
EnableAbbr=1
EnableAlert=1
EnableKatex=0       ; 禁用 KaTeX
EnableTab=0         ; 禁用标签页
```

### 添加新插件

详见 `Resources/assets/markdown-it/PLUGIN_FRAMEWORK.md`

## Fine Tuning

Plugin configuration is stored in the `edgeviewer.ini` file, located in the plugin folder.

## Setup

Binary plugin archives come with the setup script. Just enter the archive, and confirm installation.

## Development

[MS Visual Studio 2022](https://visualstudio.microsoft.com/) and [vcpkg](https://vcpkg.io) with MSBuild integration are required. Run `BuildMakeSetup.bat` from `MSVS Development Command Prompt` to build the project.

## 从 marked.js 迁移

本项目已从 marked.js 迁移到 markdown-it。主要变化：

- 使用 markdown-it 作为 Markdown 解析器
- 使用 mdit-plugins 框架管理扩展插件
- 保留 MathJax 和 Mermaid 兼容
- 所有 CSS 主题保持不变

如需回退到 marked.js，可参考 Git 历史中的旧版 `loader.html` 和 `markdownExtension.js`。
