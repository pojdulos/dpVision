# Camera / Viewer Model Notes

This document captures a possible future camera/view architecture for dpVision.

It is intentionally a design note, not a committed migration plan.

Current status:
- the application currently treats camera operations mainly as operations on
  the active GUI viewer
- `ICameraControlAPI` represents control over the current viewer camera
- raw viewer access remains available only through privileged GUI internals

This note records a more explicit model that may become useful later if camera
state needs to exist independently of a concrete viewer widget.

## Motivation

The current code base mixes two concerns:
- camera state and geometry
- concrete viewer/window ownership and interaction

That works for the current Qt/MDI application, but it becomes limiting when:
- multiple viewers each have their own camera
- the same logical camera should be reused across viewers
- geometry or projection calculations should run without a concrete widget
- camera presets/bookmarks/snapshots should be stored as domain objects
- future hosts may want a different UI while keeping the same scene/view model

The proposed model separates:
- domain camera state
- viewer/viewport implementation
- host policy deciding which viewer uses which camera

## Core Concept

Future direction:
- a `Camera` is a first-class object that may exist independently of any
  `GLViewer`
- a viewer does not own the camera semantically; it is bound to one camera
- many cameras may exist at once
- different viewers may be bound to different cameras
- multiple viewers may also share the same camera if synchronized behavior is
  desired

This means the system no longer assumes:
- one implicit global camera
- one active camera equal to one active viewer transform

Instead, the model becomes:
- scene objects exist in world space
- cameras exist as view definitions
- viewers render the scene through a chosen camera

## Camera As A Domain Object

A future domain-level `Camera` should describe view state only.

Likely responsibilities:
- position
- orientation / rotation
- look direction or target
- up vector or equivalent orientation basis
- field of view
- projection mode if applicable
- near/far clipping planes
- scale / zoom semantics if those are part of the current application model
- conversion to and from a stable transform representation

Possible optional responsibilities:
- camera label / name
- persistence key
- bookmark/preset metadata
- visibility or activation flags if cameras become scene entities
- association with workspace or scene nodes if the application wants cameras to
  behave like other scene objects

Things a domain camera should not know:
- Qt widgets
- `GLViewer`
- active MDI subwindow
- repaint scheduling
- screen pixels of a concrete widget
- screenshot file handling
- user input events

## Viewer As A Host/UI Object

A viewer remains a host-side object.

Viewer responsibilities should include:
- widget/window ownership
- OpenGL or other rendering backend integration
- viewport size and pixel geometry
- active/focused window selection
- input handling
- repaint/update scheduling
- screenshot capture of a concrete rendered surface
- local overlay rendering, selection visuals, and viewer-local interaction
- binding to one chosen camera

The viewer should consume camera state, not define the whole concept of camera.

## Explicit Separation: Camera vs Viewport

One practical rule:
- camera defines how the scene is viewed conceptually
- viewport defines where and how that view is displayed concretely

`Camera`-like concerns:
- pose
- projection parameters
- view transform
- projection transform independent of a widget

`Viewport`/viewer concerns:
- widget size
- DPI / scaling
- mouse position in widget coordinates
- framebuffer content
- screen-space conversions that require concrete window geometry

This distinction matters because some operations that sound like "camera
operations" are actually viewport-dependent.

## What Could Move To Core

If the future camera model is introduced, the following kinds of logic are good
candidates for `core`:
- mathematical camera state representation
- camera interpolation
- camera presets/bookmarks
- geometric calculations based on a supplied camera state
- projection math that only needs projection parameters and a supplied viewport
  description
- frustum construction and related intersection logic
- view matrices / projection matrices derived from abstract camera state
- serialization and deserialization of camera definitions
- explicit relations between scene objects and cameras if cameras become part of
  the scene model

Important condition:
- these calculations must take explicit input data such as `CameraState`,
  `ViewportDesc`, and scene geometry
- they should not fetch "the active viewer" implicitly

## What Should Stay In GUI / Host

The following should remain host/UI responsibilities even if cameras become
domain objects:
- deciding which viewer is active
- deciding which camera a viewer currently uses
- creating and destroying viewer widgets
- reacting to mouse/keyboard gestures
- screenshots of real windows
- conversions that depend on the current widget and framebuffer state
- window focus and MDI policy
- viewer-local temporary state that should not become scene/domain state

## Multiple Cameras And Multiple Viewers

This model becomes especially useful because the application already supports
multiple viewer windows.

In that world:
- there is no single global camera in the strict sense
- there are many camera instances
- there are many viewer instances
- host policy binds viewers to cameras

Possible binding patterns:
- one viewer uses one unique camera
- multiple viewers use the same camera for synchronized views
- a viewer may switch from one camera to another
- a camera may exist without any viewer currently bound to it

That last case is important:
- it allows storing or editing cameras without displaying them immediately

## Active Camera vs Active Viewer

The host may still expose an "active camera" concept, but it should be treated
as a derived convenience:
- active viewer is a GUI/host concept
- active camera is usually "camera bound to the active viewer"

This is useful for interactive commands, but it should not be the only model.

The architecture should avoid making "current viewer transform" the only source
of truth for all camera state.

## Suggested Future Types

The exact names are not important, but a clean split could look like:

- `CameraState`
  - pure data or near-pure data
  - stable mathematical description of a camera

- `ViewportDesc`
  - width, height, maybe pixel ratio
  - only the minimal geometry needed for calculations

- `CameraMath` or equivalent utilities
  - view/projection matrix generation
  - screen/world/ray math that depends only on explicit inputs

- `ViewBinding` or host-side equivalent
  - relation between a viewer instance and a camera instance

- `CameraRepository` / scene-managed camera collection
  - optional, only if cameras become real scene/workspace entities

## Scene Integration Option

One valid direction is to treat cameras as scene/workspace elements.

Potential benefits:
- cameras can be saved with the scene
- camera presets become explicit scene data
- plugins can enumerate and manage cameras intentionally
- switching viewers between cameras becomes a normal scene-host operation

Potential costs:
- introduces lifecycle, ownership, and serialization questions
- requires deciding whether cameras have IDs, labels, parents, transforms, and
  selection semantics like other objects
- may force UI and plugin expectations to become more explicit

This should be a conscious design choice, not an accidental side effect.

## API Implications

If this model is introduced later, the API likely needs two distinct layers:

1. Safe camera/domain API
- operate on explicit camera objects or camera states
- no raw widget/viewer pointers
- no dependence on "current viewer" unless explicitly chosen by the caller

2. Privileged viewer/GUI API
- current viewer access
- screenshot of a concrete viewer
- raw viewer pointer or transform escape hatches if they still must exist
- binding/switching viewer-camera relations in the host

This would preserve a clear rule:
- camera semantics are not the same as viewer internals

## Backward-Compatibility Notes

The current plugin-facing behavior is still centered around the active viewer.

That means a future migration should preserve legacy semantics carefully:
- legacy `UI::CAMERA::*` can keep meaning "operate on the current active
  viewer camera"
- new APIs may operate on explicit cameras or explicit viewer bindings
- legacy compatibility wrappers can map current-viewer behavior onto the new
  model

This is important because the future model should improve structure without
changing plugin behavior accidentally.

## Practical Migration Thoughts

This is not a near-term task, but if revisited later, a safe order would be:

1. Introduce explicit data structures for camera state without changing viewer
   ownership yet.
2. Move reusable camera math into `core` using explicit inputs.
3. Audit code paths that currently depend on `currentViewer()` or
   `viewer->transform()`.
4. Introduce a host-side binding layer from viewers to cameras.
5. Only then consider whether cameras should become workspace/scene entities.

This order matters because it avoids mixing a data-model change with a GUI
ownership rewrite in one step.

## Decision Summary

The key architectural idea is:
- camera should be allowed to exist independently of a viewer
- viewer should be treated as a host-side presentation object bound to a camera

At the same time:
- not every current "camera" method belongs in a future domain camera
- many operations still belong to host/viewer because they depend on a concrete
  window, viewport, or render surface

So the clean future model is not:
- "move current viewer camera code into core as-is"

It is:
- "separate domain camera state from viewer ownership, then move only the
  domain-valid parts into core"
