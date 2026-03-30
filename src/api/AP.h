#pragma once
#ifndef INTERFEJSAPLIKACJI_H
#define INTERFEJSAPLIKACJI_H

#include "dll_global.h"
#include <string>
#include <list>
#include <set>
#include <memory>
#include "BaseObject.h"
#include "Image.h"

// Forward declarations
class CWorkspace;
class CObject;
class CModel3D;
class CAnnotation;
class CMainApplication;
class CMainWindow;
class QString;

/**
 * @namespace AP
 * @brief Legacy application API kept as a compatibility shim.
 *
 * New code should prefer the explicit interfaces in `src/api/interfaces` and
 * their adapters. `AP` remains for source compatibility and for semantic
 * application operations that have not yet been fully migrated.
 */
namespace AP
{
    /**
     * @brief Legacy compatibility helper for pumping the Qt event loop.
     * @param immediate If true, processes events immediately
     *
     * New code should prefer local UI/application helpers instead of reaching
     * for `AP` as a transport layer to Qt.
     */
    DPVISION_LEGACY_API_EXPORT void processEvents(bool immediate = false);

    /**
     * @brief Legacy compatibility helper returning the executable directory.
     * @return Const reference to QString containing the path
     */
    DPVISION_LEGACY_API_EXPORT const QString& getExeFilePath(void);

    /**
     * @brief Semantic application operation kept as part of the legacy API.
     * @param filePath Path to the file that became current
     */
    DPVISION_LEGACY_API_EXPORT void adjustForCurrentFile(const QString& filePath);

    /**
     * @brief Adds annotation to a parent object (OBSOLETE)
     * @deprecated Use AP::MODEL::addAnnotation(int parentid, CAnnotation *an) instead
     */
    DPVISION_LEGACY_API_EXPORT int addAnnotation(int parentid, std::shared_ptr<CAnnotation> an);

    /**
     * @brief Adds annotation to a 3D model (OBSOLETE)
     * @deprecated Use AP::MODEL::addAnnotation(CModel3D* obj, CAnnotation *an) instead
     */
    DPVISION_LEGACY_API_EXPORT int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an);

    /**
     * @namespace OBJECT
     * @brief Semantic object operations. Safe to keep using during migration.
     */
    namespace OBJECT {
        DPVISION_LEGACY_API_EXPORT bool remove(std::shared_ptr<CBaseObject> obj);
        DPVISION_LEGACY_API_EXPORT bool removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child);

        DPVISION_LEGACY_API_EXPORT int addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child);


        /**
         * @brief Moves an object to a new parent
         * @param obj Object to move
         * @param newParent New parent object
         */
        DPVISION_LEGACY_API_EXPORT void moveTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos = true);

        /**
         * @brief Copies an object to a new parent
         * @param obj Object to copy
         * @param newParent New parent object
         */
        DPVISION_LEGACY_API_EXPORT void copyTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos = true);
    };

    /**
     * @namespace MODEL
     * @brief Semantic model operations. Safe to keep using during migration.
     */
    namespace MODEL {
        /**
         * @brief Loads a 3D model from file
         * @param path Path to the model file
         * @param synchronous If true, loads synchronously
         * @return Pointer to loaded CModel3D
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> load(std::wstring path, bool synchronous = true);

        DPVISION_LEGACY_API_EXPORT int addChild(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> child);
        DPVISION_LEGACY_API_EXPORT int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an);

        DPVISION_LEGACY_API_EXPORT void removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child);
        DPVISION_LEGACY_API_EXPORT void removeChild(int parentid, int childid);
        DPVISION_LEGACY_API_EXPORT void removeAnnotation(int parentid, int id);
    };

    /**
     * @namespace WORKSPACE
     * @brief Semantic workspace operations kept as the main legacy contract.
     *
     * Avoid adding new raw escape-hatch accessors here. Prefer extending
     * `IWorkspaceAPI` when plugins need additional read/write capabilities.
     */
    namespace WORKSPACE {
        /**
         * @brief Returns the number of objects in workspace
         * @return Size of workspace
         */
        DPVISION_LEGACY_API_EXPORT size_t size();

        /**
         * @brief Finds an object by ID
         * @param id Object identifier
         * @return Pointer to CBaseObject or nullptr if not found
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CBaseObject> findId(int id);

        DPVISION_LEGACY_API_EXPORT bool addImage(std::shared_ptr<CImage> im, bool showViewer = true, bool show3d = false);
        DPVISION_LEGACY_API_EXPORT bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent = false);
        DPVISION_LEGACY_API_EXPORT bool addObject(std::shared_ptr<CBaseObject> obj, bool setItCurrent = false);

        /**
         * @brief Notify the host that one existing object's state changed, but
         * the workspace tree structure did not.
         *
         * Use this after direct object mutations such as visibility, label,
         * lock state, transform or geometry changes, when the object keeps the
         * same parent and stays in the same place in the workspace tree.
         *
         * Do not use this for add/remove/reparent operations. Those should go
         * through semantic workspace/object operations, or fall back to
         * `notifyStructureChanged()` if legacy code already changed the tree.
         */
        DPVISION_LEGACY_API_EXPORT void notifyObjectStateChanged(int id);

        /**
         * @brief Notify the host that the workspace tree structure changed.
         *
         * Use this after direct structural mutations such as reparenting,
         * moving an object under a different parent, rebuilding a subtree or
         * changing visibility recursively in a way that effectively requires a
         * workspace tree refresh.
         *
         * This is broader and heavier than `notifyObjectStateChanged()`. Prefer
         * `notifyObjectStateChanged()` when only one object's internal state
         * changed and the tree topology stayed the same.
         */
        DPVISION_LEGACY_API_EXPORT void notifyStructureChanged();

        /**
         * @brief Loads a model from file with extension
         * @param fext File extension
         * @param path File path
         * @param synchronous If true, loads synchronously
         * @param setItCurrent If true, sets the model as current
         * @return Pointer to loaded CModel3D
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> loadModel(const QString fext, const QString& path, bool synchronous = true, bool setItCurrent = false);

        /**
         * @brief Loads a model from file
         * @param path File path
         * @param synchronous If true, loads synchronously
         * @param setItCurrent If true, sets the model as current
         * @return Pointer to loaded CModel3D
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> loadModel(const QString& path, bool synchronous = true, bool setItCurrent = false, std::shared_ptr<IProgressListener> prg = nullptr);

        /**
         * @brief Loads a model from file (wide string path)
         * @param path File path
         * @param synchronous If true, loads synchronously
         * @param setItCurrent If true, sets the model as current
         * @return Pointer to loaded CModel3D
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> loadModel(const std::wstring& path, bool synchronous = true, bool setItCurrent = false);

        /**
         * @brief Sets visibility for all models in workspace
         * @param visibility Visibility state to set
         */
        DPVISION_LEGACY_API_EXPORT void setAllModelsVisible(bool visibility = true);

        /**
         * @brief Creates a copy of a model
         * @param obj Model to duplicate
         * @return Pointer to duplicated CModel3D
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> duplicateModel(std::shared_ptr<CModel3D> orginal);

        /**
         * @brief Creates a copy of a model by ID
         * @param id Model identifier
         * @return Pointer to duplicated CModel3D
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> duplicateModel(int id);

        /**
         * @brief Creates a copy of the current model
         * @return Pointer to duplicated CModel3D
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> duplicateCurrentModel();

        /**
         * @brief Gets a model by ID
         * @param id Model identifier
         * @return Pointer to CModel3D or nullptr if not found
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> getModel(int id);

        /**
         * @brief Gets the current model
         * @return Pointer to current CModel3D or nullptr if none selected
         */
        DPVISION_LEGACY_API_EXPORT std::shared_ptr<CModel3D> getCurrentModel();

        /**
         * @brief Gets the current model ID
         * @return Current model ID or -1 if none selected
         */
        DPVISION_LEGACY_API_EXPORT int getCurrentModelId();

        DPVISION_LEGACY_API_EXPORT bool removeModel(int id);
        DPVISION_LEGACY_API_EXPORT bool removeModel(std::shared_ptr<CModel3D> obj);

        /**
         * @brief Removes an image by ID
         * @param id Image identifier
         * @param deleteIt If true, deletes the image from memory
         * @return True if removal was successful
         */
        DPVISION_LEGACY_API_EXPORT bool removeImage(int id);

        /**
         * @brief Removes an image by pointer
         * @param im Image to remove
         * @param deleteIt If true, deletes the image from memory
         * @return True if removal was successful
         */
        DPVISION_LEGACY_API_EXPORT bool removeImage(std::shared_ptr<CImage> im);

        /**
         * @brief Removes the current model
         * @return True if removal was successful
         */
        DPVISION_LEGACY_API_EXPORT bool removeCurrentModel();

        /**
         * @brief Removes all models from workspace
         * @return True if removal was successful
         */
        DPVISION_LEGACY_API_EXPORT bool removeAllModels();

        /**
         * @brief Removes all selected models from workspace
         * @return True if removal was successful
         */
        DPVISION_LEGACY_API_EXPORT bool removeSelectedModels();

        /**
         * @brief Sets the current model by ID
         * @param id Model identifier
         * @return New current model ID or -1 if not found
         */
        DPVISION_LEGACY_API_EXPORT int setCurrentModel(int id);

        /**
         * @namespace SELECTION
         * @brief Namespace containing selection management functionality
         */
        namespace SELECTION {
            /**
             * @brief Selects a model by ID
             * @param id Model identifier
             */
            DPVISION_LEGACY_API_EXPORT void selectModel(int id);

            /**
             * @brief Unselects a model by ID
             * @param id Model identifier
             */
            DPVISION_LEGACY_API_EXPORT void unselectModel(int id);

            /**
             * @brief Checks if a model is selected
             * @param id Model identifier
             * @return True if model is selected
             */
            DPVISION_LEGACY_API_EXPORT bool isModelSelected(int id);

            /**
             * @brief Clears all selections
             */
            DPVISION_LEGACY_API_EXPORT void clear();

            /**
             * @brief Gets list of selected object IDs
             * @return List of selected object IDs
             */
            DPVISION_LEGACY_API_EXPORT std::list<int> getList();

            /**
             * @brief Gets filtered list of selected object IDs
             * @param types Set of object types to filter by
             * @param obj Optional parent object to filter within
             * @return Filtered list of selected object IDs
             */
            DPVISION_LEGACY_API_EXPORT std::list<int> getList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj = nullptr);

            /**
             * @brief Gets filtered list of selected objects
             * @param types Set of object types to filter by
             * @param obj Optional parent object to filter within
             * @return List of pointers to selected objects
             */
            DPVISION_LEGACY_API_EXPORT std::list<std::shared_ptr<CBaseObject>> getObjList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj = nullptr);

            /**
             * @brief Sets visibility for selected models
             * @param visibility Visibility state to set
             */
            DPVISION_LEGACY_API_EXPORT void setModelsVisible(bool visibility = true);
        }
    };

};

#endif
