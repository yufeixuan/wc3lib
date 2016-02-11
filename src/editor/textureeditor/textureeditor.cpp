/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include <QtGui>

#include <KFileDialog>
#include <KActionCollection>
#include <KMenuBar>
#include <KStandardAction>
#include <KService>
#include <KImageFilePreview>
#include <KIntNumInput>
#include <KAction>

#include "textureeditor.hpp"
#include "../qblp/blpiohandler.hpp"
#include "../platform.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

TextureEditor::TextureEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f)
: Module(source, parent, f)
, m_scrollArea(new QScrollArea(this))
, m_imageLabel(new QLabel(this))
, m_mipMapIndex(0)
, m_showsAlphaChannel(false)
, m_showsTransparency(false)
, m_factor(1.0)
, m_zoomToFit(true)
, m_textureActionCollection(0)
, m_showAlphaChannelAction(0)
, m_showTransparencyAction(0)
, m_zoomToFitAction(0)
, m_mipMapsMenu(0)
, m_colorPaletteDialog(0)
, m_chargesDialog(0)
, m_loadDialog(0)
, m_saveDialog(0)
{
	Module::setupUi();

	imageLabel()->setAlignment(Qt::AlignCenter);
	imageLabel()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	scrollArea()->setFrameShape(QFrame::NoFrame);
	scrollArea()->setAlignment(Qt::AlignCenter);
	scrollArea()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	scrollArea()->setWidget(imageLabel());
	scrollArea()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	topLayout()->addWidget(scrollArea());

	setWindowTitle(tr("Texture Editor"));

	/*
	KService::Ptr service = KService::serviceByDesktopPath("gvpart.desktop");

	if (service)
	{
		m_part = service->createInstance<KParts::ReadWritePart>(this);

		if (m_part)
		{
			// tell the KParts::MainWindow that this is indeed
			// the main widget
			gridLayout_2->addWidget(m_part->widget());

			//setupGUI(ToolBar | Keys | StatusBar | Save);

			// and integrate the part's GUI with the shell's
			//createGUI(m_part);
		}
		else
			qDebug() << "Was not able to create gwenview part!";
	}
	else
	{
		// if we couldn't find our Part, we exit since the Shell by
		// itself can't do anything useful
		KMessageBox::error(this, i18n("Service \"gvpart.desktop\" not found."));
	}
	*/

	foreach (QAction *action, m_textureActionCollection->actions())
	{
		action->setEnabled(false);
	}
}

TextureEditor::~TextureEditor()
{
}

bool TextureEditor::configure()
{
	readSettings();

	// Update required files if started as stand-alone module
	if (!hasEditor())
	{
		if (!this->source()->configure(this))
		{
			return false;
		}

		try
		{
			source()->sharedData()->refreshWorldEditorStrings(this);
		}
		catch (wc3lib::Exception &e)
		{
			QMessageBox::critical(0, tr("Error"), tr("Error when loading default files: %1").arg(e.what()));

			return false;
		}
	}

	retranslateUi();

	return true;
}

void TextureEditor::retranslateUi()
{
	Module::retranslateUi();
}

TextureEditor::LoadDialogWidget::LoadDialogWidget(QWidget *parent) : QWidget(parent), m_mipMapsInput(new KIntNumInput(this))
{
	QVBoxLayout *layout = new QVBoxLayout();
	this->setLayout(layout);

	mipMapsInput()->setLabel(tr("MIP maps:"));
	mipMapsInput()->setMaximum(blp::Blp::maxMipMaps);
	mipMapsInput()->setMinimum(0);
	mipMapsInput()->setValue(blp::Blp::defaultMipMaps);
	layout->addWidget(mipMapsInput());
}

TextureEditor::LoadDialog::LoadDialog(QObject *parent) : QObject(parent), m_widget(new LoadDialogWidget())
{
	//KMimeType::Ptr mime(KMimeType::mimeType("image/x-blp"));
	m_dialog = new KFileDialog(KUrl(), i18n("*|All Files\n*.blp|Blizzard Pictures\n*.png|Portable Network Graphics\n*.jpg|JPEG Files\n*.tga|TGA Files"), 0, m_widget); // TODO MIME filters do not work ("all/allfiles"). Use image filter.
	dialog()->setCaption(i18n("Open texture"));
	dialog()->setConfirmOverwrite(true); // TODO doesn't work
	dialog()->setPreviewWidget(new KImageFilePreview(m_dialog));
	dialog()->setMode(KFile::File);
	dialog()->setOperationMode(KFileDialog::Opening);
}

TextureEditor::LoadDialog::~LoadDialog()
{
	delete m_dialog; // deletes widget as well
}

TextureEditor::SaveDialogWidget::SaveDialogWidget(QWidget *parent) : QWidget(parent), m_qualityInput(new KIntNumInput(this)), m_mipMapsInput(new KIntNumInput(this))
{
	QVBoxLayout *layout = new QVBoxLayout();
	this->setLayout(layout);

	qualityInput()->setLabel(i18n("Quality:"));
	qualityInput()->setMaximum(100);
	qualityInput()->setMinimum(-1);
	qualityInput()->setValue(blp::Blp::defaultQuality);
	layout->addWidget(qualityInput());

	mipMapsInput()->setLabel(i18n("MIP maps:"));
	mipMapsInput()->setMaximum(blp::Blp::maxMipMaps);
	mipMapsInput()->setMinimum(0);
	mipMapsInput()->setValue(blp::Blp::defaultMipMaps);
	layout->addWidget(mipMapsInput());
}

TextureEditor::SaveDialog::SaveDialog(QObject *parent) : QObject(parent), m_widget(new SaveDialogWidget())
{
	//KMimeType::Ptr mime(KMimeType::mimeType("image/x-blp"));
	m_dialog = new KFileDialog(KUrl(), i18n("*|All Files\n*.blp|Blizzard Pictures\n*.png|Portable Network Graphics\n*.jpg|JPEG Files\n*.tga|TGA Files"), 0, m_widget); // TODO MIME filters do not work ("all/allfiles"). Use image filter.
	dialog()->setCaption(i18n("Save texture"));
	dialog()->setConfirmOverwrite(true); // TODO doesn't work
	dialog()->setPreviewWidget(new KImageFilePreview(m_dialog));
	dialog()->setMode(KFile::File);
	dialog()->setOperationMode(KFileDialog::Saving);
}

TextureEditor::SaveDialog::~SaveDialog()
{
	delete m_dialog; // deletes widget as well
}

TextureEditor::ChargesDialog::ChargesDialog(QWidget *parent)
: QDialog(parent)
, m_buttonBox(new KDialogButtonBox(this))
, m_chargesInput(new KIntNumInput(this))
, m_hasChargesCheckBox(new QCheckBox(tr("Has Charges:"), this))
{
	this->setWindowTitle(tr("Set Charges"));
	this->setLayout(new QVBoxLayout());
	this->layout()->addWidget(chargesInput());
	this->layout()->addWidget(hasChargesCheckBox());
	this->layout()->addWidget(m_buttonBox);

	chargesInput()->setMaximum(100);
	chargesInput()->setMinimum(0);
	hasChargesCheckBox()->setChecked(true);
	m_buttonBox->setStandardButtons(KDialogButtonBox::Ok | KDialogButtonBox::Cancel);

	connect(hasChargesCheckBox(), SIGNAL(toggled(bool)), chargesInput(), SLOT(setEnabled(bool)));
	connect(m_buttonBox->button(KDialogButtonBox::Ok), SIGNAL(pressed()), this, SLOT(accept()));
	connect(m_buttonBox->button(KDialogButtonBox::Cancel), SIGNAL(pressed()), this, SLOT(reject()));
}

void TextureEditor::openFile()
{
	KUrl url;

	if (this->loadDialog()->dialog()->exec() == QDialog::Accepted)
	{
		KUrl url = loadDialog()->dialog()->selectedUrl();

		if (url.isEmpty())
		{
			return;
		}

		QMap<QString, QString> options;
		options["MipMaps"] = QString::number(loadDialog()->mipMapsInput()->value());

		openUrl(url, options);
	}
}

void TextureEditor::openUrl(const KUrl &url, QMap<QString, QString> options)
{
	TexturePtr texture(new Texture(url));

	try
	{
		texture->setSource(source());
		texture->loadBlp(options);
		texture->loadQt();
	}
	catch (Exception &exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read BLP image from file \"%1\".\nException:\n\"%2\".").arg(url.toLocalFile()).arg(exception.what()));

		return;
	}

	if (texture->qt()->isNull())
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read file \"%1\".").arg(url.toLocalFile()));

		return;
	}

	m_texture.swap(texture);
	m_mipMapIndex = 0;
	m_showsAlphaChannel = false;
	m_showsTransparency = false;
	m_factor = 1.0;
	m_zoomToFit = true;
	this->m_zoomToFitAction->setChecked(this->m_zoomToFit);

	foreach (QAction *action, m_textureActionCollection->actions())
	{
		action->setEnabled(true);
	}

	m_showAlphaChannelAction->setEnabled(this->texture()->qt()->hasAlphaChannel());
	this->m_textureActionCollection->action("editcolorpalette")->setEnabled(this->texture()->qt()->format() == QImage::Format_Indexed8);
	this->m_textureActionCollection->action("dropcolorpalette")->setEnabled(this->texture()->qt()->format() == QImage::Format_Indexed8);

	m_mipMapsMenu->clear();
	m_mipMaps.resize(this->texture()->blp()->mipMaps().size());
	std::size_t i = 0;

	BOOST_FOREACH(blp::Blp::MipMaps::const_reference mipMap, this->texture()->blp()->mipMaps())
	{
		BlpIOHandler ioHandler;
		QImage image;

		if (ioHandler.read(&image, mipMap, *this->texture()->blp()))
		{
			m_mipMaps[i] = image;
		}
		else
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to read MIP map \"%1\".").arg(i));
		}

		++i;
		QAction *action = new QAction(QIcon(":/actions/opentexture.png"), i18n("MIP map %1", i), this);
		action->setShortcut(tr("Ctrl+M+%1").arg(i));
		connect(action, SIGNAL(triggered()), this, SLOT(showMipMap()));
		m_mipMapsMenu->addAction(action);
	}

	/// \todo Image format (e. g. Format_Indexed8 is not stored) which leads to another format when saving image somewhere.
	refreshImage();
}

void TextureEditor::saveFile()
{
	if (!hasTexture() || this->texture()->qt()->isNull())
	{
		QMessageBox::critical(this, tr("Error"), tr("No open image file."));

		return;
	}

	if (saveDialog()->dialog()->exec() == QDialog::Accepted)
	{
		KUrl url = saveDialog()->dialog()->selectedUrl();

		if (url.isEmpty())
		{
			return;
		}

		try
		{
			QMap<QString, QString> compression;
			compression["Quality"] = QString::number(saveDialog()->qualityInput()->value());
			compression["MipMaps"] = QString::number(saveDialog()->mipMapsInput()->value());
			this->texture()->save(url, QFileInfo(url.toLocalFile()).suffix().toLower(), compression);
		}
		catch (Exception &exception)
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to save image to file \"%1\".\nException:\n\"%2\".").arg(url.toEncoded().constData()).arg(exception.what()));

			return;
		}
	}
}

void TextureEditor::closeFile()
{
	foreach (QAction *action, m_textureActionCollection->actions())
	{
		action->setEnabled(false);
	}

	m_mipMapsMenu->clear();

	m_texture.reset();
	refreshImage();
}

void TextureEditor::showSettings()
{
}

void TextureEditor::editColorPalette()
{
	QImage &image = m_mipMaps[mipMapIndex()];

	if (image.colorCount() == 0)
	{
		return;
	}

	this->colorPaletteDialog()->applyFromImage(image);

	if (this->colorPaletteDialog()->exec() == QDialog::Accepted)
	{
		this->colorPaletteDialog()->applyToImage(image);
		refreshImage();
	}
}

void TextureEditor::dropColorPalette()
{
	if (texture()->hasBlp())
	{
		if (texture()->blp()->compression() == blp::Blp::Compression::Paletted)
		{
			texture()->blp()->setCompression(blp::Blp::Compression::Jpeg);
			texture()->loadQt();
			refreshImage();
			qDebug() << "Dropped color palette!";
		}
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), tr("Dropping color palette is currently supported for BLP textures only."));
	}
}

void TextureEditor::makeActive()
{
}

void TextureEditor::makePassive()
{
}

void TextureEditor::makeAutocast()
{
}

void TextureEditor::makeInfocardNormal()
{
}

void TextureEditor::makeInfocardLevel()
{
}

void TextureEditor::setCharges()
{
	if (this->chargesDialog()->exec() == QDialog::Accepted)
	{
		if (!this->chargesDialog()->hasChargesCheckBox()->isChecked())
		{
			refreshImage();
		}
		else
		{
			setCharges(this->chargesDialog()->chargesInput()->value());
		}
	}
}

void TextureEditor::setCharges(int charges)
{
	refreshImage();
	QPixmap pixmap = *m_imageLabel->pixmap();
	QPainter painter(&pixmap);
	int x = pixmap.rect().width() - 20;
	int y = pixmap.rect().height() - 20;
	painter.setPen(QColor(Qt::black));
	QRect rect(x, y, pixmap.rect().width() - x, pixmap.rect().height() - y);
	painter.fillRect(rect, Qt::SolidPattern);
	painter.setPen(QColor(Qt::white));
	painter.drawText(rect, Qt::AlignCenter, QString::number(charges));
	m_imageLabel->setPixmap(pixmap);
}

void TextureEditor::showAlphaChannel()
{
	if (!hasTexture())
	{
		return;
	}

	if (!this->m_texture->qt()->hasAlphaChannel())
	{
		QMessageBox::critical(this, tr("Error"), tr("Image doesn't have alpha channel."));

		return;
	}

	if (showsAlphaChannel())
	{
		m_showAlphaChannelAction->setText(tr("Show alpha channel"));
		m_showsAlphaChannel = false;
	}
	else
	{
		m_showAlphaChannelAction->setText(tr("Hide alpha channel"));
		m_showsAlphaChannel = true;
	}

	refreshImage();
}

void TextureEditor::showTransparency()
{
	if (!hasTexture())
	{
		return;
	}

	/// \todo According to documentation checking for mask is an expensive operation.

	if (showsTransparency())
	{
		m_showTransparencyAction->setText(i18n("Show transparency"));
		m_showsTransparency = false;
	}
	else
	{
		m_showTransparencyAction->setText(i18n("Hide transparency"));
		m_showsTransparency = true;
	}

	refreshImage();
}

void TextureEditor::actualSize()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_factor = 1.0;
	refreshImage();
}

void TextureEditor::zoomToFit()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_zoomToFit = !this->m_zoomToFit;
	this->m_zoomToFitAction->setChecked(this->m_zoomToFit);
	refreshImage();
}

void TextureEditor::zoomIn()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_factor += 0.20;
	refreshImage();
}

void TextureEditor::zoomOut()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_factor -= 0.20;
	refreshImage();
}

void TextureEditor::massConverter()
{
}

void TextureEditor::showMipMap()
{
	QAction *action = boost::polymorphic_cast<QAction*>(sender());
	m_mipMapIndex = this->m_mipMapsMenu->actions().indexOf(action);
	m_showsAlphaChannel = false;
	m_showsTransparency = false;
	m_factor = 1.0;
	m_zoomToFit = true;
	this->m_zoomToFitAction->setChecked(this->m_zoomToFit);
	refreshImage();
}

void TextureEditor::refreshImage()
{
	if (!hasTexture())
	{
		this->m_imageLabel->setPixmap(QPixmap());
		// set the minimum size so that the scroll area will show scroll bars for the whole image size
		this->m_imageLabel->setMinimumSize(QSize(0, 0));
		this->m_imageLabel->setMaximumSize(QSize(0, 0));

		return;
	}

	QPixmap newPixmap;
	QImage image = mipMaps()[mipMapIndex()];

	if (m_zoomToFit)
	{
		const QSize fitSize = this->scrollArea()->maximumViewportSize();


		if (!showsAlphaChannel())
		{
			newPixmap = QPixmap::fromImage(image).scaled(fitSize, Qt::KeepAspectRatio);
		}
		else
		{
			newPixmap = QPixmap::fromImage(image.createAlphaMask()).scaled(fitSize, Qt::KeepAspectRatio);
		}
	}
	else
	{
		if (!showsAlphaChannel())
		{
			newPixmap = QPixmap::fromImage(image).scaled(mipMaps()[mipMapIndex()].size() * this->factor());
		}
		else
		{
			newPixmap = QPixmap::fromImage(image.createAlphaMask()).scaled(mipMaps()[mipMapIndex()].size() * this->factor());
		}
	}

	if (showsTransparency())
	{
		newPixmap.setMask(this->m_imageLabel->pixmap()->createMaskFromColor(Qt::transparent));
	}

	this->m_imageLabel->setPixmap(newPixmap);

	// fit to the viewport's size
	if (this->m_zoomToFit)
	{
		const QSize fitSize = this->scrollArea()->maximumViewportSize();

		this->m_imageLabel->setMinimumSize(fitSize);
		this->m_imageLabel->setMaximumSize(fitSize);
	}
	// set the minimum size so that the scroll area will show scroll bars for the whole image size
	else
	{
		this->m_imageLabel->setMinimumSize(newPixmap.size());
		this->m_imageLabel->setMaximumSize(newPixmap.size());
	}
}

void TextureEditor::createFileActions(QMenu *menu)
{
	m_textureActionCollection = new KActionCollection((QObject*)this);

	QAction *action = 0;

	action = new QAction(QIcon(":/actions/opentexture.png"), i18n("Open texture"), this);
	action->setShortcut(QKeySequence("Ctrl+O"));
	connect(action, SIGNAL(triggered()), this, SLOT(openFile()));
	menu->addAction(action);

	action = new QAction(QIcon(":/actions/savetexture.png"), i18n("Save texture"), this);
	action->setShortcut(QKeySequence("Ctrl+S"));
	connect(action, SIGNAL(triggered()), this, SLOT(saveFile()));
	menu->addAction(action);
	m_textureActionCollection->addAction("savetexture", action);

	action = new QAction(QIcon(":/actions/closetexture.png"), i18n("Close texture"), this);
	action->setShortcut(QKeySequence("Ctrl+W"));
	connect(action, SIGNAL(triggered()), this, SLOT(closeFile()));
	menu->addAction(action);
	m_textureActionCollection->addAction("closetexture", action);

	action = new QAction(QIcon(":/actions/settings.png"), i18n("Settings"), this);
	//action->setShortcut(KShortcut(i18n("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(showSettings()));
	menu->addAction(action);
}

void TextureEditor::createEditActions(QMenu *menu)
{
	QAction *action = new QAction(QIcon(":/actions/editcolorpalette.png"), i18n("Edit color palette"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(editColorPalette()));
	menu->addAction(action);
	m_textureActionCollection->addAction("editcolorpalette", action);

	action = new QAction(QIcon(":/actions/dropcolorpalette.png"), i18n("Drop color palette"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(dropColorPalette()));
	menu->addAction(action);
	m_textureActionCollection->addAction("dropcolorpalette", action);

	action = new QAction(QIcon(":/actions/setcharges.png"), i18n("Set charges"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(setCharges()));
	menu->addAction(action);
	m_textureActionCollection->addAction("setcharges", action);
}

void TextureEditor::createMenus(QMenuBar *menuBar)
{
	QMenu *viewMenu = new QMenu(tr("View"), this);
	//this->m_viewMenu = viewMenu;
	menuBar->addMenu(viewMenu);

	m_showAlphaChannelAction = new QAction(QIcon(":/actions/showalphachannel.png"), i18n("Show alpha channel"), this);
	connect(m_showAlphaChannelAction, SIGNAL(triggered()), this, SLOT(showAlphaChannel()));
	viewMenu->addAction(m_showAlphaChannelAction);
	m_textureActionCollection->addAction("showalphachannel", m_showAlphaChannelAction);

	m_showTransparencyAction = new QAction(QIcon(":/actions/showtransparency.png"), i18n("Show transparency"), this);
	connect(m_showTransparencyAction, SIGNAL(triggered()), this, SLOT(showTransparency()));
	viewMenu->addAction(m_showTransparencyAction);
	m_textureActionCollection->addAction("showtransparency", m_showTransparencyAction);

	KAction *action = KStandardAction::actualSize(this, SLOT(actualSize()), this);
	viewMenu->addAction(action);
	m_textureActionCollection->addAction("actualsize", action);

	m_zoomToFitAction = new QAction(QIcon(":/actions/zoomtofit.png"), i18n("Zoom to fit"), this);
	m_zoomToFitAction->setCheckable(true);
	connect(m_zoomToFitAction, SIGNAL(triggered()), this, SLOT(zoomToFit()));
	viewMenu->addAction(m_zoomToFitAction);
	m_textureActionCollection->addAction("zoomtofit", m_zoomToFitAction);

	action = KStandardAction::zoomIn(this, SLOT(zoomIn()), this);
	viewMenu->addAction(action);
	m_textureActionCollection->addAction("zoomin", action);

	action = KStandardAction::zoomOut(this, SLOT(zoomOut()), this);
	viewMenu->addAction(action);
	m_textureActionCollection->addAction("zoomout", action);

	QMenu *toolsMenu = new QMenu(tr("Tools"), this);
	//this->m_toolsMenu = toolsMenu;
	menuBar->addMenu(toolsMenu);

	// TODO add check buttons to mass converter widget which allow you to a) convert and b) generate info cards etc. and c) to remove old files.
	QAction *massConvAction = new QAction(QIcon(":/actions/massconverter.png"), tr("Mass converter"), this);
	connect(massConvAction, SIGNAL(triggered()), this, SLOT(massConverter()));
	toolsMenu->addAction(massConvAction);

	this->m_mipMapsMenu = new QMenu(tr("MIP maps"), this);
	menuBar->addMenu(m_mipMapsMenu);
}

void TextureEditor::createWindowsActions(WindowsMenu *menu)
{
}

void TextureEditor::createToolButtons(ModuleToolBar *toolBar)
{
}

SettingsInterface* TextureEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void TextureEditor::onSwitchToMap(Map *map)
{
}

void TextureEditor::resizeEvent(QResizeEvent *event)
{
	if (m_zoomToFit)
	{
		refreshImage();
	}

	Module::resizeEvent(event);
}

QIcon TextureEditor::icon()
{
	return QIcon(this->source()->sharedData()->worldEditDataPixmap("ToolBarIcon_Module_Terrain", "WorldEditArt", this));
}

#include "moc_textureeditor.cpp"

}

}
