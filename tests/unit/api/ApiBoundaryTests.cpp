#include "../../support/TestFramework.h"

#include "../../../src/api/interfaces/IGuiInternalsAPI.h"
#include "../../../src/api/interfaces/IPluginGuiAPI.h"
#include "../../../src/api/interfaces/IPluginHostAPI.h"
#include "../../../src/api/interfaces/IPluginPanelAPI.h"

#include <type_traits>
#include <utility>

namespace {

template <typename T, typename = void>
struct has_gui_internals : std::false_type {};

template <typename T>
struct has_gui_internals<T, std::void_t<decltype(std::declval<T&>().guiInternals())>> : std::true_type {};

template <typename T, typename = void>
struct has_dock_workspace : std::false_type {};

template <typename T>
struct has_dock_workspace<T, std::void_t<decltype(std::declval<T&>().dockWorkspace())>> : std::true_type {};

template <typename T, typename = void>
struct has_panel : std::false_type {};

template <typename T>
struct has_panel<T, std::void_t<decltype(std::declval<T&>().panel(0u))>> : std::true_type {};

template <typename T, typename = void>
struct has_add_button : std::false_type {};

template <typename T>
struct has_add_button<T, std::void_t<decltype(std::declval<T&>().addButton(0u, QString(), QString(), 0, 0))>> : std::true_type {};

} // namespace

TEST_CASE(DefaultHostApiDoesNotExposeGuiSurface)
{
    REQUIRE(!has_gui_internals<IPluginHostAPI>::value);
    REQUIRE(!has_dock_workspace<IPluginHostAPI>::value);
    REQUIRE(!has_panel<IPluginHostAPI>::value);
    REQUIRE(!has_add_button<IPluginHostAPI>::value);
}

TEST_CASE(PrivilegedGuiApiExposesGuiSurface)
{
    REQUIRE(has_gui_internals<IPluginGuiAPI>::value);
    REQUIRE(has_dock_workspace<IPluginGuiAPI>::value);
    REQUIRE((std::is_same_v<decltype(std::declval<IGuiInternalsAPI&>().currentViewer()), GLViewer*>));
    REQUIRE((std::is_same_v<decltype(std::declval<IPluginPanelAPI&>().panel(0u)), QWidget*>));
}

