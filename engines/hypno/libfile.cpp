/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "hypno/libfile.h"
#include "hypno/hypno.h"

namespace Hypno {

LibFile::LibFile() : Common::Archive() {
}

LibFile::~LibFile() {
}

bool LibFile::open(const Common::String &prefix, const Common::String &filename) {
	_prefix = prefix;
	Common::File libfile;
	if (!libfile.open(filename))
		error("Failed to open %s", filename.c_str());
	byte b;
	uint32 size;
	FileEntry f;
	f.data.push_back(0);
	do {
		f.name = "";
		f.data.clear();
		for (uint32 i = 0; i < 12; i++) {
			b = libfile.readByte();
			if (b != 0x96 && b != 0x0)
				f.name += tolower(char(b));
		}
		debugC(1, kHypnoDebugParser, "file: %s", f.name.c_str());
		uint32 start = libfile.readUint32LE();
		size = libfile.readUint32LE();
		libfile.readUint32LE(); // some field?

		uint32 pos = libfile.pos();
		libfile.seek(start);

		for (uint32 i = 0; i < size; i++) {
			b = libfile.readByte();
			if (b != '\n')
				b = b ^ 0xfe;
			f.data.push_back(b);
		}
		debugC(1, kHypnoDebugParser, "size: %d", f.data.size());
		libfile.seek(pos);
		if (size > 0)
			_fileEntries.push_back(f);

	} while (size > 0);
	return true;
}

const FileEntry *LibFile::getEntry(const Common::Path &path) const {
	Common::String name = path.toString();
	for (Common::Array<FileEntry>::const_iterator it = _fileEntries.begin(); it != _fileEntries.end(); ++it) {
		if (((_prefix + it->name).equalsIgnoreCase(name)) || it->name.equalsIgnoreCase(name))
			return it;
	}

	return nullptr;
}

void LibFile::close() {
	_fileEntries.clear();
}

bool LibFile::hasFile(const Common::Path &path) const {
	Common::String name = path.toString();
	return getEntry(name) != nullptr;
}

int LibFile::listMembers(Common::ArchiveMemberList &list) const {
	list.clear();
	for (Common::Array<FileEntry>::const_iterator it = _fileEntries.begin(); it != _fileEntries.end(); ++it)
		list.push_back(getMember(it->name));

	return list.size();
}

const Common::ArchiveMemberPtr LibFile::getMember(const Common::Path &path) const {
	Common::String name = path.toString();
	return Common::ArchiveMemberPtr(new Common::GenericArchiveMember(name, this));
}

Common::SeekableReadStream *LibFile::createReadStreamForMember(const Common::Path &path) const {
	Common::String name = path.toString();
	const FileEntry *entry = getEntry(name);
	Common::MemoryReadStream *stream = nullptr;
	if (entry != nullptr)
		stream = new Common::MemoryReadStream(entry->data.data(), entry->data.size());

	return stream;
}

} // namespace Hypno

