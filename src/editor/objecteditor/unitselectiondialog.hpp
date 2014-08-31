/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_UNITSELECTIONDIALOG_HPP
#define WC3LIB_EDITOR_UNITSELECTIONDIALOG_HPP

#include <QDialog>

#include "ui_unitselectiondialog.h"

namespace wc3lib
{

namespace editor
{

class UnitMetaData;

/**
 * \brief An icon listing dialog which allows the user to select a standard or a custom unit.
 *
 * The units are ordered by their race.
 */
class UnitSelectionDialog : public QDialog, protected Ui::UnitSelectionDialog
{
	public:
		explicit UnitSelectionDialog(UnitMetaData *unitMetaData, QWidget* parent = 0, Qt::WindowFlags f = 0);

		void select(QString rawDataId);

		UnitMetaData* metaData() const;

	private:
		UnitMetaData *m_metaData;
};

inline UnitMetaData* UnitSelectionDialog::metaData() const
{
	return this->m_metaData;
}

}

}

#endif // WC3LIB_EDITOR_UNITSELECTIONDIALOG_HPP
