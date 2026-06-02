#pragma once

#include "ProcessorInterface.h"

// Markdown file:
// Load using markdown-it with mdit-plugins framework
//------------------------------------------------------------------------
class MdProcessor : public ProcessorInterface
{
public:
	virtual bool InitPath(const fs::path& path);
	virtual void OpenIn(ViewPtr webView) const;

private:
	fs::path mPath;
};
//------------------------------------------------------------------------
