#include "MdProcessor.h"
#include "mini/ini.h"
#include <regex>
//------------------------------------------------------------------------
namespace { MdProcessor md; }
//------------------------------------------------------------------------
bool MdProcessor::InitPath(const fs::path& path)
{
	mPath = GetPhysicalPath(path);
	return isType(path.extension(), "Markdown");
}
//------------------------------------------------------------------------
// 构建插件配置 JSON 字符串
// 格式: {"EnableAbbr":true,"EnableAlert":false,...}
//------------------------------------------------------------------------
std::string BuildPluginConfig()
{
	const auto& mdIni = GlobalSettings().get("Markdown");
	
	// 所有支持的插件名称 (与 mditPluginRegistry.js 中的 name 对应)
	static const char* pluginNames[] = {
		"Abbr", "Alert", "Align", "Attrs", "Container", "Demo", "Dl",
		"Embed", "Emoji", "Field", "Figure", "Footnote", "Icon",
		"ImgLazyload", "ImgMark", "ImgSize", "InlineRule", "Ins",
		"Katex", "Layout", "Mark", "Plantuml", "Ruby", "Spoiler",
		"Stylize", "Sub", "Sup", "Tab", "Tasklist", "Uml"
	};
	
	std::string config = "{";
	bool first = true;
	
	for (const auto& name : pluginNames)
	{
		if (!first) config += ",";
		std::string key = "Enable" + std::string(name);
		
		// 默认启用 (除非显式设置为 0)
		bool enabled = true;
		if (mdIni.has(key))
		{
			enabled = (mdIni.get(key) != "0");
		}
		
		config += "\"" + key + "\":" + (enabled ? "true" : "false");
		first = false;
	}
	config += "}";
	
	return config;
}
//------------------------------------------------------------------------
void MdProcessor::OpenIn(ViewPtr webView) const
{
	mapDomains(webView, mPath.root_path());

	const auto& mdIni = GlobalSettings().get("Markdown");
	const auto cssFile = gs_IsDarkMode ? mdIni.get("CSSDark") : mdIni.get("CSS");
	
	// 构建插件配置
	const std::string pluginConfig = BuildPluginConfig();
	
	std::wstring wloader(to_utf16(ReadFile(assetsPath() / L"markdown" / L"loader.html")));
	wloader = replacePlaceholders(wloader, {
		{L"__BASE_URL__", urlPathW(mPath.parent_path().relative_path())},
		{L"__CSS_NAME__", to_utf16(cssFile)},
		{L"__MD_FILENAME__", urlPathW(mPath.relative_path())},
		{L"__PLUGIN_CONFIG__", to_utf16(pluginConfig)}
	});

	webView->NavigateToString(wloader.c_str());
}
//------------------------------------------------------------------------
