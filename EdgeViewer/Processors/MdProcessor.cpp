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
// Build plugin config JSON string
// Format: {"EnableAbbr":true,"EnableAlert":false,...}
//------------------------------------------------------------------------
std::string BuildPluginConfig()
{
	const auto& mdIni = GlobalSettings().get("Markdown");
	
	// All supported plugin names (corresponds to mditPluginRegistry.js)
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
		
		// Enabled by default (unless explicitly set to 0)
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
	
	// Build plugin config
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
