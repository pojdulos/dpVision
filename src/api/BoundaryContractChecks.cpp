#include "adapters/PluginHostAPIAdapter.h"
#include "adapters/PluginHostGuiAPIAdapter.h"
#include "interfaces/IGuiInternalsAPI.h"
#include "interfaces/IPluginGuiAPI.h"
#include "interfaces/IPluginHostAPI.h"
#include "interfaces/IPluginPanelAPI.h"

#include <type_traits>
#include <utility>

namespace {

template <typename T, typename = void>
struct has_dock_workspace : std::false_type {};

template <typename T>
struct has_dock_workspace<T, std::void_t<decltype(std::declval<T&>().dockWorkspace())>> : std::true_type {};

template <typename T, typename = void>
struct has_dock_histogram : std::false_type {};

template <typename T>
struct has_dock_histogram<T, std::void_t<decltype(std::declval<T&>().dockHistogram())>> : std::true_type {};

template <typename T, typename = void>
struct has_gui_internals : std::false_type {};

template <typename T>
struct has_gui_internals<T, std::void_t<decltype(std::declval<T&>().guiInternals())>> : std::true_type {};

template <typename T, typename = void>
struct has_plugin_panel : std::false_type {};

template <typename T>
struct has_plugin_panel<T, std::void_t<decltype(std::declval<T&>().pluginPanel())>> : std::true_type {};

template <typename T, typename = void>
struct has_camera : std::false_type {};

template <typename T>
struct has_camera<T, std::void_t<decltype(std::declval<T&>().camera())>> : std::true_type {};

template <typename T, typename = void>
struct has_progress : std::false_type {};

template <typename T>
struct has_progress<T, std::void_t<decltype(std::declval<T&>().progress())>> : std::true_type {};

template <typename T, typename = void>
struct has_current_viewer : std::false_type {};

template <typename T>
struct has_current_viewer<T, std::void_t<decltype(std::declval<T&>().currentViewer())>> : std::true_type {};

template <typename T, typename = void>
struct has_panel : std::false_type {};

template <typename T>
struct has_panel<T, std::void_t<decltype(std::declval<T&>().panel(0u))>> : std::true_type {};

template <typename T, typename = void>
struct has_add_button : std::false_type {};

template <typename T>
struct has_add_button<T, std::void_t<decltype(std::declval<T&>().addButton(0u, QString(), QString(), 0, 0))>> : std::true_type {};

static_assert(!has_dock_workspace<IPluginHostAPI>::value, "IPluginHostAPI must not expose dock workspace access.");
static_assert(!has_dock_histogram<IPluginHostAPI>::value, "IPluginHostAPI must not expose dock histogram access.");
static_assert(!has_gui_internals<IPluginHostAPI>::value, "IPluginHostAPI must not expose GUI internals.");
static_assert(!has_plugin_panel<IPluginHostAPI>::value, "IPluginHostAPI must not expose plugin panel access.");
static_assert(!has_camera<IPluginHostAPI>::value, "IPluginHostAPI must not expose camera control.");
static_assert(!has_progress<IPluginHostAPI>::value, "IPluginHostAPI must not expose GUI progress control.");

static_assert(!has_current_viewer<IPluginHostAPI>::value, "IPluginHostAPI must not expose GLViewer* access.");
static_assert(!has_panel<IPluginHostAPI>::value, "IPluginHostAPI must not expose QWidget* panel access.");
static_assert(!has_add_button<IPluginHostAPI>::value, "IPluginHostAPI must not expose QPushButton* access.");

static_assert(has_dock_workspace<IPluginGuiAPI>::value, "IPluginGuiAPI must remain the privileged entry point for workspace dock APIs.");
static_assert(has_dock_histogram<IPluginGuiAPI>::value, "IPluginGuiAPI must remain the privileged entry point for histogram dock APIs.");
static_assert(has_gui_internals<IPluginGuiAPI>::value, "IPluginGuiAPI must expose GUI internals for privileged plugins.");
static_assert(has_plugin_panel<IPluginGuiAPI>::value, "IPluginGuiAPI must expose plugin panel APIs for privileged plugins.");

static_assert(std::is_base_of_v<IPluginHostAPI, PluginHostAPIAdapter>, "PluginHostAPIAdapter must implement IPluginHostAPI.");
static_assert(!std::is_base_of_v<IPluginGuiAPI, PluginHostAPIAdapter>, "PluginHostAPIAdapter must remain host-only.");
static_assert(std::is_base_of_v<IPluginGuiAPI, PluginHostGuiAPIAdapter>, "PluginHostGuiAPIAdapter must remain the GUI-capable adapter.");

static_assert(std::is_same_v<decltype(std::declval<IGuiInternalsAPI&>().currentViewer()), GLViewer*>,
    "Privileged raw viewer access must stay behind IGuiInternalsAPI.");
static_assert(std::is_same_v<decltype(std::declval<IPluginPanelAPI&>().panel(0u)), QWidget*>,
    "Privileged raw QWidget access must stay behind IPluginPanelAPI.");
static_assert(std::is_same_v<decltype(std::declval<IPluginPanelAPI&>().addButton(0u, QString(), QString(), 0, 0)), QPushButton*>,
    "Privileged raw QPushButton access must stay behind IPluginPanelAPI.");

} // namespace
