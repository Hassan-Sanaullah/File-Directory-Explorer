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

            for (int i = 0; i < size; ++i) {
                newData[i] = data[i];
            }

            delete[] data;
            data = newData;
        }

        data[size++] = value;
    }

    void Remove(Node* value) {
        int index = -1;

        for (int i = 0; i < size; ++i) {
            if (data[i] == value) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            // Shift elements to fill the gap
            for (int i = index; i < size - 1; ++i) {
                data[i] = data[i + 1];
            }

            --size;
        }
    }

    int getSize() const {
        return size;
    }

    Node* getNodeAt(int index) const {
        if (index >= 0 && index < size) {
            return data[index];
        }
        return nullptr;
    }
};

class Node {
public:
    path name;
    DynamicArray children;

    Node(path name) {
        this->name = name;
    }

    ~Node() {
        for (int i = 0; i < children.getSize(); ++i) {
            delete children.getNodeAt(i);
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

            for (int i = 0; i < size; ++i) {
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

    Node* top() const {
        if (size > 0) {
            return data[size - 1];
        }

        return nullptr;
    }

    bool empty() const {
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
        data = nullptr;
        size = 0;
        capacity = 0;
        front = 0;
        rear = 0;
    }

    ~SimpleQueue() {
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

            for (int i = 0; i < size; ++i) {
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

    void pop() {
        if (size > 0) {
            front = (front + 1) % capacity;
            --size;
        }
    }

    Node* frontNode() const {
        if (size > 0) {
            return data[front];
        }

        return nullptr;
    }

    bool empty() const {
        return size == 0;
    }
};

void printWorkDirectory(Node* current) {
    cout << current->name;
}

path mkdir_recursion(path newDirectory, string Filename, Node** current) {

    if ((*current)->name == newDirectory) {
        for (int i = 0; i < (*current)->children.getSize(); ++i) {
            if ((*current)->children.getNodeAt(i)->name.filename() == Filename)
                (*current) = (*current)->children.getNodeAt(i);
        }
        return newDirectory;
    } else {

        mkdir_recursion(newDirectory.parent_path(), newDirectory.filename().string(), current);

        Node* newNode = new Node(newDirectory);
        (*current)->children.push_back(newNode);

        create_directory(newNode->name);

        for (int i = 0; i < (*current)->children.getSize(); ++i) {
            if ((*current)->children.getNodeAt(i)->name == newDirectory)
                (*current) = (*current)->children.getNodeAt(i);
        }
        return newDirectory;
    }
}

void make_directory(Node* current) {
    cout << "Enter folder name: " << endl;
    string inputDirectory;
    cin >> inputDirectory;

    path newDirectory = current->name / inputDirectory;


    try{
        create_directory(newDirectory);

        Node* newNode = new Node(newDirectory);
        current->children.push_back(newNode);
        
    } catch (filesystem_error e) {
        cout << "Invalid directory" << endl;
    }

    //mkdir_recursion(newDirectory.string(), "", &current);
}

int displayDirectoryStructure(Node* root, int num_of_tab) {
    for (int i = 0; i < num_of_tab; i++) {
        cout << "|-------";
    }
    cout << root->name.filename() << endl;

    if (root->children.getSize() == 0) {
        return 0;
    }
    num_of_tab++;

    for (int i = 0; i < root->children.getSize(); ++i) {
        displayDirectoryStructure(root->children.getNodeAt(i), num_of_tab);
    }
    return 0;
}

Node* cd_recursion(SimpleStack& directoryStack, path new_directory) {

    if (directoryStack.top()->name == new_directory) {
        return directoryStack.top();
    } else {

        cd_recursion(directoryStack, new_directory.parent_path());

        bool flag = false;

        for (int i = 0; i < directoryStack.top()->children.getSize(); ++i) {

            if (directoryStack.top()->children.getNodeAt(i)->name == new_directory) {
                directoryStack.push(directoryStack.top()->children.getNodeAt(i));
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
    cout << "Change directory to: " << endl;
    string inputDirectory;
    cin >> inputDirectory;

    path new_directory = directoryStack.top()->name / inputDirectory;

    //safety feature
    if ((new_directory.filename() == "..") && (directoryStack.top()->name.filename() == "root")){
        cout << "Cannot go beyond root directory" << endl;
        return;
    }

    if (new_directory.filename() == "..") {

        while(new_directory.filename() == "..") {
            new_directory = new_directory.parent_path();
            directoryStack.pop();
        }
    } else {

        try{
            cd_recursion(directoryStack, new_directory);

        } catch (string e){
            cout << e << endl;
        }
    }
}

void search(Node* root) {
    string searchDirectory;
    cin >> searchDirectory;

    SimpleQueue BFSQueue;
    BFSQueue.push(root);

    while (!BFSQueue.empty()) {
        Node* temp = BFSQueue.frontNode();
        if (temp->name.filename() == searchDirectory) {
            cout << "found at :   " << temp->name << endl;
            break;
        }

        BFSQueue.pop();

        for (int i = 0; i < temp->children.getSize(); ++i) {
            BFSQueue.push(temp->children.getNodeAt(i));
        }
    }
}

void list(Node* current) {
    cout << "name:" << "\t" << "extension" << endl;

    string extension;

    for (int i = 0; i < current->children.getSize(); ++i) {
        extension = current->children.getNodeAt(i)->name.extension().string();
        if (extension == ""){
            extension = "dir";
        }

        cout << current->children.getNodeAt(i)->name.filename() << "\t" << extension << endl;
    }
}

void saveToFile(Node *root, ofstream &file) {
    file << root->name.string() << endl;
    file << root->children.getSize() << endl;

    for (int i = 0; i < root->children.getSize(); ++i) {
        saveToFile((*root).children.getNodeAt(i), file);
    }
}

Node *loadFromFile(ifstream &file) {
    string nodeName;
    file >> nodeName;

    int numChildren;
    file >> numChildren;

    Node *root = new Node(nodeName);

    for (int i = 0; i < numChildren; ++i) {
        Node *child = loadFromFile(file);
        root->children.push_back(child);
    }

    return root;
}

void Delete(Node *current){
    cout << "Enter folder name: " << endl;
    string inputDirectory;
    cin >> inputDirectory;
    bool flag = false;

    path newDirectory = current->name / inputDirectory;

    try{

        for (int i = 0; i < current->children.getSize(); i++){

            if(current->children.getNodeAt(i)->name == newDirectory){
                remove(newDirectory);
                current->children.Remove(current->children.getNodeAt(i));
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

    if(!help){
        cout << "\n\t\t   WARNING!\n\n This program has the capability to edit folders.\n\t\tUse it carefully\n\n";
        system("pause");
        system("CLS");

        cout << "  ___   _   _           ___               _                     ___                _                           \n | __| (_) | |  ___    / __|  _  _   ___ | |_   ___   _ __     | __| __ __  _ __  | |  ___   _ _   ___   _ _   \n | _|  | | | | / -_)   \\__ \\ | || | (_-< |  _| / -_) | '  \\    | _|  \\ \\ / | '_ \\ | | / _ \\ | '_| / -_) | '_|  \n |_|   |_| |_| \\___|   |___/  \\_, | /__/  \\__| \\___| |_|_|_|   |___| /_\\_\\ | .__/ |_| \\___/ |_|   \\___| |_|    \n                              |__/                                         |_|                                 ";
        cout << endl;
    }

    cout << "Commands:\t" << endl
         << "mkdir    \tCreates a new folder." << endl
         << "ls       \tLists the contents of a directory." << endl
         << "pwd      \tPrint Working Directory." << endl
         << "rm       \tRemoves or deletes files or folders." << endl
         << "cd       \tChange directory." << endl
         << "display  \tDisplays the directory tree." << endl
         << "clear    \tClear screen." << endl
         << "exit     \tExit program." << endl
         << "save     \tSave directory tree." << endl
         << "load     \tLoad directory tree." << endl << endl
         << "help     \tDisplay commands." << endl;
}

int main() {
    path currentDirectory = current_path() / "root";
    Node* root = new Node(currentDirectory);

    //making root directory if it doesn't exist
    if (create_directory(currentDirectory)){
        cout << "Root directory created" << endl;
    }

    Node* current = root;

    string choice;

    SimpleStack directoryStack;
    directoryStack.push(current);

    menu(0);

    do {
        printWorkDirectory(current);
        cout << " >  ";

        cin >> choice;
        if (choice == "mkdir") {
            make_directory(current);
        } else if (choice == "pwd") {
            printWorkDirectory(current);
            cout << endl;
        } else if (choice == "help") {
            menu(1);
        } else if (choice == "exit") {
            break;
        } else if (choice == "display") {
            displayDirectoryStructure(root, 0);
        } else if (choice == "cd") {
            change_directory(directoryStack);
            current = directoryStack.top();
        } else if (choice == "clear") {
            system("CLS");
        } else if (choice == "ls") {
            list(current);
        } else if (choice == "find") {
            search(root);
        } else if  (choice == "rm") {
            Delete(current);
        } else if (choice == "save") {
            ofstream outFile("tree.txt");
            saveToFile(root, outFile);
            outFile.close();
        } else if (choice == "load") {
            ifstream inFile("tree.txt");
            Node *loadedTree = loadFromFile(inFile);
            inFile.close();
            
            // Cleanup existing root and set the loaded tree as the new root
            delete root;
            root = loadedTree;
            current = root;
            
            directoryStack.push(current);
        }
         else {
            cout << "Invalid input" << endl;
            continue;
        }
    } while (choice != "exit");

    delete root; // Release allocated memory

    return 0;
}
