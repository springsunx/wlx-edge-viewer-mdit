/**
 * mdit-plugin-registry.js
 * ========================
 * markdown-it 插件注册表框架
 * 
 * 设计目标:
 * 1. 声明式插件定义 - 添加新插件只需一条配置
 * 2. 运行时启用/禁用 - 通过 INI 配置控制
 * 3. 依赖管理 - 自动处理加载顺序
 * 4. CSS 按需加载 - 只加载启用插件的样式
 * 
 * 使用方法:
 * - 添加插件: 在 PLUGINS 定义中添加一条记录
 * - 禁用插件: 在 edgeviewer.ini 的 [Markdown] 中设置 Enable<Name>=0
 * - 自定义插件: 在 CUSTOM_PLUGINS 中添加本地插件
 */

(function(window) {
    'use strict';

    // =====================================================================
    // 插件定义区 - 在此添加新插件
    // =====================================================================
    
    /**
     * 内置插件定义
     * 每个插件定义包含:
     * @param {string} name      - 插件唯一标识 (对应 INI 中的 Enable<Name>)
     * @param {string} globalVar - UMD 全局变量名
     * @param {string} exportKey - 从全局变量中取出插件的 key
     * @param {string} jsFile    - UMD 文件路径 (相对于 mdit-plugins 目录)
     * @param {string|null} cssFile - CSS 文件路径 (相对于 markdown 目录), null 表示无
     * @param {string|null} cssId   - CSS link 元素的 ID, 用于去重
     * @param {Function|null} initFn - 初始化函数 (接收 md 实例), null 表示无需初始化
     * @param {Array<string>} deps   - 依赖的其他插件 name 列表
     * @param {string} description   - 插件描述 (用于文档/调试)
     */
    const PLUGINS = [
        // ---- 文本格式化 ----
        {
            name: 'Abbr',
            globalVar: 'mdItPluginAbbr',
            exportKey: 'abbr',
            jsFile: 'mdit-plugin-abbr.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '缩写定义 *[HTML]: Hyper Text Markup Language'
        },
        {
            name: 'Mark',
            globalVar: 'mdItPluginMark',
            exportKey: 'mark',
            jsFile: 'mdit-plugin-mark.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '高亮标记 ==marked text=='
        },
        {
            name: 'Sub',
            globalVar: 'mdItPluginSub',
            exportKey: 'sub',
            jsFile: 'mdit-plugin-sub.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '下标 H~2~O'
        },
        {
            name: 'Sup',
            globalVar: 'mdItPluginSup',
            exportKey: 'sup',
            jsFile: 'mdit-plugin-sup.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '上标 X^2^'
        },
        {
            name: 'Ins',
            globalVar: 'mdItPluginIns',
            exportKey: 'ins',
            jsFile: 'mdit-plugin-ins.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '插入文本 ++inserted++'
        },
        {
            name: 'Ruby',
            globalVar: 'mdItPluginRuby',
            exportKey: 'ruby',
            jsFile: 'mdit-plugin-ruby.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '注音标注 {ruby base}(annotation)'
        },

        // ---- 块级元素 ----
        {
            name: 'Alert',
            globalVar: 'mdItPluginAlert',
            exportKey: 'alert',
            jsFile: 'mdit-plugin-alert.umd.js',
            cssFile: 'mdit-plugin-alert.css',
            cssId: 'mdit-css-alert',
            initFn: null,
            deps: [],
            description: 'GitHub 风格告警 > [!NOTE] / [!TIP] / [!WARNING] / [!CAUTION] / [!IMPORTANT]'
        },
        {
            name: 'Container',
            globalVar: 'mdItPluginContainer',
            exportKey: 'container',
            jsFile: 'mdit-plugin-container.umd.js',
            cssFile: null,
            cssId: null,
            initFn: function(md) {
                // 默认注册常用容器类型
                var containerPlugin = window.mdItPluginContainer.container;
                md.use(containerPlugin, 'tip', { validate: function(p) { return p.trim().startsWith('tip'); }, });
                md.use(containerPlugin, 'warning', { validate: function(p) { return p.trim().startsWith('warning'); } });
                md.use(containerPlugin, 'danger', { validate: function(p) { return p.trim().startsWith('danger'); } });
                md.use(containerPlugin, 'info', { validate: function(p) { return p.trim().startsWith('info'); } });
                md.use(containerPlugin, 'details', { validate: function(p) { return p.trim().startsWith('details'); } });
            },
            deps: [],
            description: '自定义容器 ::: tip / warning / danger / info / details'
        },
        {
            name: 'Dl',
            globalVar: 'mdItPluginDl',
            exportKey: 'dl',
            jsFile: 'mdit-plugin-dl.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '定义列表 Term\n: Definition'
        },
        {
            name: 'Footnote',
            globalVar: 'mdItPluginFootnote',
            exportKey: 'footnote',
            jsFile: 'mdit-plugin-footnote.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '脚注 text[^1] 和 [^1]: footnote content'
        },
        {
            name: 'Tasklist',
            globalVar: 'mdItPluginTasklist',
            exportKey: 'tasklist',
            jsFile: 'mdit-plugin-tasklist.umd.js',
            cssFile: 'mdit-plugin-tasklist.css',
            cssId: 'mdit-css-tasklist',
            initFn: null,
            deps: [],
            description: '任务列表 - [x] done / - [ ] todo'
        },
        {
            name: 'Align',
            globalVar: 'mdItPluginAlign',
            exportKey: 'align',
            jsFile: 'mdit-plugin-align.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '文本对齐 ->= <-= <->='
        },
        {
            name: 'Tab',
            globalVar: 'mdItPluginTab',
            exportKey: 'tab',
            jsFile: 'mdit-plugin-tab.umd.js',
            cssFile: 'mdit-plugin-tab.css',
            cssId: 'mdit-css-tab',
            initFn: null,
            deps: [],
            description: '标签页 ::: tabs / tab'
        },

        // ---- 图片与媒体 ----
        {
            name: 'Figure',
            globalVar: 'mdItPluginFigure',
            exportKey: 'figure',
            jsFile: 'mdit-plugin-figure.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '图片自动转 <figure>'
        },
        {
            name: 'ImgSize',
            globalVar: 'mdItPluginImgSize',
            exportKey: 'imgSize',
            jsFile: 'mdit-plugin-img-size.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '图片尺寸 ![alt](url =100x200)'
        },
        {
            name: 'ImgLazyload',
            globalVar: 'mdItPluginImgLazyload',
            exportKey: 'imgLazyload',
            jsFile: 'mdit-plugin-img-lazyload.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '图片懒加载 loading="lazy"'
        },
        {
            name: 'ImgMark',
            globalVar: 'mdItPluginImgMark',
            exportKey: 'imgMark',
            jsFile: 'mdit-plugin-img-mark.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '图片标记语法'
        },

        // ---- 属性与样式 ----
        {
            name: 'Attrs',
            globalVar: 'mdItPluginAttrs',
            exportKey: 'attrs',
            jsFile: 'mdit-plugin-attrs.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '属性语法 {.class #id key=value}'
        },
        {
            name: 'Stylize',
            globalVar: 'mdItPluginStylize',
            exportKey: 'stylize',
            jsFile: 'mdit-plugin-stylize.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: ['Attrs'],
            description: '自定义内联样式'
        },
        {
            name: 'Spoiler',
            globalVar: 'mdItPluginSpoiler',
            exportKey: 'spoiler',
            jsFile: 'mdit-plugin-spoiler.umd.js',
            cssFile: 'mdit-plugin-spoiler.css',
            cssId: 'mdit-css-spoiler',
            initFn: null,
            deps: [],
            description: '隐藏文本 ||spoiler||'
        },

        // ---- 图表与代码 ----
        {
            name: 'Uml',
            globalVar: 'mdItPluginUml',
            exportKey: 'uml',
            jsFile: 'mdit-plugin-uml.umd.js',
            cssFile: null,
            cssId: null,
            initFn: function(md) {
                // 使用 fence 规则渲染 UML (保留 mermaid 兼容)
                var umlPlugin = window.mdItPluginUml.uml;
                md.use(umlPlugin, {
                    name: 'mermaid',
                    open: 'mermaid',
                    close: 'mermaid'
                });
            },
            deps: [],
            description: 'UML/Mermaid 图表 ```mermaid'
        },
        {
            name: 'Plantuml',
            globalVar: 'mdItPluginPlantuml',
            exportKey: 'plantuml',
            jsFile: 'mdit-plugin-plantuml.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: 'PlantUML 图表 @startuml / @enduml'
        },

        // ---- 数学公式 ----
        {
            name: 'Katex',
            globalVar: 'mdItPluginKatex',
            exportKey: 'katex',
            jsFile: 'mdit-plugin-katex.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: 'KaTeX 数学公式 $...$ 和 $$...$$'
        },

        // ---- 嵌入与链接 ----
        {
            name: 'Embed',
            globalVar: 'mdItPluginEmbed',
            exportKey: 'embed',
            jsFile: 'mdit-plugin-embed.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '嵌入语法 @[type](id)'
        },
        {
            name: 'Icon',
            globalVar: 'mdItPluginIcon',
            exportKey: 'icon',
            jsFile: 'mdit-plugin-icon.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '图标语法 :icon-name:'
        },
        {
            name: 'Emoji',
            globalVar: 'mdItPluginEmoji',
            exportKey: 'emoji',
            jsFile: 'mdit-plugin-emoji.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: 'Emoji :smile: :heart:'
        },

        // ---- 布局 ----
        {
            name: 'Layout',
            globalVar: 'mdItPluginLayout',
            exportKey: 'layout',
            jsFile: 'mdit-plugin-layout.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '布局语法'
        },
        {
            name: 'Field',
            globalVar: 'mdItPluginField',
            exportKey: 'field',
            jsFile: 'mdit-plugin-field.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '字段语法'
        },

        // ---- 开发/演示 ----
        {
            name: 'Demo',
            globalVar: 'mdItPluginDemo',
            exportKey: 'demo',
            jsFile: 'mdit-plugin-demo.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '演示块语法'
        },
        {
            name: 'InlineRule',
            globalVar: 'mdItPluginInlineRule',
            exportKey: 'inlineRule',
            jsFile: 'mdit-plugin-inline-rule.umd.js',
            cssFile: null,
            cssId: null,
            initFn: null,
            deps: [],
            description: '自定义内联规则工具'
        }
    ];

    // =====================================================================
    // 自定义插件区 - 用户可在此添加本地/自定义插件
    // =====================================================================
    
    /**
     * 自定义插件定义 (格式同 PLUGINS)
     * 
     * 示例:
     * const CUSTOM_PLUGINS = [
     *     {
     *         name: 'MyPlugin',
     *         globalVar: 'myPluginGlobal',
     *         exportKey: 'default',
     *         jsFile: 'my-plugin.umd.js',     // 放在 mdit-plugins 目录
     *         cssFile: 'my-plugin.css',       // 放在 markdown 目录
     *         cssId: 'mdit-css-myplugin',
     *         initFn: null,
     *         deps: [],
     *         description: '我的自定义插件'
     *     }
     * ];
     */
    const CUSTOM_PLUGINS = [];

    // =====================================================================
    // 框架核心 (一般不需要修改)
    // =====================================================================

    const allPlugins = PLUGINS.concat(CUSTOM_PLUGINS);
    const pluginMap = {};
    allPlugins.forEach(function(p) { pluginMap[p.name] = p; });

    /**
     * 加载 CSS 文件 (幂等, 不会重复加载)
     */
    function loadCSS(cssFile, cssId) {
        if (!cssFile) return;
        if (cssId && document.getElementById(cssId)) return;
        
        var link = document.createElement('link');
        link.rel = 'stylesheet';
        link.href = 'http://assets.example/markdown/' + cssFile;
        if (cssId) link.id = cssId;
        document.head.appendChild(link);
    }

    /**
     * 拓扑排序 - 处理插件依赖顺序
     */
    function topologicalSort(pluginNames) {
        var visited = {};
        var result = [];
        var visiting = {};

        function visit(name) {
            if (visited[name]) return;
            if (visiting[name]) {
                console.warn('[mdit-registry] Circular dependency detected:', name);
                return;
            }
            
            visiting[name] = true;
            var plugin = pluginMap[name];
            if (plugin && plugin.deps) {
                plugin.deps.forEach(function(dep) {
                    if (pluginNames.indexOf(dep) !== -1) {
                        visit(dep);
                    }
                });
            }
            visiting[name] = false;
            visited[name] = true;
            result.push(name);
        }

        pluginNames.forEach(visit);
        return result;
    }

    /**
     * 注册所有启用的插件到 markdown-it 实例
     * 
     * @param {Object} md - markdown-it 实例
     * @param {Object} config - 插件启用配置 { EnableAbbr: true, EnableAlert: false, ... }
     * @returns {Object} { registered: string[], skipped: string[], errors: string[] }
     */
    function registerPlugins(md, config) {
        config = config || {};
        
        // 确定启用的插件列表
        var enabledNames = [];
        allPlugins.forEach(function(plugin) {
            var configKey = 'Enable' + plugin.name;
            // 默认启用, 除非显式设置为 false/0
            var enabled = config[configKey] !== false && config[configKey] !== '0' && config[configKey] !== 0;
            if (enabled) {
                enabledNames.push(plugin.name);
            }
        });

        // 按依赖顺序排序
        var sortedNames = topologicalSort(enabledNames);
        
        var registered = [];
        var skipped = [];
        var errors = [];

        sortedNames.forEach(function(name) {
            var plugin = pluginMap[name];
            if (!plugin) {
                errors.push('Unknown plugin: ' + name);
                return;
            }

            try {
                // 检查全局变量是否存在
                var globalObj = window[plugin.globalVar];
                if (!globalObj) {
                    skipped.push(name + ' (global variable ' + plugin.globalVar + ' not found)');
                    return;
                }

                // 获取插件函数
                var pluginFn = globalObj[plugin.exportKey];
                if (!pluginFn) {
                    skipped.push(name + ' (export key ' + plugin.exportKey + ' not found)');
                    return;
                }

                // 加载 CSS
                loadCSS(plugin.cssFile, plugin.cssId);

                // 注册插件
                if (plugin.initFn) {
                    // 使用自定义初始化函数
                    plugin.initFn(md);
                } else {
                    // 直接注册
                    md.use(pluginFn);
                }

                registered.push(name);
            } catch (e) {
                errors.push(name + ': ' + e.message);
                console.error('[mdit-registry] Error registering plugin ' + name + ':', e);
            }
        });

        return {
            registered: registered,
            skipped: skipped,
            errors: errors
        };
    }

    /**
     * 获取所有已定义的插件信息
     */
    function getPluginInfo() {
        return allPlugins.map(function(p) {
            return {
                name: p.name,
                description: p.description,
                hasCSS: !!p.cssFile,
                deps: p.deps
            };
        });
    }

    /**
     * 获取特定插件的定义
     */
    function getPlugin(name) {
        return pluginMap[name] || null;
    }

    // =====================================================================
    // 导出 API
    // =====================================================================
    
    window.MditPluginRegistry = {
        registerPlugins: registerPlugins,
        getPluginInfo: getPluginInfo,
        getPlugin: getPlugin,
        loadCSS: loadCSS,
        ALL_PLUGINS: allPlugins
    };

})(window);
