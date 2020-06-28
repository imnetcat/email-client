#include "file.h"
using namespace std;

Core::Filesystem::File::File(const Path& p) :
	CopyableFile(p),
	MoveableFile(p),
	ReadableFile(p),
	FileDescryptor(p) {}

Core::Filesystem::File::~File()
{
	close();
}

void Core::Filesystem::File::open()
{
	ReadableFile::open();
	// TODO: 
	// WriteableFile::open();
}
void Core::Filesystem::File::close()
{
	ReadableFile::close();
	// TODO: 
	// WriteableFile::close();
}