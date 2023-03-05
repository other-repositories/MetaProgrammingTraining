#include <iostream>
#include <algorithm>
#include <fstream>

template<class Derived>
class PhotoComponent
{
public:
	void draw() {
		static_cast<const Derived*>(this)->draw();
	}

};

class _Photo : public PhotoComponent<_Photo>
{
public:
	void draw() 
	{
		if (image.empty())
			std::cout << "Image error\n";
		std::for_each(image.begin(), image.end(), [](char c) { std::cout << c; });
	}
	_Photo(const std::string& path)
	{
		image.reserve(500);
		_getImage(path);
	}
private:
	void _getImage(const std::string& path)
	{
		char ch;
		std::ifstream in(path);
		if (in.is_open())
		{
			while (!in.eof())
			{
				in.get(ch);
				image.push_back(ch);
			}
			in.close();
		}
	}
	std::string image;
};

//Proxy
class Photo : public PhotoComponent<Photo>
{
public:
	Photo(std::string login, std::string password) :path("")
	{
		getPath(login, password);
	}
	void draw() 
	{
		if (photo == nullptr)
		{
			photo = new _Photo(path);
		}
		photo->draw();
	}
	~Photo()
	{
		if (photo != nullptr)
			delete photo;
	}
private:
	void getPath(std::string login, std::string password)
	{
		if (login == "admin" && password == "admin")
		{
			path = "image.txt";
		}
	}
	std::string path;
	_Photo* photo;
};

int main() {

}