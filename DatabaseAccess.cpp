#include "DatabaseAccess.h"
//This function opens the database "galleryDB.sqlite"
bool DatabaseAccess::open()
{
	std::string dbFileName = "galleryDB.sqlite";
	int res = sqlite3_open(dbFileName.c_str(), &_db);
	if (res != SQLITE_OK) {
		_db = nullptr;
		throw MyException("Error: Opening the database!");
	}
	return true;
}
//This function closes the database "MyDb.sqlite"
void DatabaseAccess::close()
{
	sqlite3_close(_db);
	_db = nullptr;
}
void DatabaseAccess::clear() {};
void DatabaseAccess::closeAlbum(Album& album) {};
//This function deletes the album and the pictures in this album and their tags
void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID = (SELECT ID FROM PICTURES WHERE ALBUM_ID LIKE (SELECT ID FROM ALBUMS WHERE NAME LIKE ";
		sqlStatement += "\"" + albumName + "\"));";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: deleting the album!");
		sqlStatement = "DELETE FROM Pictures WHERE ALBUM_ID LIKE (SELECT ID FROM Albums WHERE NAME = ";
		sqlStatement += "\"" + albumName + "\");";
		checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: deleting the album!");
		sqlStatement = "DELETE FROM Albums WHERE Name LIKE ";
		sqlStatement += "\"" + albumName + "\"" + " AND User_ID = " + std::to_string(userId) + ";";
		checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: deleting the album!");
	}
}
//This function is a call back which checks if something exists
int checkExists(void* data, int argc, char** argv, char** azColName)
{
	int* flag = (int*)data;
	if (argc != 0)
	{
		*flag = 1;// make the existsFlag be 1
		return 1;
	}
	return 0;
}
//This function checks if a picture exists
bool DatabaseAccess::doesPictureExists(int picId)
{
	if (_db)
	{
		int existsFlag = 0;
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT * FROM PICTURES WHERE ID = " + std::to_string(picId) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), checkExists, &existsFlag, &errMessage);
		if (existsFlag)
			return true;
	}
	return false;
}
//This function adds a picture to a specific album
void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	if (_db)
	{
		int existsFlag = 0;
		char* errMessage = nullptr;
		std::string toAdd = "\"" + picture.getName() + "\", " + "\"" + picture.getPath() + "\", " + "\"" + picture.getCreationDate() + "\", " + "(SELECT ID FROM Albums WHERE Name LIKE " +
			"\"" + albumName + "\"));";
		std::string sqlStatement = "INSERT INTO Pictures VALUES(" + std::to_string(picture.getId()) + ", ";
		if (doesPictureExists(picture.getId()))
			sqlStatement = "INSERT INTO Pictures VALUES((SELECT ID FROM PICTURES ORDER BY ID DESC limit 1)+1, ";
		sqlStatement += toAdd;
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: adding picture to the album!");
	}
}
//This function removes the picture from a specific album
void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "DELETE FROM TAGS WHERE TAGS.PICTURE_ID = (SELECT PICTURES.ID FROM PICTURES WHERE PICTURES.NAME = ";
		sqlStatement += "\"" + pictureName + "\");";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: removing picture!");
		sqlStatement = "DELETE FROM Pictures WHERE Name = \"" + pictureName + "\" AND Album_ID = " + "(SELECT ID FROM Albums WHERE Name LIKE " + "\"" + albumName + "\");";
		checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: removing picture!");
	}
}
//This function tags user in a picture from the album
void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	if (_db)
	{
		int picId = 0;
		char* errMessage = nullptr;
		std::string sqlStatement = "INSERT INTO Tags VALUES ((SELECT ID FROM PICTURES WHERE NAME = ";
		sqlStatement += "\"" + pictureName + "\"" + " AND ALBUM_ID LIKE (SELECT ID FROM ALBUMS WHERE NAME = \"" + albumName + "\")) , " + std::to_string(userId) + ");";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: tagging the user!");
	}
}
//This function untags user in a picture from the album
void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "DELETE FROM Tags WHERE Picture_ID = (SELECT ID FROM PICTURES WHERE NAME = ";
		sqlStatement += "\"" + pictureName + "\"" + ")" + "AND User_ID = " + std::to_string(userId) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: untagging the user!");
	}
}
//This function delets the user, his tags, his pictures and his albums
void DatabaseAccess::deleteUser(const User& user)
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "DELETE FROM TAGS WHERE PICTURE_ID = (SELECT ID FROM PICTURES WHERE ALBUM_ID LIKE (SELECT ID FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + "));";
		sqlStatement += "DELETE FROM PICTURES WHERE ALBUM_ID = (SELECT ID FROM ALBUMS WHERE USER_ID LIKE " + std::to_string(user.getId()) + ");DELETE FROM ALBUMS WHERE USER_ID = " +
			std::to_string(user.getId()) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: deleting user!");
		sqlStatement = "DELETE FROM TAGS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
		checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: deleting the user!");
		sqlStatement = "DELETE FROM Users WHERE ID = " + std::to_string(user.getId()) + " AND Name = \"" + user.getName() + "\";";
		checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: deleting user!");
	}
}
//This function creats an album
void DatabaseAccess::createAlbum(const Album& album)
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "INSERT INTO Albums VALUES ((SELECT ID FROM PICTURES ORDER BY ID DESC limit 1)+1, \"" +
			album.getName() + "\", " + std::to_string(album.getOwnerId()) + ", \"" + album.getCreationDate() + "\");";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: creating the album!");
	}
}
//this function checks if a specific album exist
bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	if (_db)
	{
		char* errMessage = nullptr;
		int existsFlag = 0;
		std::string sqlStatement = "SELECT * FROM Albums WHERE Name LIKE \"" + albumName + "\";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), checkExists, &existsFlag, &errMessage);
		if (existsFlag)
			return true;
	}
	return false;
}
//this function is a call back which prints users list
int callBackPrintUser(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << azColName[i] << ": " << argv[i] << " ";
		if (std::string(azColName[i]) == "NAME")
			std::cout << std::endl;
	}
	return 0;
}
//this function is a call back which prints albums list
int callBackPrintAlbum(void* data, int argc, char** argv, char** azColName)
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << azColName[i] << ": " << argv[i] << " ";
		if (std::string(azColName[i]) == "CREATION_DATE")
			std::cout << std::endl;
	}
	return 0;
}
//This function prints created albums
void DatabaseAccess::printAlbums()
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT * FROM Albums;";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackPrintAlbum, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: printing albums!");
	}
}
//This function prints created users
void DatabaseAccess::printUsers()
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT * FROM Users;";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackPrintUser, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: printing users!");
	}
}
//This function checks if user exist
bool DatabaseAccess::doesUserExists(int userId)
{
	if (_db)
	{
		int existsFlag = 0;
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT * FROM USERS WHERE ID = " + std::to_string(userId) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), checkExists, &existsFlag, &errMessage);
		if (existsFlag)
			return true;
	}
	return false;
}
//This function is a call back which counts the first field
int countBack(void* data, int argc, char** argv, char** azColName)
{
	int* flag = (int*)data;
	*flag = atoi(argv[0]); // make the count to the counter in the func
	return atoi(argv[0]);
}
//This function counts owned albums of a user
int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	int counter = 0;
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT Count(*) FROM Users INNER JOIN Albums ON Users.id = Albums.User_ID WHERE Users.id = " +
			std::to_string(user.getId()) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), countBack, &counter, &errMessage);
		if (checkRes != SQLITE_OK && std::string(errMessage) != "query aborted") // if there was an error in the sqlite
			throw MyException("Error: counting albums!");
	}
	return counter;
}
//This function counts albums that the user is tagged in
int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	int counter = 0;
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT COUNT(DISTINCT albums.name) FROM ((Tags INNER JOIN Pictures ON Tags.Picture_ID = Pictures.ID)INNER JOIN Albums ON Pictures.ALBUM_ID = Albums.ID) WHERE Tags.User_ID = "
			+ std::to_string(user.getId()) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), countBack, &counter, &errMessage);
		if (checkRes != SQLITE_OK && std::string(errMessage) != "query aborted") // if there was an error in the sqlite
			throw MyException("Error: counting albums!");
	}
	return counter;
}
//This function counts tags of user
int DatabaseAccess::countTagsOfUser(const User& user)
{
	int counter = 0;
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT COUNT(*) FROM Tags INNER JOIN Pictures ON Tags.Picture_ID = Pictures.ID WHERE Tags.User_ID = "
			+ std::to_string(user.getId()) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), countBack, &counter, &errMessage);
		if (checkRes != SQLITE_OK && std::string(errMessage) != "query aborted") // if there was an error in the sqlite
			throw MyException("Error: counting tags of the user!");
	}
	return counter;
}
//This function calculates averageTagsPerAlbumOfUser
float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	return countTagsOfUser(user) / countAlbumsOwnedOfUser(user);
}
//This function is a call back function which gives the top tagged user
int toptaggedUser(void* data, int argc, char** argv, char** azColName)
{
	//data syntax: ID|NAME|COUNT
	User* bestTaggedUser = reinterpret_cast<User*>(data);
	bestTaggedUser->setId(atoi(argv[0])); // set the first user id to the top tagged
	bestTaggedUser->setName(argv[1]);// set the first user name to the top tagged
	int bestTags = atoi(argv[TWO]); //set to the first user tags count
	for (int i = THREE; i < argc; i += THREE)
	{
		if (std::string(azColName[i]) == "ID")
		{
			if (bestTags < atoi((argv[i + TWO])))
			{
				bestTaggedUser->setId(atoi(argv[i]));
				bestTaggedUser->setName(argv[i + 1]);
				bestTags = atoi((argv[i + TWO]));
			}
		}
	}
	return 0;
}
//This function gives the top tagged user in pictures
User DatabaseAccess::getTopTaggedUser()
{
	if (_db)
	{
		User* bestTaggedUser = new User(UNDEFINED, "");
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT ID,NAME, (SELECT COUNT(*) FROM TAGS INNER JOIN Pictures ON TAGS.Picture_ID = Pictures.ID WHERE Tags.User_ID = Users.ID) FROM Users;";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), toptaggedUser, bestTaggedUser, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: getting top tagged user!");
		User ans(bestTaggedUser->getId(), bestTaggedUser->getName());
		delete bestTaggedUser;
		return ans;
	}
}
//This function is a call back function which gives the top tagged pic
int toptaggedPic(void* data, int argc, char** argv, char** azColName)
{
	Picture* bestTaggedPic = reinterpret_cast<Picture*>(data);
	bestTaggedPic->setId(atoi(argv[0]));
	bestTaggedPic->setName(argv[1]);
	bestTaggedPic->setPath(argv[TWO]);
	bestTaggedPic->setCreationDate(argv[THREE]);
	return 0;
}
//This function gives the top tagged pic
Picture DatabaseAccess::getTopTaggedPicture()
{
	if (_db)
	{
		Picture* bestTaggedPic = new Picture(UNDEFINED, "");
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT * FROM PICTURES GROUP BY ID ORDER BY (SELECT count(*) FROM TAGS WHERE Tags.PICTURE_ID = PICTURES.ID) DESC LIMIT 1;";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), toptaggedPic, bestTaggedPic, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: getting top tagged picture!");
		Picture ans(bestTaggedPic->getId(), bestTaggedPic->getName(), bestTaggedPic->getPath(), bestTaggedPic->getCreationDate());
		delete bestTaggedPic;
		return ans;
	}
}
//This function is a call back which gives the albums
int callBackGetAlbums(void* data, int argc, char** argv, char** azColName)
{
	std::list<Album>* temp = reinterpret_cast<std::list<Album>*>(data);
	Album album;
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "USER_ID")
			album.setOwner(atoi(argv[i]));
		if (std::string(azColName[i]) == "NAME")
			album.setName(argv[i]);
		if (std::string(azColName[i]) == "CREATION_DATE")
		{
			album.setCreationDate(argv[i]);
			temp->push_back(album);
		}
	}
	return 0;
}
//This function gives the list of albums
const std::list<Album> DatabaseAccess::getAlbums()
{
	if (_db)
	{
		std::list<Album>* saver = new std::list<Album>;
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT USER_ID,NAME,CREATION_DATE FROM Albums;";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackGetAlbums, saver, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: getting albums!");
		std::list<Album> ans(*saver);
		delete saver;
		return ans;
	}
}
//This function gives the albums of the user
const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	if (_db)
	{
		std::list<Album>* saver = new std::list<Album>;
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT USER_ID,NAME,CREATION_DATE FROM Albums WHERE USER_ID = " + std::to_string(user.getId()) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackGetAlbums, saver, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: getting albums of the user!");
		std::list<Album> ans(*saver);
		delete saver;
		return ans;
	}
}
//This function is a call back which gives the user
int callBackGetUser(void* data, int argc, char** argv, char** azColName)
{
	User* temp = reinterpret_cast<User*>(data);
	if (argc == TWO)
	{
		temp->setId(atoi(argv[0]));
		temp->setName(argv[1]);
	}
	return 0;
}
//This function which gives the user according to their userId
User DatabaseAccess::getUser(int userId)
{
	if (_db)
	{
		User* saver = new User(UNDEFINED, "");
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT ID,NAME FROM Users WHERE ID = " + std::to_string(userId) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackGetUser, saver, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: getting the user!");
		User ans(saver->getId(), saver->getName());
		delete saver;
		return ans;
	}
}
//This function creates a user
void DatabaseAccess::createUser(User& user)
{
	if (_db)
	{
		char* errMessage = nullptr;
		std::string sqlStatement = "INSERT INTO Users VALUES(" + std::to_string(user.getId()) + ", \"" + user.getName() + "\");";
		if (doesUserExists(user.getId()))
		{
			sqlStatement = "INSERT INTO Users VALUES((SELECT ID FROM USERS order by id DESC limit 1)+1";
			sqlStatement += ",\"" + user.getName() + "\");";
		}
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: creating a user!");
	}
}
//This function is a call back function which gives the tagged pictures
int callBackTaggedPictures(void* data, int argc, char** argv, char** azColName)
{
	std::list<Picture>* temp = reinterpret_cast<std::list<Picture>*>(data);
	Picture pic(0, "", "", "");
	for (int i = 0; i < argc; i += 4)
	{
		pic.setId(atoi(argv[i]));
		pic.setName(argv[i + 1]);
		pic.setPath(argv[i + 2]);
		pic.setCreationDate(argv[i + 3]);
		temp->push_back(pic);
	}
	return 0;
}
//This function gives the tagged pictures of a user
std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	if (_db)
	{
		std::list<Picture>* saver = new std::list<Picture>;
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT ID,NAME,LOCATION,CREATION_DATE FROM TAGS INNER JOIN PICTURES ON TAGS.PICTURE_ID = Pictures.ID WHERE TAGS.USER_ID = " + std::to_string(user.getId()) + ";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackTaggedPictures, saver, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: getting tagged pictures of the user!");
		std::list<Picture> ans;
		for (auto& pic : *saver)
		{
			pic.tagUser(user);
			ans.push_back(pic);
		}
		delete saver;
		return ans;
	}
}
//This function is a call back which gives the album details with the pictures in it
int callBackAlbum2(void* data, int argc, char** argv, char** azColName)
{
	Album* temp = reinterpret_cast<Album*>(data);
	if (argc >= THREE)
	{
		temp->setName(argv[0]);
		temp->setOwner(atoi(argv[1]));
		temp->setCreationDate(argv[TWO]);
		for (int i = THREE; i < argc; i += 4)
		{
			if (std::string(azColName[i]) == "ID")
			{
				DatabaseAccess dataAccess;
				Picture pic(atoi(argv[i]), argv[i + 1], argv[i + 2], argv[i + 3]);
				temp->addPicture(pic);
			}
		}
	}
	return 0;
}
//This function is a call back which gives the album with details
int callBackAlbum1(void* data, int argc, char** argv, char** azColName)
{
	Album* temp = reinterpret_cast<Album*>(data);
	if (argc >= THREE)
	{
		temp->setName(argv[0]);
		temp->setOwner(atoi(argv[1]));
		temp->setCreationDate(argv[TWO]);
	}
	return 0;
}
//This function is a call back which gives the user id to tag
int callBackOpenTags(void* data, int argc, char** argv, char** azColName)
{
	if (argc > 0)
	{
		int* flag = (int*)data;
		*flag = atoi(argv[0]); // make the count to the counter in the func
		return atoi(argv[0]);
	}
	return UNDEFINED;
}
//This function opens album with the details of it and the pictures in it
Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	if (_db)
	{
		Album* album = new Album();
		char* errMessage = nullptr;
		std::string sqlStatement = "SELECT ALBUMS.NAME, ALBUMS.USER_ID, ALBUMS.CREATION_DATE FROM ALBUMS WHERE ALBUMS.NAME = \"" + albumName + "\";";
		int checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackAlbum1, album, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: opening the album!");
		sqlStatement = "SELECT ALBUMS.NAME, ALBUMS.USER_ID, ALBUMS.CREATION_DATE, PICTURES.ID, PICTURES.NAME, PICTURES.LOCATION, PICTURES.CREATION_DATE FROM ALBUMS INNER JOIN PICTURES ON ALBUMS.ID == PICTURES.ALBUM_ID WHERE ALBUMS.NAME = \"" + albumName + "\";";
		checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackAlbum2, album, &errMessage);
		if (checkRes != SQLITE_OK) // if there was an error in the sqlite
			throw MyException("Error: opening the album!");
		int taggedID = UNDEFINED;
		Album ans(album->getOwnerId(), album->getName(), album->getCreationDate());
		for (auto& picture : album->getPictures())
		{
			sqlStatement = "SELECT TAGS.USER_ID FROM TAGS INNER JOIN PICTURES ON TAGS.PICTURE_ID = PICTURES.ID WHERE PICTURES.ID = " + std::to_string(picture.getId()) + ";";
			checkRes = sqlite3_exec(_db, sqlStatement.c_str(), callBackOpenTags, &taggedID, &errMessage);
			if (checkRes != SQLITE_OK && std::string(errMessage) != "query aborted") // if there was an error in the sqlite
				throw MyException("Error!");
			if (taggedID >= 0)
			{
				picture.tagUser(taggedID);
				taggedID = UNDEFINED;
			}
			ans.addPicture(picture);
		}
		delete album;
		return ans;
	}
}