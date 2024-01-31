#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std;
using namespace std::filesystem;

class Node;

class DynamicArray {
private:
    Node** data;
    int size;
    int capacity;

public:
    DynamicArray() {
        data = nullptr;
        size = 0;
        capacity = 0;
    }

    ~DynamicArray() {
        delete[] data;
    }

    void push_back(Node* value) {
        if (size == capacity) {
            if (capacity == 0) {
                capacity = 1;
            } else {
                capacity *= 2;
            }

            Node** newData = new Node*[capacity];

            for (int i = 0; i < size; i++) {
                newData[i] = data[i];
            }

            delete[] data;
            data = newData;
        }

        data[size++] = value;
    }

    void Remove(Node* value) {
        int index = -1;

        for (int i = 0; i < size; i++) {
            if (data[i] == value) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            // Shift elements to fill the gap
            for (int i = index; i < size - 1; i++) {
                data[i] = data[i + 1];
            }

            --size;
        }
    }

    int getSize() {
        return size;
    }

    Node* getNodeAt(int index) {
        if (index >= 0 && index < size) {
            return data[index];
        }
        return NULL;
    }
};

class Node {
public:
    path name;
    DynamicArray sub_directories;

    Node(path name) {
        this->name = name;
    }

    ~Node() {
        for (int i = 0; i < sub_directories.getSize(); i++) {
            delete sub_directories.getNodeAt(i);
        }
    }
};

class SimpleStack {
private:
    Node** data;
    int size;
    int capacity;

public:
    SimpleStack() {
        data = nullptr;
        size = 0;
        capacity = 0;
    }

    ~SimpleStack() {
        delete[] data;
    }

    void push(Node* value) {
        if (size == capacity) {
            if (capacity == 0) {
                capacity = 1;
            } else {
                capacity *= 2;
            }

            Node** newData = new Node*[capacity];

            for (int i = 0; i < size; i++) {
                newData[i] = data[i];
            }

            delete[] data;
            data = newData;
        }

        data[size++] = value;
    }

    void pop() {
        if (size > 0) {
            --size;
        }
    }

    Node* top() {
        if (size > 0) {
            return data[size - 1];
        }

        return NULL;
    }

    bool isEmpty() {
        return size == 0;
    }
};

class SimpleQueue {
private:
    Node** data;
    int size;
    int capacity;
    int front;
    int rear;

public:
    SimpleQueue() {
        data = NULL;
        size = 0;
        capacity = 0;
        front = 0;
        rear = 0;
    }

    ~SimpleQueue() {
        delete[] data;
    }

    void enqueue(Node* value) {
        if (size == capacity) {
            if (capacity == 0) {
                capacity = 1;
            } else {
                capacity *= 2;
            }

            Node** newData = new Node*[capacity];

            for (int i = 0; i < size; i++) {
                newData[i] = data[(front + i) % capacity];
            }

            delete[] data;
            data = newData;
            front = 0;
            rear = size;
        }

        data[rear] = value;
        rear = (rear + 1) % capacity;
        ++size;
    }

    void dequeue() {
        if (size > 0) {
            front = (front + 1) % capacity;
            --size;
        }
    }

    Node* frontNode(){
        if (size > 0) {
            return data[front];
        }

        return NULL;
    }

    bool isEmpty(){
        return size == 0;
    }
};

void printWorkDirectory(Node* current) {
    cout << current->name;
}

void make_directory(Node* current) {
    //cout << "Enter folder name: " << endl;
    string inputDirectory;
    cin >> inputDirectory;

    path newDirectory = current->name / inputDirectory;

    try{
        create_directory(newDirectory);

        Node* newNode = new Node(newDirectory);
        current->sub_directories.push_back(newNode);
        
    } catch (filesystem_error e) {
        cout << "Invalid directory" << endl;
    }
}

int displayDirectoryStructure(Node* root, int num_of_tab) {

    for (int i = 0; i < num_of_tab; i++) {
        cout << "|     ";
    }
    cout << "|--- " << root->name.filename() << endl;


    for (int i = 0; i < root->sub_directories.getSize(); i++) {
        displayDirectoryStructure(root->sub_directories.getNodeAt(i), num_of_tab + 1);
    }

    return 0;
}

Node* changeDirectory_recursion(SimpleStack& directoryStack, path new_directory) {

    if (directoryStack.top()->name == new_directory) {
        return directoryStack.top();
    }
    else {
        changeDirectory_recursion(directoryStack, new_directory.parent_path());

        bool flag = false;

        for (int i = 0; i < directoryStack.top()->sub_directories.getSize(); i++) {

            if (directoryStack.top()->sub_directories.getNodeAt(i)->name == new_directory) {

                directoryStack.push(directoryStack.top()->sub_directories.getNodeAt(i));
                flag = true;
                break;
            }
        }

        if (!flag){
            throw string("Invalid directory");
        }
    }

}

void change_directory(SimpleStack& directoryStack) {
    // cout << "Change directory to: " << endl;
    string inputDirectory;
    cin >> inputDirectory;

    path new_directory = directoryStack.top()->name / inputDirectory;

    //safety feature
    if ((new_directory.filename() == "..") && (directoryStack.top()->name.filename() == "root")) {
        cout << "Cannot go beyond root directory" << endl;
        return;
    }

    if (new_directory.filename() == "..") {

        while(new_directory.filename() == "..") {
            new_directory = new_directory.parent_path();
            directoryStack.pop();
        }

    } 
    else {

        try {
            changeDirectory_recursion(directoryStack, new_directory);

        } catch (string message){
            cout << message << endl;
        }
    }
}

void search(Node* root) {
    string searchDirectory;
    cin >> searchDirectory;

    SimpleQueue BFSQueue;
    BFSQueue.enqueue(root);

    while (!BFSQueue.isEmpty()) {
        Node* temp = BFSQueue.frontNode();

        if (temp->name.filename() == searchDirectory) {
            cout << "found at :   " << temp->name << endl;
            return;
        }

        BFSQueue.dequeue();

        for (int i = 0; i < temp->sub_directories.getSize(); i++) {
            BFSQueue.enqueue(temp->sub_directories.getNodeAt(i));
        }
    }

    cout << "Directory not found" << endl;
}

void list(Node* current) {
    string extension, directoryName;
    cout << "name:" << "\t" << "extension" << endl;

    for (int i = 0; i < current->sub_directories.getSize(); i++) {

        extension = current->sub_directories.getNodeAt(i)->name.extension().string();
        directoryName = current->sub_directories.getNodeAt(i)->name.filename().string();
        
        if (extension == ""){
            extension = "folder";
        }

        cout << directoryName << "\t" << extension << endl;
    }
}

void saveToFile(Node *root, ofstream &file) {
    file << root->name.string() << endl;
    file << root->sub_directories.getSize() << endl;

    for (int i = 0; i < root->sub_directories.getSize(); i++) {
        saveToFile((*root).sub_directories.getNodeAt(i), file);
    }
}

Node *loadFromFile(ifstream &file) {
    string nodeName;
    file >> nodeName;

    int numChildren;
    file >> numChildren;

    Node *root = new Node(nodeName);

    for (int i = 0; i < numChildren; i++) {
        Node *child = loadFromFile(file);
        root->sub_directories.push_back(child);
    }

    return root;
}

void Delete(Node *current){
    //cout << "Enter folder name: " << endl;
    string inputDirectory;
    cin >> inputDirectory;
    bool flag = false;

    path newDirectory = current->name / inputDirectory;

    try{

        for (int i = 0; i < current->sub_directories.getSize(); i++){

            if(current->sub_directories.getNodeAt(i)->name == newDirectory){
                remove(newDirectory);
                current->sub_directories.Remove(current->sub_directories.getNodeAt(i));
                flag = true;
                break;
            }
        }

        if(!flag){
            throw string("Invalid directory");
        }
        
    } catch (filesystem_error e) {
        cout << "Invalid directory: directory contains folders" << endl;
    } catch (string e){
        cout << e << endl;
    }
}

void menu(bool help){

    if(help == 0){
        cout << "\n\t\t   WARNING!\n\n This program has the capability to edit folders.\n\t\tUse it carefully\n\n\t";
        system("pause");
        system("CLS");

        cout << "  ___   _   _           ___               _                     ___                _                           \n | __| (_) | |  ___    / __|  _  _   ___ | |_   ___   _ __     | __| __ __  _ __  | |  ___   _ _   ___   _ _   \n | _|  | | | | / -_)   \\__ \\ | || | (_-< |  _| / -_) | '  \\    | _|  \\ \\ / | '_ \\ | | / _ \\ | '_| / -_) | '_|  \n |_|   |_| |_| \\___|   |___/  \\_, | /__/  \\__| \\___| |_|_|_|   |___| /_\\_\\ | .__/ |_| \\___/ |_|   \\___| |_|    \n                              |__/                                         |_|                                 ";
        cout << endl;
        cout << "Made by: Hassan & Anas" << endl;
    }

    cout << "Commands:           \tFunctions performed:" << endl
         << "mkdir [directory]   \tCreates a new folder." << endl
         << "rm [directory]      \tRemoves or deletes files or folders." << endl
         << "cd [directory]      \tChange directory." << endl
         << "find [directory]    \tSearch directory." << endl
         << "ls                  \tLists the contents of a directory." << endl
         << "pwd                 \tPrint Working Directory." << endl
         << "display             \tDisplays the directory tree." << endl
         << "clear               \tClear screen." << endl
         << "exit                \tExit program.(saves all data before exit)" << endl
         << "save                \tSave directory tree." << endl
         << "load                \tLoad directory tree." << endl << endl
         << "help                \tDisplay commands." << endl;
}

int main() {
    path currentDirectory = current_path() / "root";
    Node* root = new Node(currentDirectory);
    
    menu(0);

    //making root directory if it doesn't exist
    if (create_directory(currentDirectory)){
        cout << "Root directory created" << endl;
    }

    Node* current = root;

    //for taking user choice
    string choice;

    SimpleStack directoryStack;
    directoryStack.push(current);

    //displays menu

    //checks if text file for loading data exists. 
    //If it exists it loads tree data from it
    if (exists(current_path() / "directory_tree.txt")){
        ifstream inFile("directory_tree.txt");
        Node *loadedTree = loadFromFile(inFile);
        inFile.close();

        delete root;
        root = loadedTree;
        current = root;
            
        directoryStack.push(current);
    }

    do {
        cout << endl;
        printWorkDirectory(current);
        cout << " >  ";

        cin >> choice;
        if (choice == "mkdir") {
            make_directory(current);
        } 
        else if (choice == "pwd") {
            printWorkDirectory(current);
            cout << endl;
        } 
        else if (choice == "help") {
            menu(1);
        } 
        else if (choice == "exit") {

            // saves data and exits program
            ofstream outFile("directory_tree.txt");
            saveToFile(root, outFile);
            outFile.close();
            break;
        } 
        else if (choice == "display") {
            displayDirectoryStructure(root, 0);
        } 
        else if (choice == "cd") {
            change_directory(directoryStack);
            current = directoryStack.top();
        } 
        else if (choice == "clear") {
            system("CLS");
        } 
        else if (choice == "ls") {
            list(current);
        } 
        else if (choice == "find") {
            search(root);
        } 
        else if  (choice == "rm") {
            Delete(current);
        } 
        else if (choice == "save") {
            ofstream outFile("directory_tree.txt");
            saveToFile(root, outFile);
            outFile.close();
        } 
        else if (choice == "load") {
            ifstream inFile("directory_tree.txt");
            Node *loadedTree = loadFromFile(inFile);
            inFile.close();
            
            delete root;
            root = loadedTree;
            current = root;
            
            directoryStack.push(current);
        }
        else {
            cout << "Invalid command" << endl;
            continue;
        }

    } while (choice != "exit");

    delete root;
    return 0;
}
