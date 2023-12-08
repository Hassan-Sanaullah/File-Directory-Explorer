#include <filesystem>
#include <stack>
#include <iostream>
#include <vector>
#include <cstring>

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

vector<string> splitString(const string& input, const char* delimiter) {
    vector<string> result;
    char s[input.length() + 1];
    strcpy(s, input.c_str());

    // Pointer to point the word returned by the strtok() function.
    char *p;
    // Here, the delimiter is specified.
    p = strtok(s, delimiter);
    while (p != NULL) {
        result.push_back(p);
        p = strtok(NULL, delimiter);
    }

    return result;
}

void print_work_directory(node *current){
    cout << current->name << endl;
}


path mkdir_helper(path new_directory, node **current){
    
    // splitString(current->name.string(), "\\");
    if(exists(new_directory)){
        return new_directory;
    }
    else if(!exists(new_directory)){
        //new_directory = mkdir_helper(new_directory.parent_path(), current);
        create_directory(mkdir_helper(new_directory.parent_path(), current));
        node *new_node = new node(new_directory);
        (*current)->children.push_back(new_node);

        for (node *temp: (*current)->children){
            if(temp->name == new_directory)
                (*current) = temp;
        }
        return new_directory;
    }

}


void mkdir(node *current){

    cout << "Enter folder name: " << endl;
    string input_directory;
    cin >> input_directory;

    path new_directory = current->name / input_directory;

    //PROBLEM!! cannot pass non existing path to function, must convert to string
    mkdir_helper(new_directory.string(), &current);
    //add condition here in case user enters folder1/folder2/folder3 to store them properly in children

    // if(!exists(new_directory)){
        
    //     //create new node and push it to childern vector
    //     node *new_node = new node(new_directory);
    //     current->children.push_back(new_node);

    //     //create folder
    //     create_directory(new_directory);
    //     cout << endl << new_directory << "\n created successfully\n";
    // }
    // else{
    //     cout << "\nFile already exists\n";
    // }
}

int displayDirectoryStructure(node* root, int num_of_tab) {

    for (int i = 0; i < num_of_tab; i++){
        cout << "\t";
    }
        cout << root->name.filename() << endl;

    if(root->children.empty()){
        return 0;
    }
    num_of_tab++;

    vector<node*>::iterator iter;
    for (iter = root->children.begin(); iter != root->children.end(); iter++){
        
        displayDirectoryStructure(*iter, num_of_tab);
    }
    return 0;
}

node* change_directory(node *current, path current_directory){
    cout << "Change directory to: " << endl;
    string new_directory;
    cin >> new_directory;

    vector<node*>::iterator iter;

    //linear search through child vector for the directory
    for (iter = current->children.begin(); iter != current->children.end(); iter++){

        if ((*iter)->name == (current_directory / new_directory)){

            current = *iter;
            return (current);
        }
    }

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
            displayDirectoryStructure(root, 0);
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
