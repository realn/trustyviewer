
#include "ExtPlugin.h"

namespace realn {
  void ExtPluginList::addPlugin(ExtPluginPtr plugin)
  {
    list.push_back(plugin);
  }

  ExtPluginList::ExtPluginPtr ExtPluginList::getPluginForExt(const QString& ext)
  {
    for (auto& plugin : list) {
      if (plugin->isExtSupported(ext))
        return plugin;
    }

    return nullptr;
  }

  QStringList ExtPluginList::getSupportedExts() const
  {
    auto result = QStringList();
    for (auto& plugin : list) {
      result << plugin->getSupportedExts();
    }
    result.removeDuplicates();
    return result;
  }
}