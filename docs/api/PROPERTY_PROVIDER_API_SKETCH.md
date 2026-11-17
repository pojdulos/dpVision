# Property Provider API Sketch

This note records the current state of property-widget integration and the
intended architectural classification.

## Current State

The system already supports GUI-aware property widgets through object-level
virtual methods such as:

- `has_prop_widget()`
- `prop_widget()`
- `prop_widget_update()`

The host GUI detects and uses that mechanism automatically when showing
properties for the selected object.

For deliberately GUI-aware plugins, this is a working extension mechanism.

## Architectural Classification

This mechanism should be treated as:

- a privileged GUI extension path
- owned by the GUI-side properties system
- intentionally unavailable in the default-safe plugin path
- not something legacy/default plugin APIs need to expose

In other words:

- the `Properties` dock remains GUI-owned
- a plugin that consciously opts into GUI access may provide a widget for its
  object type
- the host GUI decides when and where that widget is embedded

## What Not To Do

Do not, for now:

- redesign this into a new abstract provider framework just for cleanliness
- force it into the default plugin API
- duplicate it with a second competing property-provider mechanism
- expose direct properties-dock control through legacy wrappers unless a real
  compatibility need appears

The mechanism already works and should first be documented and classified
correctly before any redesign is considered.

## When a Redesign Would Be Justified

A more formal provider registry or a cleaner API would make sense only if a real
need appears, for example:

- multiple competing property providers per object type
- plugin lifetime and registration problems
- inability to scope or prioritize providers
- need to support richer composition than one object-supplied widget
- need to offer a non-widget property model for default-safe plugins

Until such a limitation appears, the current mechanism should be preserved as an
existing privileged GUI integration path.

## Short Version

The current `prop_widget()` mechanism is not the problem.

It is a deliberate GUI-aware extension point and should be documented as such,
not unnecessarily replaced.
