#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using namespace std::filesystem;

class node{
public:
    path name;
    vector<node *> children;

    node(path name){
        this->name = name;
    }

    ~node()
    {
        for (node *child : children)
        {
            delete child;
        }
    }
};

void print_work_directory(node *current){
    cout << current->name << endl;
}

void mkdir(node *current){
    //node *current = root;

    cout << "Enter folder name: " << endl;
    string input_directory;
    cin >> input_directory;

    path new_directory = current->name / input_directory;

    //add condition here in case user enters folder1/folder2/folder3 to store them properly in children

    if(!exists(new_directory)){
        
        //create new node and push it to childern vector
        node *new_node = new node("\\" + input_directory);
        current->children.push_back(new_node);

        //create folder
        create_directory(new_directory);
        cout << endl << new_directory << "\n created successfully\n";
    }
    else{
        cout << "\nFile already exists\n";
    }
}

int displayDirectoryStructure(node* root) {
    cout << root->name.filename() << endl;

    if(root->children.empty()){
        return 0;
    }

    vector<node*>::iterator iter;

    for (iter = root->children.begin(); iter != root->children.end(); iter++){
        displayDirectoryStructure(*iter);
    }
    return 0;
}
        // void displayDirectoryStructure(node* root, int depth = 0) {
        //     // for (int i = 0; i < depth; ++i) {
        //     //     cout << "  "; // Add indentation based on the depth
        //     // }
        //     cout << "|-- " << root->name.filename() << endl;

        //     for (node* child : root->children) {
        //         displayDirectoryStructure(child, depth + 1);
        //     }
        // }

node* change_directory(node *current, path new_path){
    cout << "Change directory to: " << endl;
    string new_directory;
    cin >> new_directory;
    // if(current->children.empty()){
    //     return 0;
    // }

    vector<node*>::iterator iter;

    //linear search through child vector for the directory
    for (iter = current->children.begin(); iter != current->children.end(); iter++){

        if ((*iter)->name == ("\\" + new_directory)){

            new_path = current->name / new_directory;
            current = *iter;
            return (current);
        }
    }
    //change_directory(current->ch, new_path)

}

int main()
{
    path current_directory = current_path() / "root";
    node *root = new node(current_directory);

    node *current = root;

    string choice;
    cout << "Start" << endl;
    do
    {
        cin >> choice;
        if (choice == "mkdir")
        {
            mkdir(current);
        }
        else if (choice == "pwd"){
            print_work_directory(current);
        }
        else if (choice == "exit")
        {
            break;
        }
        else if (choice == "display")
        {
            displayDirectoryStructure(root);
        }
        else if(choice == "cd"){
            current = change_directory(current, current_directory);
        }
        else if(choice == "clear"){
            system("CLS");
        }
        else{
            cout << "Invalid input" << endl;
            continue;
        }   
    } while (choice != "exit");

    return 0;
}
