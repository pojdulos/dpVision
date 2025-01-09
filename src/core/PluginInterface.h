/**
 * @file plugininterface.h
 * @brief Interface defining basic plugin functionality
 */

#pragma once

#include "Global.h"

#include <set>
#include <QtPlugin>

 /**
  * @class PluginInterface
  * @brief Base class for all plugins
  *
  * Defines the basic interface and functionality required by the plugin system
  */
class PluginInterface
{
public:
    /**
     * @struct PickEvent
     * @brief Structure storing information about object picking event
     */
    struct PickEvent
    {
        void* viewer;            ///< Pointer to the viewer
        int objId;              ///< ID of picked object
        size_t faceId;          ///< ID of picked face
        size_t vertexId;        ///< ID of closest vertex
        double x;               ///< True X coordinate in object space
        double y;               ///< True Y coordinate in object space
        double z;               ///< True Z coordinate in object space

        std::set<INDEX_TYPE> hits;  ///< Set of intersected elements

        /**
         * @brief Ray origin point
         */
        struct
        {
            double x;           ///< X coordinate of origin point
            double y;           ///< Y coordinate of origin point
            double z;           ///< Z coordinate of origin point
        } origin;

        /**
         * @brief Ray direction vector
         */
        struct
        {
            double x;           ///< X component of direction vector
            double y;           ///< Y component of direction vector
            double z;           ///< Z component of direction vector
        } direction;

        /**
         * @brief Mouse position
         */
        struct
        {
            double x;           ///< Mouse X coordinate
            double y;           ///< Mouse Y coordinate
        } mouse;

        bool facePicked;        ///< Flag indicating if a face was picked
    };

protected:
    unsigned int m_ID;          ///< Unique plugin identifier
    QString m_UUID;             ///< Plugin UUID
    QString m_Name;             ///< Plugin name
    QString m_Path;             ///< Plugin file path

public:
    /**
     * @brief Virtual destructor
     */
    virtual ~PluginInterface() {}

    /**
     * @brief Gets the plugin ID
     * @return Plugin ID
     */
    unsigned int id() { return m_ID; };

    /**
     * @brief Sets the plugin ID
     * @param id New plugin ID
     */
    void setID(unsigned int id) { m_ID = id; };

    /**
     * @brief Gets the plugin UUID
     * @return Plugin UUID
     */
    QString uuid() const { return m_UUID; }

    /**
     * @brief Sets the plugin UUID
     * @param uuid New plugin UUID
     */
    void setUUID(const QString& uuid) { m_UUID = uuid; }

    /**
     * @brief Gets the plugin name
     * @return Plugin name
     */
    QString name() const { return m_Name; }

    /**
     * @brief Sets the plugin name
     * @param name New plugin name
     */
    void setName(const QString& name) { m_Name = name; }

    /**
     * @brief Gets the plugin path
     * @return Plugin path
     */
    QString path() const { return m_Path; }

    /**
     * @brief Sets the plugin path
     * @param path New plugin path
     */
    void setPath(const QString& path) { m_Path = path; }

    /**
     * @brief Checks if the plugin should be hidden
     * @return 0 if plugin is not hidden, non-zero otherwise
     * @note Used to hide certain plugins, like FileParsers
     */
    virtual int isHidden() { return 0; }

    /**
     * @brief Checks if plugin has the specified UUID
     * @param strUUID UUID to check
     * @return true if UUID matches, false otherwise
     */
    bool hasUUID(const QString& strUUID) { return m_UUID == strUUID; }

    /**
     * @brief Called on double-click on plugin name
     */
    virtual void onRun() {}

    /**
     * @brief Called when plugin is loaded
     */
    virtual void onLoad() {}

    /**
     * @brief Called before plugin unload
     */
    virtual void onUnload() {}

    /**
     * @brief Called when plugin is activated
     */
    virtual void onActivate() {}

    /**
     * @brief Called when plugin is deactivated
     */
    virtual void onDeactivate() {}

    /**
     * @brief Called when button on PluginPanel is pressed (deprecated)
     * @param name Button name
     */
    virtual void onButton(std::wstring name) {}

    /**
     * @brief Called when button on PluginPanel is pressed
     * @param name Button name
     */
    virtual void onButton(const QString& name) { onButton(name.toStdWString()); }

    /**
     * @brief Called when plugin is active and Ctrl+MouseClick is performed on any model
     * @param pickEvent Pick event information
     * @return true if event was handled, false otherwise
     */
    virtual bool onMousePick(PickEvent pickEvent) { return false; }

    /**
     * @brief Called when plugin is active and Shift+MouseClick is performed on any model
     * @param objId ID of indicated object
     * @return true if event was handled, false otherwise
     */
    virtual bool onModelIndication(int objId) { return false; }
};

/**
 * @brief Qt macro declaring plugin interface
 */
Q_DECLARE_INTERFACE(PluginInterface, "dpVision.PluginInterface/1.0")
