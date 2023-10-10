#include <iostream>
#include <direct.h>
#include "dirent.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <errno.h>
#include <string>
#pragma warning(disable:4996)

namespace fs = std::filesystem;

class Object
{
protected:
    char* name;
public:
    Object(const char* name)
    {
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    }
    virtual void r_object(const char* name) = 0;
    virtual const char* get_name()
    {
        return this->name;
    }
    virtual bool is_name_equal(const char* name) const
    {
        return strcmp(this->name, name) == 0;
    }
    virtual bool is_what_type(const char* name) const = 0;
    virtual ~Object()
    {
        delete[] name;
    }
};

class Folder : public Object
{
public:
    //Folder creation method
    Folder(const char* name) :Object(name)
    {
        int error_check = _mkdir(name);
        if (error_check == -1)
        {
            std::cout << "Make directory failed.\n";
            if (errno == EEXIST)
                std::cout << "Directory already exists.\n";
            else if (errno == ENOENT)
                std::cout << "Path not found.\n";
        }
        else
            std::cout << "Make directory succeeded.\n";
    }
    void r_object(const char* name) override
    {
        char answer;
        if (fs::exists(name))
        {
            if (fs::is_directory(name))
            {
                std::cout << "Are you SURE that you want to delete the \"" << name << "\" folder?\n\n";
                std::cout << "Y / N: ";
                std::cin >> answer;
                std::cin.ignore();
                answer = toupper(answer);
                if (answer == 'Y')
                {
                    if (fs::remove_all(name))
                        std::cout << "Remove directory " << name << " succeeded.\n";
                    else
                        std::cout << "Remove directory " << name << " failed.\n";
                }
                else if (answer == 'N')
                    std::cout << "Remove operation canceled.\n";
                else
                    std::cout << "Remove operation canceled.\n";
            }
            else if (fs::is_regular_file(name))
            {
                if (fs::remove(name))
                    std::cout << "Remove file " << name << " succeeded.\n";
                else
                    std::cout << "Remove file " << name << " failed.\n";
            }
            else
                std::cout << "Unknown object type for " << name << ".\n";
        }
        else
        {
            std::cout << "Object not found: " << name << ".\n";
        }
    }
    bool is_what_type(const char* name) const override
    {
        return fs::is_directory(name);
    }
};
class File : public Object
{
public:
    //File creation method
    File(const char* name) :Object(name)
    {
        std::ofstream file(name);
        if (file.is_open())
        {
            std::cout << "File created successfully.\n";
            file.close();
        }
        else
            std::cout << "Error creating file.\n";
    }
    void r_object(const char* name) override
    {
        if (remove(name) == 0)
            std::cout << "Remove file " << name << " succeeded.\n";
        else
            std::cout << "Remove file " << name << " failed.\n";
    }
    bool is_what_type(const char* name) const override
    {
        return fs::is_regular_file(name);
    }
};

class Application
{
private:
    char path[2048];
    std::vector<Object*> objects;
    const char* gwd()
    {
        if (_getcwd(path, sizeof(path)) == NULL)
        {
            return "Get current working directory failed.\n";
        }
        return path;
    }

    //спроба реалізувати скан директорі для додавання об'єктів в вектор
    /*void scan_dir(const char* path, std::vector<Object*>& objects)
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_directory())
                objects.push_back(new Folder(entry.path().string().c_str()));
            else if (entry.is_regular_file())
                objects.push_back(new Folder(entry.path().string().c_str()));
        }
    }*/
    /*void clear_objects()
    {
        for (Object* obj : objects)
            delete obj;
        objects.clear();
    }*/
public:
    Application(const char* absolute_path)
    {
        strncpy(path, absolute_path, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
        ch_dir(path);
    }
    //меню програми
    void menu()
    {
        std::string choice;
        bool flag = true;
        std::cout << "#########################################################\n";
        std::cout << "# Welcome to the File_Manager.cpp v1.0.                 #\n";
        std::cout << "# This file manager works just like CMD in Windows.     #\n";
        std::cout << "# That means that you have to use commands to work with #\n";
        std::cout << "# files and folders in your system.                     #\n";
        std::cout << "#########################################################\n\n";
        std::cout << "Type \"help\" to get a list of the commands.\n";
        while (flag)
        {
            pwd();
            getline(std::cin, choice);
            for (int i = 0; i < choice.length(); i++)
                choice[i] = tolower(choice[i]);
            if (choice == "help")
            {
                std::cout << "List of the commands:\n";
                std::cout << " pwd - print working directory\n";
                std::cout << " mkf - make file\n";
                std::cout << " write - write to file\n";
                std::cout << " read - read from file\n";
                std::cout << " mkdir - make directory\n";
                std::cout << " remove - remove object(File or Dir)\n";
                std::cout << " rename - rename file or directory\n";
                std::cout << " copy - copy file or directory to specific destination\n";
                std::cout << " cut - cut file or directory to specific destination\n";
                std::cout << " size - size of file or directory\n";
                std::cout << " src - search object by specific mask\n";
                std::cout << " cd - change working directory\n";
                std::cout << " ls - list all objects in the directory\n";
                std::cout << " cls - clear screen\n";
            }
            else if (choice == "pwd")
            {
                std::cout << "Current working directory: ";
                pwd();
                std::cout << std::endl;
            }
            else if (choice == "mkf")
            {
                std::string name;
                std::cout << "File name or an absolute path: ";
                getline(std::cin, name);
                create_file(name.c_str());
            }
            else if (choice == "write")
            {
                std::string f_name;
                std::string str;
                std::cout << "File name or an absolute path: ";
                getline(std::cin, f_name);
                std::cout << "# Info to write to file:\n";
                getline(std::cin, str);
                write_to_file(f_name.c_str(), str);
            }
            else if (choice == "read")
            {
                std::string content;
                std::string f_name;
                std::cout << "File name or an absolute path: ";
                getline(std::cin, f_name);
                content = read_from_file(f_name.c_str());
                std::cout << "# File content:\n";
                std::cout << content;
            }
            else if (choice == "mkdir")
            {
                std::string name;
                std::cout << "Directory name or an absolute path: ";
                getline(std::cin, name);
                create_folder(name.c_str());
            }
            else if (choice == "remove")
            {
                std::string name;
                std::cout << "File name or an absolute path: ";
                getline(std::cin, name);
                del_obj(name.c_str());
            }
            else if (choice == "rename")
            {
                std::string name;
                std::string new_name;
                std::cout << "Object name or an absolute path: ";
                getline(std::cin, name);
                std::cout << "New Object name: ";
                getline(std::cin, new_name);
                rename(name.c_str(), new_name.c_str());
            }
            else if (choice == "copy")
            {
                std::string path;
                std::string new_path;
                std::cout << "Object name or an absolute path: ";
                getline(std::cin, path);
                std::cout << "Destination name or an absolute path: ";
                getline(std::cin, new_path);
                copy(path.c_str(), new_path.c_str());
            }
            else if (choice == "cut")
            {
                std::string path;
                std::string new_path;
                std::cout << "Object name or an absolute path: ";
                getline(std::cin, path);
                std::cout << "Destination name or an absolute path: ";
                getline(std::cin, new_path);
                cut(path.c_str(), new_path.c_str());
            }
            else if (choice == "size")
            {
                std::string path;
                std::cout << "Object name or an absolute path: ";
                getline(std::cin, path);
                std::cout << "Size of " << path << ": " << is_what_size(path) << " byte(s)\n";
            }
            else if (choice == "src")
            {
                std::string path;
                std::string mask;
                std::cout << "Path(where to search): ";
                getline(std::cin, path);
                std::cout << "Mask: ";
                getline(std::cin, mask);
                search(path, mask);
            }
            else if (choice == "cd")
            {
                std::string path;
                std::cout << "New directory path: ";
                getline(std::cin, path);
                ch_dir(path.c_str());
            }
            else if (choice == "ls")
            {
                list_dir(path);
            }
            else if (choice == "cls")
            {
                system("cls");
            }
            else if (choice == "exit" || choice == "EXIT")
            {
                std::cout << "\nEnd of a program\n\nThanks for using this File Manager!\n";
                break;
            }
            else
                std::cout << "Wrong command\n";
        }
    }
    void create_file(const char* name)
    {
        std::ifstream file(name);
        if (!file.good())
            objects.push_back(new File(name));
        else
            std::cout << "Make file failed.\nFile already exists.\n";
    }
    void write_to_file(const char* name, const std::string& str)
    {
        std::ofstream file(name, std::ios::app);
        if (!file.is_open())
            std::cout << "Error opening file " << name << ".\n";
        else
        {
            file << str << std::endl;
            file.close();
            std::cout << "Written to file " << name << " successfully.\n";
        }
    }
    std::string read_from_file(const char* name)
    {
        std::ifstream file(name);
        if (!file.is_open())
        {
            std::cout << "Error opening file" << name << ".\n";
            return "";
        }
        else
        {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            return content;
        }
    }
    void create_folder(const char* name)
    {
        objects.push_back(new Folder(name));
    }
    void del_obj(const char* name)
    {
        if (!objects.empty())
        {
            for (auto it = objects.begin(); it != objects.end(); it++)
            {
                Object* obj = *it;
                if (obj->is_name_equal(name))
                {
                    obj->r_object(name);
                    delete obj;
                    objects.erase(it);
                    break;
                }
            }
        }
        else
        {
            char answer;
            if (fs::exists(name))
            {
                if (fs::is_directory(name))
                {
                    std::cout << "Are you SURE that you want to delete the \"" << name << "\" folder?\n\n";
                    std::cout << "Y / N: ";
                    std::cin >> answer;
                    std::cin.ignore();
                    answer = toupper(answer);
                    if (answer == 'Y')
                    {
                        if (fs::remove_all(name))
                            std::cout << "Remove directory " << name << " succeeded.\n";
                        else
                            std::cout << "Remove directory " << name << " failed.\n";
                    }
                    else if (answer == 'N')
                        std::cout << "Remove operation canceled.\n";
                    else
                        std::cout << "Remove operation canceled.\n";
                }
                else if (fs::is_regular_file(name))
                {
                    if (fs::remove(name))
                        std::cout << "Remove file " << name << " succeeded.\n";
                    else
                        std::cout << "Remove file " << name << " failed.\n";
                }
                else
                    std::cout << "Unknown object type for " << name << ".\n";
            }
            else
            {
                std::cout << "Object not found: " << name << ".\n";
            }
        }
    }
    //змінити робочу директорію
    void ch_dir(const char* path)
    {
        //clear_objects();
        if (_chdir(path) == -1)
        {
            std::cout << "Change working directory failed.\n";
            if (errno == ENOENT)
                std::cout << "Path not found.\n";
        }
        else
        {
            gwd();
            //scan_dir(path, objects);
            std::cout << "Change working directory succeeded.\n";
        }
    }
    //лістинг всіх об'єктів в директорії
    void list_dir(const char* name)
    {
        DIR* directory;
        struct dirent* entry;
        directory = opendir(name);
        if (directory == NULL)
            std::cout << "Error opening directory.\n";
        while ((entry = readdir(directory)) != NULL)
        {
            if (entry->d_type == DT_REG)
                std::cout << "<file>\t" << entry->d_name << std::endl;
            else if (entry->d_type == DT_DIR)
                std::cout << "<dir>\t" << entry->d_name << std::endl;
        }
        if (closedir(directory) == -1)
            std::cout << "Error closing directory.\n";
    }
    void rename(const char* name, const char* new_name)
    {
        if (fs::exists(name))
        {
            fs::rename(name, new_name);
            std::cout << "Renamed " << name << " successfully.\n";
        }
        else
            std::cout << "File or directory does not exist.\n";
    }
    void copy_file(const std::string& path, const std::string& new_path)
    {
        std::string content;
        content = read_from_file(path.c_str());
        create_file(new_path.c_str());
        write_to_file(new_path.c_str(), content);
    }
    void copy(const std::string& path, const std::string& new_path)
    {
        //створюємо нову директорію
        create_folder(new_path.c_str());

        //перебираємо об'єкти в директорії
        DIR* dir;
        struct dirent* entry;
        if ((dir = opendir(path.c_str())) != nullptr) {
            while ((entry = readdir(dir)) != nullptr) {
                if (entry->d_type == DT_REG) {
                    //якщо файл, то копіюємо його
                    std::string source_file = path + "/" + entry->d_name;
                    std::string destination_file = new_path + "/" + entry->d_name;
                    copy_file(source_file, destination_file);
                }
                else if (entry->d_type == DT_DIR && std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
                    //якщо директорія, рекурсивно викликаєио метод копіювання всього в цій директорії
                    std::string source_subdir = path + "/" + entry->d_name;
                    std::string destination_subdir = new_path + "/" + entry->d_name;
                    copy(source_subdir, destination_subdir);
                }
            }
            closedir(dir);
        }
    }
    void cut(const std::string& path, const std::string& new_path)
    {
        copy(path, new_path);
        char answer;
        if (fs::exists(path))
        {
            if (fs::is_directory(path))
            {
                std::cout << "Are you SURE that you want to delete the \"" << path << "\" folder?\n\n";
                std::cout << "Y / N: ";
                std::cin >> answer;
                std::cin.ignore();
                answer = toupper(answer);
                if (answer == 'Y')
                {
                    if (fs::remove_all(path))
                        std::cout << "Remove directory " << path << " succeeded.\n";
                    else
                        std::cout << "Remove directory " << path << " failed.\n";
                }
                else if (answer == 'N')
                    std::cout << "Remove operation canceled.\n";
                else
                    std::cout << "Remove operation canceled.\n";
            }
            else if (fs::is_regular_file(path))
            {
                if (fs::remove(path))
                    std::cout << "Remove file " << path << " succeeded.\n";
                else
                    std::cout << "Remove file " << path << " failed.\n";
            }
            else
                std::cout << "Unknown object type for " << path << ".\n";
        }
        else
        {
            std::cout << "Object not found: " << path << ".\n";
        }
    }
    long long int is_what_size(const fs::path& path)
    {
        if (fs::exists(path))
        {
            long long int total_size = 0;
            if (fs::is_regular_file(path))
            {
                total_size = fs::file_size(path);
                return total_size;
            }
            else if (fs::is_directory(path))
            {
                for (const auto& entry : fs::directory_iterator(path))
                {
                    if (fs::is_regular_file(entry))
                        total_size += fs::file_size(entry);
                    else if (fs::is_directory(entry))
                        total_size += is_what_size(entry);
                }
                return total_size;
            }
        }
        else
        {
            std::cout << "File or directory does NOT exist!\n";
            return -1;
        }
    }
    void search(const fs::path& path, std::string mask)
    {
        // Mask with no stars - .txt if *.txt 
        mask.erase(std::remove(mask.begin(), mask.end(), '*'), mask.end());
        // Mask with no dots - file_name if file_name.*
        std::string no_dots = mask;
        no_dots.erase(std::remove(no_dots.begin(), no_dots.end(), '.'), no_dots.end());

        if (fs::exists(path))
        {
            for (const auto& entry : fs::directory_iterator(path))
            {
                if (fs::is_regular_file(entry))
                {
                    if (mask.empty() || entry.path().extension() == mask || entry.path().filename() == mask || entry.path().stem() == no_dots)
                        std::cout << "File found: " << entry.path() << "\n";
                }
                else if (fs::is_directory(entry))
                {
                    if (mask.empty() || entry.path().filename() == mask)
                    {
                        std::cout << "Directory found: " << entry.path() << "\n";
                    }
                    search(entry.path(), mask);
                }
            }
        }
        else
            std::cout << "No such file or directory!\n";
    }
    //вивести поточну директорію
    void pwd() const
    {
        char buffer[1024];
        _getcwd(buffer, 1024);
        std::cout << "<" << buffer << ">";
    }
    ~Application()
    {
        for (Object* obj : objects)
            delete obj;
    }
};
int main()
{
    //початкова директорія
    Application ap1("C:\\Users");
    ap1.menu();
    return 0;
}