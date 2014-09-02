/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef WC3LIB_EDITOR_OBJECTEDITORTAB_HPP
#define WC3LIB_EDITOR_OBJECTEDITORTAB_HPP

#include <boost/cast.hpp>

#include <QWidget>

#include <KUrl>
#include <KUrlRequester>
#include <KLineEdit>

#include "objecteditor.hpp"
#include "objecttreewidget.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class ObjectMetaData;
class ObjectTableWidget;

class ObjectEditorTab : public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \brief Hash table which stores modificiations by using the field ID as hash (such as "unam").
		 */
		typedef QHash<QString, map::CustomUnits::Modification> Modifications;
		/**
		 * \brief Stores the two object IDs. The original as well as the custom.
		 */
		typedef QPair<QString, QString> ObjectId;
		/**
		 * \brief Hash table which stores objects by using the object ID pair as hash.
		 */
		typedef QHash<ObjectId, Modifications> Objects;

		ObjectEditorTab(class MpqPriorityList *source, ObjectMetaData *metaData, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~ObjectEditorTab();

		class MpqPriorityList* source() const;

		/**
		 * \return If it has an object editor (\ref hasObjectEditor()) this returns its corresponding tab index of \ref ObjectEditor::tabWidget().
		 */
		int tabIndex() const;
		bool hasObjectEditor() const;
		class ObjectEditor* objectEditor() const throw (std::bad_cast);
		KLineEdit* filterLineEdit() const;
		ObjectTreeWidget* treeWidget() const;
		ObjectTableWidget* tableWidget() const;

		ObjectMetaData* metaData() const;
		/**
		 * Custom Units are used in Warcraft III: Reign of Chaos.
		 * So this should throw an exception in other tabs than unit and items since they need additional data
		 * which can be gotten from \ref customObjects().
		 *
		 * \return Returns the custom units of the current tab.
		 *
		 * \sa customObjects() hasCustomUnits()
		 *
		 * \throws Exception Throws an exception if custom units cannot store the object data.
		 */
		virtual map::CustomUnits customUnits() const = 0;
		virtual bool hasCustomUnits() const = 0;
		virtual map::CustomObjects customObjects() const = 0;
		virtual bool hasCustomObjects() const = 0;

		virtual map::CustomUnits::Unit currentUnit() const = 0;
		virtual map::CustomObjects::Object currentObject() const = 0;

		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, const map::CustomUnits::Modification &modification);
		void resetField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
		bool isFieldModified(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;
		void clearModifications();
		bool fieldModificiation(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId, map::CustomUnits::Modification &modification) const;
		QString fieldValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;
		QString fieldReadableValue(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId) const;

		void setShowRawData(bool show);
		bool showRawData() const;

		/**
		 * \return Returns the object raw data ID of the currently selected object.
		 */
		QString currentObjectId() const;

		virtual QString name() const = 0;

		virtual void onUpdateCollection(const map::CustomObjects &objects);

		void importCustomUnits(const map::CustomUnits &units);

	public slots:
		void newObject();
		void renameObject();
		void deleteObject();
		void resetObject();
		void resetAllObjects();
		void exportAllObjects();
		void importAllObjects();
		void copyObject();
		void pasteObject();

	protected:
		friend class ObjectEditor;

		virtual void setupUi();

		virtual void setupTreeWidget(ObjectTreeWidget *treeWidget) = 0;
		virtual void fillTreeItem(const QString &originalObjectId, const QString &customObjectId, QTreeWidgetItem *item) = 0;
		virtual ObjectTableWidget* createTableWidget() = 0;

		virtual void onSwitchToMap(class Map *map) = 0;

		virtual void onNewObject() = 0;
		virtual void onRenameObject() = 0;
		virtual void onDeleteObject() = 0;
		virtual void onResetObject() = 0;
		virtual void onResetAllObjects() = 0;
		virtual void onExportAllObjects() = 0;
		virtual void onImportAllObjects() = 0;
		virtual void onCopyObject() = 0;
		virtual void onPasteObject() = 0;

		virtual void onShowRawData(bool show) = 0;

		virtual void activateObject(QTreeWidgetItem *item, int column, const QString &originalObjectId, const QString &customObjectId) = 0;
		virtual void activateFolder(QTreeWidgetItem *item, int column) = 0;

		virtual QString newObjectText() const = 0;
		virtual QString renameObjectText() const = 0;
		virtual QString deleteObjectText() const = 0;
		virtual QString resetObjectText() const = 0;
		virtual QString resetAllObjectsText() const = 0;
		virtual QString exportAllObjectsText() const = 0;
		virtual QString importAllObjectsText() const = 0;
		virtual QString copyObjectText() const = 0;
		virtual QString pasteObjectText() const = 0;

		virtual KUrl copyObjectIconUrl() const = 0;
		virtual KUrl pasteObjectIconUrl() const = 0;
		virtual KUrl newObjectIconUrl() const = 0;

		class MpqPriorityList *m_source;

		int m_tabIndex;

		KLineEdit *m_filterLineEdit;
		ObjectTreeWidget *m_treeWidget; // left side tree widget
		QTreeWidgetItem *m_standardObjectsItem;
		QTreeWidgetItem *m_customObjectsItem;

		class ObjectTableWidget *m_tableWidget; // centered table widget of current selected object

		ObjectMetaData *m_metaData;

		Objects m_objects;

		bool m_showRawData;

	private slots:
		void itemClicked(QTreeWidgetItem *item, int column);
		void filterTreeWidget(QString text);
};

inline class MpqPriorityList* ObjectEditorTab::source() const
{
	return m_source;
}

inline int ObjectEditorTab::tabIndex() const
{
	return m_tabIndex;
}

inline bool ObjectEditorTab::hasObjectEditor() const
{
	// TODO typeid comparison doesn't work, dynamic_cast is working workaround!
	// NOTE parent could be tab widget of object editor
	if (dynamic_cast<ObjectEditor*>(parent()) == 0)
	{
		if (dynamic_cast<KTabWidget*>(parent()) != 0)
			return dynamic_cast<ObjectEditor*>(parentWidget()->parentWidget()->parentWidget()) != 0; // first parent is stacked widget, second tab widget and third object editor
	}

	return true;
}

inline ObjectEditor* ObjectEditorTab::objectEditor() const throw (std::bad_cast)
{
	// TODO typeid comparison doesn't work, dynamic_cast is working workaround!
	// NOTE parent could be tab widget of object editor
	if (dynamic_cast<ObjectEditor*>(parent()) == 0)
		return boost::polymorphic_cast<class ObjectEditor*>(parentWidget()->parentWidget()->parentWidget()); // first parent is stacked widget, second tab widget and third object editor

	return boost::polymorphic_cast<class ObjectEditor*>(parent());
}

inline KLineEdit* ObjectEditorTab::filterLineEdit() const
{
	return this->m_filterLineEdit;
}

inline ObjectTreeWidget* ObjectEditorTab::treeWidget() const
{
	return m_treeWidget;
}

inline ObjectTableWidget* ObjectEditorTab::tableWidget() const
{
	return m_tableWidget;
}

inline ObjectMetaData* ObjectEditorTab::metaData() const
{
	return this->m_metaData;
}

inline bool ObjectEditorTab::showRawData() const
{
	return this->m_showRawData;
}

inline QString ObjectEditorTab::currentObjectId() const
{
	QList<QTreeWidgetItem*> items = this->treeWidget()->selectedItems();

	if (!items.empty())
	{
		return items.first()->data(0, Qt::UserRole).toString();
	}

	return QString();
}

inline void ObjectEditorTab::newObject()
{
	onNewObject();
}

inline void ObjectEditorTab::renameObject()
{
	onRenameObject();
}

inline void ObjectEditorTab::deleteObject()
{
	onDeleteObject();
}

inline void ObjectEditorTab::resetObject()
{
	onResetObject();
}

inline void ObjectEditorTab::resetAllObjects()
{
	onResetAllObjects();
}

inline void ObjectEditorTab::copyObject()
{
	onCopyObject();
}

inline void ObjectEditorTab::pasteObject()
{
	onPasteObject();
}

}

}

#endif
