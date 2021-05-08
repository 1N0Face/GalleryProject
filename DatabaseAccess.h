#pragma once
#include "IDataAccess.h"
#include <list>
#include "Album.h"
#include "User.h"
#include "sqlite3.h"
#include <string.h>
#include <io.h>
#include "MyException.h"
#define ERROR_CODE -1
#define UNDEFINED -1
#define TWO 2
#define THREE 3
#define FOUR 4
class DatabaseAccess: public IDataAccess
{
public:

	// album related
	virtual const std::list<Album> getAlbums();
	virtual const std::list<Album> getAlbumsOfUser(const User & user);
	virtual void createAlbum(const Album & album);
	virtual void deleteAlbum(const std::string & albumName, int userId);
	virtual bool doesAlbumExists(const std::string & albumName, int userId);
	virtual Album openAlbum(const std::string & albumName);
	virtual void closeAlbum(Album & pAlbum);
	virtual void printAlbums();

	// picture related
	bool doesPictureExists(int picId); // added by my own because of their mistake in adding pictures
	virtual void addPictureToAlbumByName(const std::string & albumName, const Picture & picture);
	virtual void removePictureFromAlbumByName(const std::string & albumName, const std::string & pictureName);
	virtual void tagUserInPicture(const std::string & albumName, const std::string & pictureName, int userId);
	virtual void untagUserInPicture(const std::string & albumName, const std::string & pictureName, int userId);

	// user related
	virtual void printUsers();
	virtual User getUser(int userId);
	virtual void createUser(User & user);
	virtual void deleteUser(const User & user);
	virtual bool doesUserExists(int userId);


	// user statistics
	virtual int countAlbumsOwnedOfUser(const User & user);
	virtual int countAlbumsTaggedOfUser(const User & user);
	virtual int countTagsOfUser(const User & user);
	virtual float averageTagsPerAlbumOfUser(const User & user);

	// queries
	virtual User getTopTaggedUser();
	virtual Picture getTopTaggedPicture();
	virtual std::list<Picture> getTaggedPicturesOfUser(const User & user);

	virtual bool open();
	virtual void close();
	virtual void clear();
private:
	sqlite3* _db;
};