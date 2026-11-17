# Plugin Author Guide

Ten dokument opisuje wspierane wejścia do API dla autora pluginu.

## Zasada ogólna

Dla zwykłego pluginu używaj:

- `AP::`
- `UI::`

To jest wspierana publiczna powierzchnia pluginów. Ma być krótka, prosta i
stabilna, nawet jeśli mechanizm pod spodem będzie dalej porządkowany.

## Kiedy używać `AP::`

Używaj `AP::`, gdy plugin pracuje na danych i stanie aplikacji, na przykład:

- obiekty i modele
- workspace
- selekcja
- import / load / duplicate
- ścieżki aplikacji i ustawienia aplikacyjne

Przykłady:

```cpp
auto model = AP::WORKSPACE::getCurrentModel();
auto selected = AP::WORKSPACE::SELECTION::objects();
AP::WORKSPACE::setCurrentModel(id);
```

## Kiedy używać `UI::`

Używaj `UI::`, gdy plugin potrzebuje wspieranych operacji UI, na przykład:

- plugin panel
- progress bar
- status bar
- message box
- file chooser
- podstawowe operacje kamery
- odświeżenie docków / widoków przez legacy wrappers

Przykłady:

```cpp
UI::PLUGINPANEL::create(m_ID, "Example");
UI::PLUGINPANEL::addButton(m_ID, "run", "Run", 0, 0);

UI::PROGRESSBAR::init(0, 100, 0);
UI::PROGRESSBAR::setValue(50);
UI::PROGRESSBAR::hide();

UI::STATUSBAR::setText("Working...");
UI::MESSAGEBOX::warning("Something happened");
```

## Szerszy dostęp do GUI

Jeśli plugin świadomie potrzebuje szerszego dostępu do GUI, używaj jawnego
wejścia GUI-aware:

- `IPluginGuiAPI`
- `PluginHostGuiAPIAdapter`

To jest ścieżka dla pluginów, które naprawdę potrzebują dostępu do:

- dock workspace
- dock histogram
- gui internals
- app internals
- plugin panel jako usługa interfejsowa
- węższych kontraktów kamery i progressu

Przykład:

```cpp
#include "../api/adapters/PluginHostGuiAPIAdapter.h"

namespace {
PluginHostGuiAPIAdapter& guiApi()
{
    static PluginHostGuiAPIAdapter api;
    return api;
}
}

void rebuildWorkspace()
{
    guiApi().dockWorkspace().rebuildTree();
}
```

## Czego nie traktować jako publicznego kontraktu

Nie buduj nowego pluginu na:

- prywatnych nagłówkach z `src/gui`
- bezpośrednich widgetach Qt hosta
- helperach, które istnieją tylko dla wygody wywołań

W szczególności:

- `PluginGuiAPIAdapter` jest tylko convenience wrapperem
- można go spotkać w kodzie, ale nie jest promowany jako docelowy stabilny
  kontrakt publiczny

Jeśli potrzebujesz szerszego GUI API, wybieraj raczej:

- `PluginHostGuiAPIAdapter`
- `IPluginGuiAPI`

## Praktyczne reguły

- Dla zwykłego pluginu zacznij od `AP::` i `UI::`.
- Jeśli to wystarcza, nie przechodź na szerszy host GUI.
- Nie includuj `src/gui/*.h` w pluginie, chyba że świadomie wchodzisz w
  niewspieraną, host-specyficzną integrację.
- Jeśli stary plugin używa `UI::` lub `AP::`, to jest akceptowalna ścieżka.
- Jeśli plugin jest budowany poza głównym CMake, `UI::` i `AP::` są zwykle
  bezpieczniejszym wyborem niż adaptery zależne od pełnego kontekstu GUI.

## Jak wybierać ścieżkę

1. Potrzebujesz danych, modeli, workspace lub selekcji:
   użyj `AP::`
2. Potrzebujesz wspieranych operacji UI:
   użyj `UI::`
3. Potrzebujesz świadomie szerszego GUI:
   użyj `PluginHostGuiAPIAdapter` / `IPluginGuiAPI`
4. Jeśli masz wątpliwość:
   zostań przy `AP::` / `UI::`

## Przykładowy plugin

W repozytorium dostępny jest w pełni działający przykład:

- `src/plugins/example/SamplePlugin.cpp`

Pokazuje on:
- jak zarejestrować plugin i obsługiwać zdarzenia (`onLoad`, `onButton`)
- jak używać `AP::WORKSPACE` i `AP::OBJECT` do operacji na danych hosta
- jak używać `PluginHostGuiAPIAdapter` do panelu pluginu, dialogów i komunikatów
- jak połączyć publiczną ścieżkę danych z jawnie GUI-aware hostem

Uwaga:

- ten przykład pokazuje działający wzorzec dla pluginu in-tree, który używa
  danych hosta przez `AP::` i operacji GUI przez `PluginHostGuiAPIAdapter`
- dla prostszego pluginu nadal preferowanym punktem startowym są `AP::` i `UI::`
- jeśli potrzebujesz szerszego GUI-aware hosta, przejdź na
  `PluginHostGuiAPIAdapter` / `IPluginGuiAPI`
