// filesys.cc
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"

// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1
//#define CurrentDirSector	2

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)

#define DirectoryFileSize 	(sizeof(DirectoryEntry) * NumDirEntries)

//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{
    DEBUG('f', "Initializing the file system.\n");
    if (format) {
        BitMap *freeMap = new BitMap(NumSectors);
        Directory *directory = new Directory(NumDirEntries);
        directory->table[0].inUse=TRUE;
        strncpy(directory->table[0].name, ".", FileNameMaxLen);
        directory->table[0].sector = DirectorySector;
        directory->table[1].inUse=TRUE;
        strncpy(directory->table[1].name, "..", FileNameMaxLen);
        directory->table[1].sector = DirectorySector;
	FileHeader *mapHdr = new FileHeader;
	FileHeader *dirHdr = new FileHeader;
  //FileHeader *currDirHdr = new FileHeader;

        DEBUG('f', "Formatting the file system.\n");

    // First, allocate space for FileHeaders for the directory and bitmap
    // (make sure no one else grabs these!)
	//freeMap->Mark(FreeMapSector);
	//freeMap->Mark(DirectorySector);
  freeMap->Mark(FreeMapSector);
  freeMap->Mark(DirectorySector);
  //freeMap->Mark(CurrentDirSector);

    // Second, allocate space for the data blocks containing the contents
    // of the directory and bitmap files.  There better be enough space!

	ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize,FALSE));
	ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize,TRUE));
	//ASSERT(currDirHdr->Allocate(freeMap, DirectoryFileSize,TRUE));
    // Flush the bitmap and directory FileHeaders back to disk
    // We need to do this before we can "Open" the file, since open
    // reads the file header off of disk (and currently the disk has garbage
    // on it!).

        DEBUG('f', "Writing headers back to disk.\n");
	mapHdr->WriteBack(FreeMapSector);
	dirHdr->WriteBack(DirectorySector);
  //currDirHdr->WriteBack(CurrentDirSector);
    // OK to open the bitmap and directory files now
    // The file system operations assume these two files are left open
    // while Nachos is running.

        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);
        currentDirFile = new OpenFile(DirectorySector);
        for(int i = 0; i< NumMaxOpenedFiles; i++){
          fileTable[i] = NULL;
        }
    // Once we have the files "open", we can write the initial version
    // of each file back to disk.  The directory at this point is completely
    // empty; but the bitmap has been changed to reflect the fact that
    // sectors on the disk have been allocated for the file headers and
    // to hold the file data for the directory and bitmap.

        DEBUG('f', "Writing bitmap and directory back to disk.\n");
	freeMap->WriteBack(freeMapFile);	 // flush changes to disk
	directory->WriteBack(directoryFile);
  //directory->WriteBack(currentDirFile);

	if (DebugIsEnabled('f')) {
	    freeMap->Print();
	    directory->Print();

  delete freeMap;
	delete directory;
	delete mapHdr;
	delete dirHdr;
  //delete currDirHdr;
	}
    } else {
    // if we are not formatting the disk, just open the files representing
    // the bitmap and directory; these are left open while Nachos is running
        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);
        currentDirFile = new OpenFile(DirectorySector);
        for(int i = 0; i< NumMaxOpenedFiles; i++){
          fileTable[i] = NULL;
        }
    }
}

//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------

bool
FileSystem::CreateFile(const char *name, int initialSize)
{
    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    bool success;

    DEBUG('f', "Creating file %s, size %d\n", name, initialSize);

    directory = new Directory(NumDirEntries);
    directory->FetchFrom(currentDirFile);

    if (directory->Find(name) != -1)
      success = FALSE;			// file is already in directory
    else {
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	// find a sector to hold the file header
    	if (sector == -1)
            success = FALSE;		// no free block for file header
      else if (!directory->Add(name, sector))
            success = FALSE;	// no space in directory
	    else {
    	    hdr = new FileHeader;
	         if (!hdr->Allocate(freeMap, initialSize, FALSE))
            	success = FALSE;	// no space on disk for data
	         else {
	    	       success = TRUE;
		// everthing worked, flush all changes back to disk
    	    	hdr->WriteBack(sector);
    	    	directory->WriteBack(currentDirFile);
    	    	freeMap->WriteBack(freeMapFile);
	         }
        delete hdr;
	    }
      delete freeMap;
    }
    delete directory;
    return success;
}

// Create a new directory in the current directory

bool
FileSystem::CreateDir(const char *name)
{
    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    bool success;

    DEBUG('f', "Creating a directory %s\n", name);

    directory = new Directory(NumDirEntries);
    directory->FetchFrom(currentDirFile);

    if (directory->Find(name) != -1)
      success = FALSE;			// file is already in directory
    else {
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	// find a sector to hold the file header
    	if (sector == -1)
            success = FALSE;		// no free block for file header
      else if (!directory->Add(name, sector))
            success = FALSE;	// no space in directory
	    else {
    	    hdr = new FileHeader;
	         if (!hdr->Allocate(freeMap, DirectoryFileSize,TRUE))
            	success = FALSE;	// no space on disk for data
	         else {
	    	       success = TRUE;
		// everthing worked, flush all changes back to disk
    	    	hdr->WriteBack(sector);
            Directory * createdDir = new Directory(NumDirEntries);
            OpenFile * fileNewDir = new OpenFile(sector);
            createdDir->WriteBack(fileNewDir);
            createdDir->FetchFrom(fileNewDir);
            createdDir->table[0].inUse=TRUE;
            strncpy(createdDir->table[0].name, ".", FileNameMaxLen);
            createdDir->table[0].sector = sector;
            createdDir->table[1].inUse=TRUE;
            strncpy(createdDir->table[1].name, "..", FileNameMaxLen);
            createdDir->table[1].sector = directory->table[0].sector;
            createdDir->WriteBack(fileNewDir);
    	    	directory->WriteBack(currentDirFile);
    	    	freeMap->WriteBack(freeMapFile);
            delete createdDir;
	         }
        delete hdr;
	    }
      delete freeMap;
    }
    delete directory;
    return success;
}





bool
FileSystem::AddToFdTable(OpenFile * openFile)
{
  if(openFile->GetHeader()->isDir){// On vérifie que le fichier ne soit pas un répertoire
    return FALSE;
  }


  for (int i = 0; i<NumMaxOpenedFiles;i++){
    //on vérifie si le fichier est déja ouvert
    if (fileTable[i] != NULL){
      if (fileTable[i]->GetHeader()->GetFd() == openFile->GetHeader()->GetFd()){
        return FALSE;
      }
    }
  }

  for (int i = 0; i<NumMaxOpenedFiles;i++){
    if (fileTable[i] == NULL){ // On vérifie qu'il reste une place dans la table
      fileTable[i] = openFile; // On l'ajoute à la table
      //currentThread->threadFileTable[i] = openFile;
      return TRUE;
    }
  }
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.
//	To open a file:
//	  Find the location of the file's header, using the directory
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------

OpenFile *
FileSystem::Open(const char *name)
{
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    DEBUG('f', "Opening file %s\n", name);
    directory->FetchFrom(currentDirFile);
    sector = directory->Find(name);

    if (sector >= 0)
	    openFile = new OpenFile(sector);	// name was found in directory



    delete directory;
    return openFile;

}


void FileSystem::Close(int fd){
  for (int i = 0; i<NumMaxOpenedFiles;i++){
    if(fileTable[i]!=NULL){
      if (fileTable[i]->GetHeader()->GetFd() == fd){ //on vérifie si le fichier est déja ouvert
        //if(currentThread->threadFileTable[i]->GetHeader()->GetFd() == fd){//on vérifie que le fichier est bien ouvert par le thread courant
          fileTable[i] = NULL;
          //currentThread->threadFileTable[i] = NULL;
          break;
      //}
      }
    }
  }
}


//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"name" -- the text name of the file to be removed
//----------------------------------------------------------------------

bool
FileSystem::Remove(const char *name)
{
    Directory *directory;
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;


    directory = new Directory(NumDirEntries);
    directory->FetchFrom(currentDirFile);
    sector = directory->Find(name);
    if (sector == -1) {
       delete directory;
       return FALSE;			 // file not found
    }
    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);
    //Close(fileHdr->GetFd());

    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);

    fileHdr->Deallocate(freeMap);  		// remove data blocks
    freeMap->Clear(sector);			// remove header block
    bool test = directory->Remove(name);

    freeMap->WriteBack(freeMapFile);		// flush to disk
    directory->WriteBack(currentDirFile);        // flush to disk
    delete fileHdr;
    delete directory;
    delete freeMap;
    return test;
}

char* FileSystem::Pathing(char* path){
  int taille = strlen(path);
  int pos =0;
  if(path[pos]=='/'){
    delete currentDirFile;
    currentDirFile = new OpenFile(1);
    pos ++;
  }
  char * buff = new char[10];
  for(int j = 0 ; j<10 ; j++){
    buff[j]='\0';
  }
  int i =0;

  for(;pos<taille;pos++){
    if(path[pos] == '/'){
      this->Move(buff);
      for(int j = 0 ; j<10 ; j++){
        buff[j]='\0';
      }
      i = 0;
      pos++;
    }
    buff[i]=path[pos];
    i++;
  }

  return buff;
}

bool
FileSystem::Move(const char *name)
{
    Directory *directory;
    OpenFile *fileNextDir;
    int sector;
    int index;
    directory = new Directory(NumDirEntries);
    directory->FetchFrom(currentDirFile);
    sector = directory->Find(name);
    if (sector == -1) {
       delete directory;
       return FALSE;			 // file not found
    }

    Directory *dirNext = new Directory(NumDirEntries);
    fileNextDir = new OpenFile(sector);
    dirNext->FetchFrom(fileNextDir);
    delete currentDirFile;
    currentDirFile = new OpenFile(sector);
    dirNext->WriteBack(currentDirFile);        // flush to disk
    delete fileNextDir;
    delete dirNext;
    delete directory;
    return TRUE;
}












//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List()
{
    Directory *directory = new Directory(NumDirEntries);

    directory->FetchFrom(currentDirFile);
    directory->List();
    delete directory;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void
FileSystem::Print()
{
    FileHeader *bitHdr = new FileHeader;
    FileHeader *dirHdr = new FileHeader;
    BitMap *freeMap = new BitMap(NumSectors);
    Directory *directory = new Directory(NumDirEntries);

    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    directory->FetchFrom(currentDirFile);
    directory->Print();

    printf("Directory file header:\n");
    dirHdr->FetchFrom(directory->table[0].sector);
    dirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();


    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
}
