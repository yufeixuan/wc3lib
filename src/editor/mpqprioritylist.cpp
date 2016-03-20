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

#include <QtCore>

#include <KIO/NetAccess>

#include "mpqprioritylist.hpp"
#include "sourcesdialog.hpp"

namespace wc3lib
{

namespace editor
{

MpqPriorityListEntry::MpqPriorityListEntry(const KUrl &url, Priority priority) : m_priority(priority), m_url(url)
{
	qDebug() << "Protocol: " << url.protocol() << " is local file " << url.isLocalFile();
	qDebug() << "URL: " << url << "Local file: " << url.fileName() << "Scheme: " << url.scheme() << " Path: " << url.path();

	/*
	 * For local MPQ archives we use an instance to perform download and upload operations.
	 * This is safer than relying on the installed KIO slave plugin and maybe also faster.
	 * TODO isLocalFile() returns false.
	 * TODO transform into Windows path if required?
	 */
	if (url.protocol() == "mpq" && QFile::exists(url.path()))
	{
		qDebug() << "Opening local MPQ archive: " << url.path();
		m_mpqArchive.reset(new mpq::Archive());
		m_mpqArchive->open(url.path().toUtf8().constData());
	}
}

MpqPriorityListEntry::~MpqPriorityListEntry()
{
}

MpqPriorityList::MpqPriorityList() : m_sharedData(new WarcraftIIIShared(this)), m_locale(mpq::File::Locale::Neutral)
{

}

MpqPriorityList::~MpqPriorityList()
{
	/*
	 * Sets the source for all resources to 0 before deleting the MPQ priority list.
	 * Otherwise when the priority list is already deleted they will try to remove themselves from this source or to load data from the source.
	 */
	for (Resources::iterator iterator = this->m_resources.begin(); iterator != this->m_resources.end(); ++iterator)
	{

		Resource *resource = iterator->second;
		this->m_resources.erase(iterator);
		resource->setSource(0);
	}

	/*
	 * Remove all temporary files which where produced by downloads from local MPQ archives.
	 */
	foreach (QString tmpFile, m_temporaryFiles)
	{
		QFile::remove(tmpFile);
	}
}

bool MpqPriorityListEntry::isDirectory(QWidget *window) const
{
	if (url().isLocalFile())
	{
		return QFileInfo(url().toLocalFile()).isDir();
	}

	if (KIO::NetAccess::exists(url(), KIO::NetAccess::SourceSide, window))
	{
		KIO::UDSEntry entry;

		if (KIO::NetAccess::stat(url(), entry, window))
		{
			return entry.isDir();
		}
	}

	return false;
}

bool MpqPriorityList::addSource(const KUrl &url, MpqPriorityListEntry::Priority priority)
{
	// proper URLs must refer to directories or archives
	/// \todo Support all archive properties and remote directories (smb).
	if (!QFileInfo(url.toLocalFile()).isDir() && url.protocol() != "mpq" && url.protocol() != "tar")
	{
		return false;
	}

	std::auto_ptr<MpqPriorityListEntry> ptr(new MpqPriorityListEntry(url, priority));
	sources().insert(ptr);

	return true;
}

bool MpqPriorityList::addWar3Source()
{
	KUrl url = war3Url();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 20);
}

bool MpqPriorityList::addWar3PatchSource()
{
	KUrl url = war3PatchUrl();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 22);
}

bool MpqPriorityList::addWar3XSource()
{
	KUrl url = war3XUrl();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 21);
}

bool MpqPriorityList::addWar3XLocalSource()
{
	KUrl url = war3XLocalUrl();

	if (url.isEmpty())
	{
		return false;
	}

	return addSource(url, 21);
}

bool MpqPriorityList::addDefaultSources()
{
	return addWar3Source() &&
	addWar3PatchSource() &&
	addWar3Source() &&
	addWar3XLocalSource();
}

bool MpqPriorityList::removeSource(const KUrl& url)
{
	Sources::iterator iterator = std::find_if(this->sources().begin(), this->sources().end(), [&url](Sources::const_reference ref){ return ref.url() == url; });

	if (iterator == this->sources().end())
	{
		return false;
	}

	this->m_sources.erase(iterator);

	return true;
}

bool MpqPriorityList::removeSource(Source &source)
{
	Sources::iterator iterator = sources().find(source);

	if (iterator == sources().end())
	{
		return false;
	}

	sources().erase(iterator);

	return true;
}

bool MpqPriorityList::removeWar3Source()
{
	KUrl url = war3Url();

	if (url.isEmpty())
	{
		return false;
	}

	return removeSource(url);
}


bool MpqPriorityList::removeWar3PatchSource()
{
	KUrl url = war3PatchUrl();

	if (url.isEmpty())
		return false;

	return removeSource(url);
}

bool MpqPriorityList::removeWar3XSource()
{
	KUrl url = war3XUrl();

	if (url.isEmpty())
		return false;

	return removeSource(url);
}

bool MpqPriorityList::removeWar3XLocalSource()
{
	KUrl url = war3XLocalUrl();

	if (url.isEmpty())
		return false;

	return removeSource(url);
}

bool MpqPriorityList::removeDefaultSources()
{
	return removeWar3Source() &&
	removeWar3PatchSource() &&
	removeWar3XSource() &&
	removeWar3XLocalSource();
}

namespace
{

void toRelativeUrl(QString &filePath)
{
	if (QDir::separator() != '\\')
	{
		filePath.replace(QChar('\\'),  QDir::separator());
	}
}

}

bool MpqPriorityList::download(const KUrl &src, QString &target, QWidget *window) const
{
	qDebug() << "Download: " << src.url();

	if (!src.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::download(src, target, window))
		{
			qDebug() << "Downloaded successfully into file " << target;

			return true;
		}
	}

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(Sources::const_reference entry, sources())
	{
		// this version does not rely on a KIO slave plugin which has to be installed. It uses an instance of the archive instead.
		if (entry.mpqArchive() != nullptr && entry.mpqArchive()->isOpen())
		{
			/*
			 * The file path has to be transformed into a Windows path which works with the MPQ archive.
			 */
			const QString archiveSrc = src.toLocalFile().replace('/', '\\');
			const std::string filePath = archiveSrc.toUtf8().constData();
			qDebug() << "Downloading from local MPQ archive " << entry.url() << " file: " << filePath.c_str();

			mpq::File file = entry.mpqArchive()->findFile(filePath, m_locale);

			if (file.isValid())
			{
				stringstream sstream;
				file.decompress(sstream);

				// Create the temporary file in the global temporary dir instead of the current working dir. Otherwise many temporary files might be created in the working dir.
				QTemporaryFile targetFile(QDir::temp().filePath(src.fileName()));
				// TODO this produces many many temporary files which have to be deleted at some point but that does happen when the MpqPriorityList is destroyed. Therefore many temporary files are produced.
				targetFile.setAutoRemove(false);

				if (targetFile.open())
				{
					/*
					 * The whole file from the MPQ archive has to be written into the temporary file which is stored in the hash of temporary files for later removal.
					 */
					const std::streamoff size = std::streamoff(wc3lib::endPosition(sstream));
					boost::scoped_array<byte> data(new byte[size]);
					sstream.read(data.get(), size);
					targetFile.write(data.get(), size);

					target = targetFile.fileName();
					/*
					 * Store temporary file path for deletion when the program exits.
					 */
					m_temporaryFiles.push_back(target);

					qDebug() << "Got temporary file " << target;

					return true;
				}
				else
				{
					qDebug() << "Error on opening target file " << target;
				}
			}
			else
			{
				qDebug() << "Did not find file " << filePath.c_str();
			}
		}
		// this version simply uses KIO and all available plugins/slaves
		else
		{
			QString archiveSrc = src.toLocalFile();
			toRelativeUrl(archiveSrc);

			// entry path can be a directory path or something like tar:/... or mpq:/...
			KUrl absoluteSource = entry.url();
			absoluteSource.addPath(archiveSrc);

			if (KIO::NetAccess::exists(absoluteSource, KIO::NetAccess::SourceSide, window))
			{
				qDebug() << "Found:" << absoluteSource << "and it should exist!";

				return KIO::NetAccess::download(absoluteSource, target, window);
			}
		}
	}

	qDebug() << "Downloaded failed";

	return false;
}

bool MpqPriorityList::upload(const QString &src, const KUrl &target, QWidget *window) const
{
	if (!target.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::upload(src, target, window))
		{
			return true;
		}
	}

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(Sources::const_reference entry, sources())
	{
		// TODO implement first version which does not rely on any slave

		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteTarget = entry.url();
		QString archiveTarget = target.toLocalFile();
		toRelativeUrl(archiveTarget);
		absoluteTarget.addPath(archiveTarget);

		if (KIO::NetAccess::upload(src, absoluteTarget, window))
		{
			return true;
		}
	}

	return false;
}

bool MpqPriorityList::mkdir(const KUrl &target, QWidget *window) const
{
	if (!target.isRelative()) // has protocol - is absolute
	{
		if (KIO::NetAccess::mkdir(target, window))
		{
			return true;
		}
	}

	// TODO only do this if it doesn't start with /
	// Since entries are ordered by priority highest priority entry should be checked first
	BOOST_REVERSE_FOREACH(Sources::const_reference entry, sources())
	{
		// entry path can be a directory path or something like tar:/... or mpq:/...
		KUrl absoluteTarget = entry.url();
		absoluteTarget.addPath(target.toLocalFile());

		if (KIO::NetAccess::mkdir(absoluteTarget, window))
		{
			return true;
		}
	}

	return false;
}

void MpqPriorityList::removeTempFile(const QString &name)
{
	if (this->m_temporaryFiles.contains(name))
	{
		QFile::remove(name);
		this->m_temporaryFiles.removeAll(name);
	}
	else
	{
		KIO::NetAccess::removeTempFile(name);
	}
}

bool MpqPriorityList::exists(const KUrl& url, QWidget *window) const
{
	if (url.isRelative())
	{
		// Since entries are ordered by priority highest priority entry should be checked first
		BOOST_REVERSE_FOREACH(Sources::const_reference entry, sources())
		{
			// this version does not rely on a KIO slave plugin which has to be installed. It uses an instance of the archive instead.
			if (entry.mpqArchive() != nullptr && entry.mpqArchive()->isOpen())
			{
				/*
				* The file path has to be transformed into a Windows path which works with the MPQ archive.
				*/
				const QString archiveSrc = url.toLocalFile().replace('/', '\\');
				const std::string filePath = archiveSrc.toUtf8().constData();
				qDebug() << "Downloading from local MPQ archive " << entry.url() << " file: " << filePath.c_str();

				mpq::File file = entry.mpqArchive()->findFile(filePath, m_locale);

				if (file.isValid())
				{
					return true;
				}
				else
				{
					qDebug() << "Did not find file " << filePath.c_str();
				}
			}
			// this version simply uses KIO and all available plugins/slaves
			else
			{
				// entry path can be a directory path or something like tar:/... or mpq:/...
				KUrl absoluteSource = entry.url();
				absoluteSource.addPath(url.toLocalFile());

				if (KIO::NetAccess::exists(absoluteSource, KIO::NetAccess::SourceSide, window))
				{
					qDebug() << "Found:" << absoluteSource << "and it should exist!";

					return true;
				}
			}
		}
	}
	// in an absolute URL everything has to be specified (protocol, host etc.)
	else
	{
		return KIO::NetAccess::exists(url, KIO::NetAccess::SourceSide, window);
	}

	return false;
}

void MpqPriorityList::readSettings(const QString& group)
{
	qDebug() << "Reading settings for group " << group;

	QSettings settings("wc3editor", "wc3editor");
	qDebug() << "Settings file name: " << settings.fileName();
	settings.beginGroup(group);
	const int size = settings.beginReadArray("entries");
	qDebug() << "Number of MPQ priority list entries: " << size;

	for (int i = 0; i < size; ++i)
	{
		qDebug() << "i is" << i;
		settings.setArrayIndex(i);
		const KUrl url = KUrl(settings.value("url").toByteArray());
		const int priority = settings.value("priority").toInt();
		qDebug() << "Loading source url " << url;
		qDebug() << "With priority " << priority;
		this->addSource(url, priority);
		qDebug() << "After adding source";
	}

	settings.endArray();
	settings.endGroup();
}

void MpqPriorityList::writeSettings(const QString& group)
{
	qDebug() << "Writing settings for group " << group;

	QSettings settings("wc3editor", "wc3editor");
	settings.beginGroup(group);
	settings.beginWriteArray("entries", sources().size());
	int i = 0;

	qDebug() << "Size 1: " << sources().size();

	BOOST_FOREACH(Sources::const_reference entry, sources())
	{
		settings.setArrayIndex(i);
		qDebug() << "Storing url " << entry.url();
		settings.setValue("url", entry.url().toEncoded());
		const int priority = boost::numeric_cast<int>(entry.priority());
		qDebug() << "Storing priority " << priority;
		settings.setValue("priority", priority);
		++i;
	}

	settings.endArray();
	settings.endGroup();
}

void MpqPriorityList::clear()
{
	sources().clear();
}

bool MpqPriorityList::configure(QWidget *parent)
{
	SourcesDialog sourcesDialog(this, parent);

	if (sourcesDialog.exec() == QDialog::Accepted)
	{
		return true;
	}

	return false;
}

MpqPriorityList::Sources& MpqPriorityList::sources()
{
	return m_sources;
}

}

}
