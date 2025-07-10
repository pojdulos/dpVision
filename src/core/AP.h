#pragma once
#ifndef INTERFEJSAPLIKACJI_H
#define INTERFEJSAPLIKACJI_H

#include "dll_global.h"
#include <string>
#include <list>
#include <set>
#include "BaseObject.h"

// Forward declarations
class PluginInterface;
class CWorkspace;
class CObject;
class CModel3D;
class CAnnotation;
class CMainApplication;
class CMainWindow;
class CImage;
class QString;

/**
 * @namespace AP
 * @brief Main application namespace containing core functionality and utility functions
 */
namespace AP
{
    /**
     * @brief Returns reference to the main application instance
     * @return Reference to CMainApplication
     */
    DPVISION_EXPORT CMainApplication& mainApp();

    /**
     * @brief Returns reference to the main window instance
     * @return Reference to CMainWindow
     */
    DPVISION_EXPORT CMainWindow& mainWin();

    /**
     * @brief Returns pointer to the main window instance
     * @return Pointer to CMainWindow
     */
    DPVISION_EXPORT CMainWindow* mainWinPtr();

    /**
     * @brief Exits the selection mode
     */
    DPVISION_EXPORT void leaveSelectionMode();

    /**
     * @brief Generates and returns a unique identifier
     * @return Unique integer identifier
     */
    DPVISION_EXPORT int getUniqueId();

    /**
     * @brief Processes pending events in the application
     * @param immediate If true, processes events immediately
     */
    DPVISION_EXPORT void processEvents(bool immediate = false);

    /**
     * @brief Returns pointer to the workspace instance
     * @warning Use only as a last resort. Prefer AP::WORKSPACE namespace functions
     * @return Pointer to CWorkspace
     */
    DPVISION_EXPORT CWorkspace* getWorkspace(void);

    /**
     * @brief Returns the executable file path
     * @return Const reference to QString containing the path
     */
    DPVISION_EXPORT const QString& getExeFilePath(void);

    /**
     * @brief Adds annotation to a parent object (OBSOLETE)
     * @deprecated Use AP::MODEL::addAnnotation(int parentid, CAnnotation *an) instead
     */
    DPVISION_EXPORT int addAnnotation(int parentid, std::shared_ptr<CAnnotation> an);

    /**
     * @brief Adds annotation to a 3D model (OBSOLETE)
     * @deprecated Use AP::MODEL::addAnnotation(CModel3D* obj, CAnnotation *an) instead
     */
    DPVISION_EXPORT int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an);

    /**
     * @namespace PLUGIN
     * @brief Namespace containing plugin management functionality
     */
    namespace PLUGIN {
        /**
         * @brief Loads a plugin from the specified path
         * @param pluginPath Path to the plugin file
         * @return True if loading was successful
         */
        DPVISION_EXPORT bool loadPlugin(const QString& pluginPath);

        /**
         * @brief Unloads a plugin with the specified ID
         * @param id Plugin identifier
         */
        DPVISION_EXPORT void unloadPlugin(const unsigned int id);

        /**
         * @brief Retrieves a plugin instance by ID
         * @param id Plugin identifier
         * @return Pointer to PluginInterface or nullptr if not found
         */
        DPVISION_EXPORT PluginInterface* getPlugin(unsigned int id);

        /**
         * @brief Runs a plugin identified by UUID
         * @param strUUID Plugin UUID string
         * @return True if plugin was successfully executed
         */
        DPVISION_EXPORT bool runPlugin(const char* strUUID);
    };

    /**
     * @namespace OBJECT
     * @brief Namespace containing object management functionality
     */
    namespace OBJECT {
        /**
         * @brief Removes an object from the hierarchy
         * @param obj Object to remove
         * @param deleteIt If true, deletes the object from memory
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool remove(std::shared_ptr<CBaseObject> obj, bool deleteIt = false);

        /**
         * @brief Adds a child object to a parent object
         * @param obj Parent object
         * @param child Child object to add
         * @return Child object ID
         */
        DPVISION_EXPORT int addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child);

        /**
         * @brief Removes a child object from a parent object
         * @param obj Parent object
         * @param child Child object to remove
         * @param deleteIt If true, deletes the child object from memory
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child, bool deleteIt = false);

        /**
         * @brief Moves an object to a new parent
         * @param obj Object to move
         * @param newParent New parent object
         */
        DPVISION_EXPORT void moveTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos = true);

        /**
         * @brief Copies an object to a new parent
         * @param obj Object to copy
         * @param newParent New parent object
         */
        DPVISION_EXPORT void copyTo(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> newParent, bool keep_pos = true);
    };

    /**
     * @namespace MODEL
     * @brief Namespace containing 3D model management functionality
     */
    namespace MODEL {
        /**
         * @brief Loads a 3D model from file
         * @param path Path to the model file
         * @param synchronous If true, loads synchronously
         * @return Pointer to loaded CModel3D
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> load(std::wstring path, bool synchronous = true);

        /**
         * @brief Adds a child object to a 3D model
         * @param obj Parent model
         * @param child Child object to add
         * @return Child object ID
         */
        DPVISION_EXPORT int addChild(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> child);
        DPVISION_EXPORT int addChild(CModel3D* obj, CBaseObject* child);

        /**
         * @brief Adds a child object to a parent identified by ID
         * @param parentId Parent object ID
         * @param child Child object to add
         * @return Child object ID
         */
        DPVISION_EXPORT int addChild(int parentId, CBaseObject* child);

        /**
         * @brief Removes a child object from a parent object
         * @param obj Parent object
         * @param child Child object to remove
         */
        DPVISION_EXPORT void removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child);
        //DPVISION_EXPORT void removeChild(CBaseObject* obj, CBaseObject* child);

        /**
         * @brief Removes a child object from a parent object using IDs
         * @param parentid Parent object ID
         * @param childid Child object ID
         */
        DPVISION_EXPORT void removeChild(int parentid, int childid);

        /**
         * @brief Adds an annotation to a 3D model
         * @param obj Parent model
         * @param an Annotation to add
         * @return Annotation ID
         */
        DPVISION_EXPORT int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CAnnotation> an);

        /**
         * @brief Adds an annotation to a parent object identified by ID
         * @param parentid Parent object ID
         * @param an Annotation to add
         * @return Annotation ID
         */
        DPVISION_EXPORT int addAnnotation(int parentid, std::shared_ptr<CAnnotation> an);

        /**
         * @brief Removes an annotation from a parent object
         * @param parentid Parent object ID
         * @param id Annotation ID
         */
        DPVISION_EXPORT void removeAnnotation(int parentid, int id);
    };

    /**
     * @namespace WORKSPACE
     * @brief Namespace containing workspace management functionality
     */
    namespace WORKSPACE {
        /**
         * @brief Returns the workspace instance
         * @return Pointer to CWorkspace
         */
        DPVISION_EXPORT CWorkspace* instance(void);

        /**
         * @brief Returns the number of objects in workspace
         * @return Size of workspace
         */
        DPVISION_EXPORT size_t size();

        /**
         * @brief Finds an object by ID
         * @param id Object identifier
         * @return Pointer to CBaseObject or nullptr if not found
         */
        DPVISION_EXPORT std::shared_ptr<CBaseObject> findId(int id);

        /**
         * @brief Adds a 3D model to workspace
         * @param obj Model to add
         * @param setItCurrent If true, sets the model as current
         * @return True if addition was successful
         */
        DPVISION_EXPORT bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent = false);
        DPVISION_EXPORT bool addModel(CModel3D* obj, bool setItCurrent = false);

        /**
         * @brief Adds an image to workspace
         * @param im Image to add
         * @param showViewer If true, shows the image viewer
         * @param show3d If true, shows 3D view
         * @return True if addition was successful
         */
        DPVISION_EXPORT bool addImage(std::shared_ptr<CImage> im, bool showViewer = true, bool show3d = false);

        /**
         * @brief Adds a generic object to workspace
         * @param obj Object to add
         * @param setItCurrent If true, sets the object as current
         * @return True if addition was successful
         */
        DPVISION_EXPORT bool addObject(std::shared_ptr<CBaseObject> obj, bool setItCurrent = false);

        /**
         * @brief Loads a model from file with extension
         * @param fext File extension
         * @param path File path
         * @param synchronous If true, loads synchronously
         * @param setItCurrent If true, sets the model as current
         * @return Pointer to loaded CModel3D
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> loadModel(const QString fext, const QString& path, bool synchronous = true, bool setItCurrent = false);

        /**
         * @brief Loads a model from file
         * @param path File path
         * @param synchronous If true, loads synchronously
         * @param setItCurrent If true, sets the model as current
         * @return Pointer to loaded CModel3D
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> loadModel(const QString& path, bool synchronous = true, bool setItCurrent = false);

        /**
         * @brief Loads a model from file (wide string path)
         * @param path File path
         * @param synchronous If true, loads synchronously
         * @param setItCurrent If true, sets the model as current
         * @return Pointer to loaded CModel3D
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> loadModel(const std::wstring& path, bool synchronous = true, bool setItCurrent = false);

        /**
         * @brief Sets visibility for all models in workspace
         * @param visibility Visibility state to set
         */
        DPVISION_EXPORT void setAllModelsVisible(bool visibility = true);

        /**
         * @brief Creates a copy of a model
         * @param obj Model to duplicate
         * @return Pointer to duplicated CModel3D
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> duplicateModel(std::shared_ptr<CModel3D> orginal);

        /**
         * @brief Creates a copy of a model by ID
         * @param id Model identifier
         * @return Pointer to duplicated CModel3D
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> duplicateModel(int id);

        /**
         * @brief Creates a copy of the current model
         * @return Pointer to duplicated CModel3D
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> duplicateCurrentModel();

        /**
         * @brief Gets a model by ID
         * @param id Model identifier
         * @return Pointer to CModel3D or nullptr if not found
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> getModel(int id);

        /**
         * @brief Gets the current model
         * @return Pointer to current CModel3D or nullptr if none selected
         */
        DPVISION_EXPORT std::shared_ptr<CModel3D> getCurrentModel();

        /**
         * @brief Gets the current model ID
         * @return Current model ID or -1 if none selected
         */
        DPVISION_EXPORT int getCurrentModelId();

        /**
         * @brief Removes a model by ID
         * @param id Model identifier
         * @param deleteIt If true, deletes the model from memory
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeModel(int id, bool deleteIt = true);

        /**
         * @brief Removes a model by pointer
         * @param obj Model to remove
         * @param deleteIt If true, deletes the model from memory
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeModel(std::shared_ptr<CModel3D> obj, bool deleteIt = true);
        DPVISION_EXPORT bool removeModel(CModel3D* obj, bool deleteIt = true);

        /**
         * @brief Removes an image by ID
         * @param id Image identifier
         * @param deleteIt If true, deletes the image from memory
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeImage(int id, bool deleteIt = true);

        /**
         * @brief Removes an image by pointer
         * @param im Image to remove
         * @param deleteIt If true, deletes the image from memory
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeImage(CImage* im, bool deleteIt = true);

        /**
         * @brief Removes the current model
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeCurrentModel();

        /**
         * @brief Removes all models from workspace
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeAllModels();

        /**
         * @brief Removes all selected models from workspace
         * @return True if removal was successful
         */
        DPVISION_EXPORT bool removeSelectedModels();

        /**
         * @brief Sets the current model by ID
         * @param id Model identifier
         * @return New current model ID or -1 if not found
         */
        DPVISION_EXPORT int setCurrentModel(int id);

        /**
         * @namespace SELECTION
         * @brief Namespace containing selection management functionality
         */
        namespace SELECTION {
            /**
             * @brief Selects a model by ID
             * @param id Model identifier
             */
            DPVISION_EXPORT void selectModel(int id);

            /**
             * @brief Unselects a model by ID
             * @param id Model identifier
             */
            DPVISION_EXPORT void unselectModel(int id);

            /**
             * @brief Checks if a model is selected
             * @param id Model identifier
             * @return True if model is selected
             */
            DPVISION_EXPORT bool isModelSelected(int id);

            /**
             * @brief Clears all selections
             */
            DPVISION_EXPORT void clear();

            /**
             * @brief Gets list of selected object IDs
             * @return List of selected object IDs
             */
            DPVISION_EXPORT std::list<int> getList();

            /**
             * @brief Gets filtered list of selected object IDs
             * @param types Set of object types to filter by
             * @param obj Optional parent object to filter within
             * @return Filtered list of selected object IDs
             */
            DPVISION_EXPORT std::list<int> getList(std::set<CBaseObject::Type> types, CObject* obj = nullptr);

            /**
             * @brief Gets filtered list of selected objects
             * @param types Set of object types to filter by
             * @param obj Optional parent object to filter within
             * @return List of pointers to selected objects
             */
            DPVISION_EXPORT std::list<std::shared_ptr<CBaseObject>> getObjList(std::set<CBaseObject::Type> types, std::shared_ptr<CObject> obj = nullptr);

            /**
             * @brief Sets visibility for selected models
             * @param visibility Visibility state to set
             */
            DPVISION_EXPORT void setModelsVisible(bool visibility = true);
        }
    };

    /**
     * @namespace EVENTS
     * @brief Namespace containing event handling functionality
     */
    namespace EVENTS {
        /**
         * @brief Triggers model indication event
         * @param objId ID of the model being indicated
         */
        DPVISION_EXPORT void modelIndicationEvent(int objId);

        /**
         * @brief Handles workspace tree click event
         * @param objId ID of the clicked object in workspace tree
         */
        DPVISION_EXPORT void workspaceTreeClicked(int objId);
    };
};

#endif