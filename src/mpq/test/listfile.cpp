/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#define BOOST_TEST_MODULE ListfileTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>

#include <boost/foreach.hpp>

#include "../listfile.hpp"
#include "../archive.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;

/*
 2.7 LISTFILE
 The listfile is a very simple extension to the MoPaQ format that contains the file paths of (most) files in the archive. The languages and platforms of the files are not stored in the listfile. The listfile is contained in the file "(listfile)" (default language and platform), and is simply a text file with file paths separated by ';', 0Dh, 0Ah, or some combination of these. The file "(listfile)" may not be listed in the listfile.
 */
BOOST_AUTO_TEST_CASE(ListfileEntriesTest)
{
	stringstream sstream;
	sstream <<
	"bla1;"
	"bla2\n"
	"bla3\r"
	// ;
	"bla4;\n\r"
	"bla5;\r\n"
	"bla6;\n"
	"bla7;\r"
	// line break
	"bla8\n\r;"
	"bla9\n;\r"
	"bla10\n;"
	"bla11\n\r"
	// carriage return
	"bla12\r\n;"
	"bla13\r;\n"
	"bla14\r\n"
	"bla15\r;"
	"end"
	;

	mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 16);
	BOOST_REQUIRE(entries[0] == "bla1");
	BOOST_REQUIRE(entries[1] == "bla2");
	BOOST_REQUIRE(entries[2] == "bla3");
	BOOST_REQUIRE(entries[3] == "bla4");
	BOOST_REQUIRE(entries[4] == "bla5");
	BOOST_REQUIRE(entries[5] == "bla6");
	BOOST_REQUIRE(entries[6] == "bla7");
	BOOST_REQUIRE(entries[7] == "bla8");
	BOOST_REQUIRE(entries[8] == "bla9");
	BOOST_REQUIRE(entries[9] == "bla10");
	BOOST_REQUIRE(entries[10] == "bla11");
	BOOST_REQUIRE(entries[11] == "bla12");
	BOOST_REQUIRE(entries[12] == "bla13");
	BOOST_REQUIRE(entries[13] == "bla14");
	BOOST_REQUIRE(entries[14] == "bla15");
	BOOST_REQUIRE(entries.back() == "end");
}

BOOST_AUTO_TEST_CASE(ListfileContentTest)
{
	mpq::Listfile::Entries entries;
	entries.push_back("bla1");
	entries.push_back("bla2");
	entries.push_back("bla3");
	entries.push_back("bla4");
	entries.push_back("bla5");
	entries.push_back("bla6");
	entries.push_back("bla7");
	entries.push_back("bla8");
	entries.push_back("bla9");
	entries.push_back("bla10");
	entries.push_back("bla11");
	entries.push_back("bla12");
	entries.push_back("bla13");
	entries.push_back("bla14");
	entries.push_back("bla15");
	entries.push_back("end");

	const string content = mpq::Listfile::content(entries);

	entries = mpq::Listfile::entries(content);
	BOOST_REQUIRE(entries.size() == 16);
	BOOST_REQUIRE(entries[0] == "bla1");
	BOOST_REQUIRE(entries[1] == "bla2");
	BOOST_REQUIRE(entries[2] == "bla3");
	BOOST_REQUIRE(entries[3] == "bla4");
	BOOST_REQUIRE(entries[4] == "bla5");
	BOOST_REQUIRE(entries[5] == "bla6");
	BOOST_REQUIRE(entries[6] == "bla7");
	BOOST_REQUIRE(entries[7] == "bla8");
	BOOST_REQUIRE(entries[8] == "bla9");
	BOOST_REQUIRE(entries[9] == "bla10");
	BOOST_REQUIRE(entries[10] == "bla11");
	BOOST_REQUIRE(entries[11] == "bla12");
	BOOST_REQUIRE(entries[12] == "bla13");
	BOOST_REQUIRE(entries[13] == "bla14");
	BOOST_REQUIRE(entries[14] == "bla15");
	BOOST_REQUIRE(entries.back() == "end");
}

BOOST_AUTO_TEST_CASE(CaseFileSensitiveEntries)
{
	stringstream sstream;
	sstream <<
	"Abilities"
	";abilities"
	";abilities\\Hans" // must be excluded since it is not recursive
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 3);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveFileEntries(entries, "", false);

	/*
	for (std::size_t i = 0; i < uniqueEntries.size(); ++i)
	{
		std::cerr << uniqueEntries[i] << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 2);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities");
	BOOST_REQUIRE(uniqueEntries[1] == "abilities");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveFileEntriesRecursive)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans"
	";abilities\\Peter"
	";abilities\\PeTeR"
	";abILIties\\UI\\test"
	";abilities\\ui\\test2"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 5);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveFileEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 5);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI\\test");
	BOOST_REQUIRE(uniqueEntries[4] == "Abilities\\UI\\test2");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveFileEntriesRecursiveExtension)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans"
	";abilities\\Peter"
	";abilities\\PeTeR"
	";abILIties\\UI\\test"
	";abilities\\ui\\test2"
	";abilities\\ui\\test3.txt"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 6);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveFileEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 6);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI\\test");
	BOOST_REQUIRE(uniqueEntries[4] == "Abilities\\UI\\test2");
	BOOST_REQUIRE(uniqueEntries[5] == "Abilities\\UI\\test3.txt");
}

BOOST_AUTO_TEST_CASE(CaseSensitiveDirEntriesRecursive)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans\\bla"
	";abilities\\Peter\\blu"
	";abilities\\Peter\\bla"
	";abilities\\PeTeR\\bli"
	";abILIties\\UI\\test"
	";abilities\\ui\\test2"
	";abilities\\ui\\test3.txt"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 7);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveDirEntries(entries, "", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 4);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[3] == "Abilities\\UI");
}

/*
 * Tests listing directory dir entries with the prefix "Abilities\\" but without recursion.
 * Therefore the directory "testDir" should not be listed at all.
 */
BOOST_AUTO_TEST_CASE(CaseSensitiveDirEntriesWithPrefix)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans\\bla"
	";abilities\\Peter\\blu"
	";abilities\\PeTeR\\bli"
	";abILIties\\UI\\test"
	";abILIties\\UI\\test1"
	";abilities\\ui\\test2"
	";abilities\\ui\\test3.txt"
	/*
	 * This is a recursive sub directory called "testDir" which should not be listed since recursive is "false".
	 */
	";abilities\\ui\\testDir\\test"
	";test\\ui\\test3.txt"
	";test\\ui\\anothersubdir\\myfile.txt"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 10);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveDirEntries(entries, "Abilities\\", false);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 3);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities\\Hans");
	BOOST_REQUIRE(uniqueEntries[1] == "Abilities\\Peter");
	BOOST_REQUIRE(uniqueEntries[2] == "Abilities\\UI");
}

/*
 * Tests if all directory entries are detected without recursion and prefix.
 * If no recursion and no prefix is used it must list all directories in the top level directory.
 */
BOOST_AUTO_TEST_CASE(CaseSensitiveDirEntriesWithEmptyPrefix)
{
	stringstream sstream;
	sstream <<
	"Abilities\\Hans\\bla"
	";abilities\\Peter\\blu"
	";abilities\\PeTeR\\bli"
	";abILIties\\UI\\test"
	";abILIties\\UI\\test1"
	";abilities\\ui\\test2"
	";abilities\\ui\\test3.txt"
	";test\\ui\\test3.txt"
	";test\\ui\\anothersubdir\\myfile.txt"
	;

	const mpq::Listfile::Entries entries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(entries.size() == 9);

	const mpq::Listfile::Entries uniqueEntries = mpq::Listfile::caseSensitiveDirEntries(entries, "", false);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, uniqueEntries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(uniqueEntries.size() == 2);
	BOOST_REQUIRE(uniqueEntries[0] == "Abilities");
	BOOST_REQUIRE(uniqueEntries[1] == "test");
}

BOOST_AUTO_TEST_CASE(ExistingEntriesWithPrefix)
{
	mpq::Archive archive;
	bool success = true;

	try
	{
		archive.open("mpqmaster_mpq1_no_extended_attributes.mpq");
	}
	catch (Exception &e)
	{
		std::cerr << e.what() << std::endl;

		success = false;
	}

	BOOST_REQUIRE(success);
	stringstream sstream;
	sstream <<
	"test\\testfile.txt"
	";testfile.txt"
	;

	const mpq::Listfile::Entries listfileEntries = mpq::Listfile::entries(sstream.str());

	BOOST_REQUIRE(listfileEntries.size() == 2);

	const mpq::Listfile::Entries entries = mpq::Listfile::existingEntries(listfileEntries, archive, "test\\", true);

	/*
	BOOST_FOREACH(mpq::Listfile::Entries::const_reference ref, entries)
	{
		std::cerr << ref << std::endl;
	}
	*/

	BOOST_REQUIRE(entries.size() == 1);

	BOOST_REQUIRE(entries[0] == "test\\testfile.txt");
}

BOOST_AUTO_TEST_CASE(DirPath)
{
	BOOST_CHECK_EQUAL(mpq::Listfile::dirPath(""), "");
	BOOST_CHECK_EQUAL(mpq::Listfile::dirPath("Bla"), "");
	BOOST_CHECK_EQUAL(mpq::Listfile::dirPath("Bla\\bla"), "Bla");
	BOOST_CHECK_EQUAL(mpq::Listfile::dirPath("Bla\\bla\\bla.txt"), "Bla\\bla");
}

BOOST_AUTO_TEST_CASE(FileName)
{
	BOOST_CHECK_EQUAL(mpq::Listfile::fileName(""), "");
	const string filePath0 = "test.txt";
	BOOST_CHECK_EQUAL(mpq::Listfile::fileName(filePath0), "test.txt");
	const string filePath1 = "UI\\peter\\test.txt";
	BOOST_CHECK_EQUAL(mpq::Listfile::fileName(filePath1), "test.txt");
	const string filePath2 = "UI\\peter\\";
	BOOST_CHECK_EQUAL(mpq::Listfile::fileName(filePath2), "");
}
