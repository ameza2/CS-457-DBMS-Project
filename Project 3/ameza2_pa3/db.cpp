// Abraham Meza //
// CS 457 - Project 3 //
// November 10, 2021 //

// This program creates a Database Management System (DMS), allowing a user to conduct tuple queries using inner/outer join statements, referencing multiple files/tables for the purpose of mirroring SQlite functionalities and metadata organization. The program has been coded with a variety of fail-safes to prevent runtime errors from user-input. //

/* Future Implementation:

- Add Attribute Case Insensitivity: SQL attributes should be case insensitive. All attributes will be tolower() as a solution.

*/

// Header File(s) //

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <regex>

// Linux Header File(s) //

#include <algorithm>
#include <string>
#include <climits>

// Namespace Declaration(s) //

using namespace std;

// Function Prototype(s) //

// Database Functions (Creation, Deletion, Access) //

void createDatabase(string initialPath, string dataBase); // createDatabase(): creates a database by initializing a directory using specified database name (3rd arg). By passing the root/parent path, the function ensures that users can only create a database while in the root/parent. If they are not, then the function will place them at the root before creating a new database. The function prevents a user from creating an existing database (no overwrite).
void deleteDatabase(string initialPath, string dataBase); // deleteDatabase(): deletes a database by recursively deleteing a database using system calls, removing the specified database name (3rd arg). By passing the root/parent path, the function ensures that users can only delete a database while in the root/parent. If they are not, then the function places them at the root before delete an existing database. The function prevents the user from deleting a database that doesn't exist.
void useDatabase(string initialPath, string dataBase); // useDatabase(): allows a user to access a specific database (2nd arg) to create, use, modify, and delete tables containing metadata. By passing the root/parent path, the function ensures that users can only access databases while in the root/parent. If they are not, then the function will place them at the root before accessing a new database. The function prevents a user from accessing a database that doesn't exist.

// Table Functions (Creation, Deletion, Update, Query) //

void createTable(string tableName, vector<string> attributesName, vector<string> attributesType); // createTable(): creates a table by exporting a .txt file with the table name, that contains specified attribute names/types in a tabular format. The function ensures that users are unable to create tables outside a database. The function prevents users from creating an existing table (no overwrite).
void deleteTable(string tableName); // deleteTable(): deletes a table by removing the file with the specified table name (deallocated properly). The function prevents a user from deleting a table outside the database. The function prevents users from removing a table/file that doesn't exist.
void queryTable(vector<string> query, string tableName, string attributeSearch, string condition, string targetOperand); // queryTable(): fetches table information by reading the specified file/table name, using the query to filter out unnecessary information. The function prevents users from fetching information outside a database. The function prevents users from fetching information from a table/file that doesn't exist. The function can utilize 'where' as a comparison condition when filtering table data
void updateTable(string alter, string tableName, vector<string> attributesName, vector<string> attributesType); // updateTable(): modifies an existing table by appending attribute name/type information to the target file. The function prevents users from modifying an existing table outside a database. The function prevents users from modifying a table/file that doesn't exist.

// Tuple Functions (Insertion, Deletion, Modification) //

void insertTuple(string tableName, vector<string> attributesValue); // insertTuple(): inserts table tuple data by appending given information to the next row of the table. The function prevents users from inserting empty records/samples of data. In addition, the function has a built-in failsafe that prevents users from adding multiple tuples in a single line, while also ensuring that the user can only pass tuples that do not exceed the number of current attributes. If the inserted tuple is missing attribute information, the function initializes empty cells to NULL. The function prevents users from inserting information outside a database. The function prevents users from inserting table/file information to a file that doesn’t exist.
void deleteTuple(string tableName, string attributeSearch, string condition, string targetOperand); // deleteTuple(): deletes tuple data based on a given 'where' comparison (limited to a single condition). The function traverses through each record's attribute data and conducts a conditional statement to filter out data. The indexes (line num.) of values that passed the comparison are pushed into a list. The list of indexes is then referenced to omit corresponding tuple data. To update the table, the function creates a new copy of the file, containing relevant metadata information. Once the target data has been omitted, the function deletes the old state and renames the new file to match the previous. The function prevents users from deleting information outside a database. The function prevents users from deleting table/file information from a file that doesn’t exist.
void modifyTuple(string tableName, string changeAttribute, string changeOperator, string changeValue, string attributeSearch, string condition, string targetOperand); // modifyTuple(): modifies existing tuple data based on a 'where' comparison (limited to a single condition). The function traverse through each record's attribute data and conducts conditional statement to filter out data. The indexes (line num.) of values that passed the comparison are pushed into a list. The list of indexes is then referenced to modifies corresponding tuple data. After modifying the value, the function generates a new copy of the table containing updated information, deletes the old state, then renames the new file to match the previous. The function prevents users from updating information outside a database. The function prevents users from updating table/file information to a file that doesn’t exist.

// Join Functions (Inner Join, Outer Join) //

void innerJoin(vector<string> query, vector<string> tableNames, vector<string> tableAcronyms, string attributeSearch, string condition, string targetOperand); // innerJoin(): fetches intersecting tuple data between two file(s)/table(s) by dissecting each file based on attributeNames and attributeValues. The function conducts validity checks, verifying that the input’s parameterized data exists (i.e., query options, file/table names, matching acronyms, etc.). If an unknown element is introduced, then the function will prompt an error. In addition, the function prevents users from fetching information outside a database. Furthermore, the function prevents users from fetching information from a table/file that doesn't exist. Using a given set of parameterized data, the function uses "on" comparison conditions to filter out rows of data that contain matching values. Tuples that display matching values (intersection) will be included in the query, while those with no matching values (no intersection), will be omitted. Algorithm is very similar to queryTable(); however, uses an additional for-loop to fetch information for an additional file.
void outerJoin(vector<string> query, vector<string> tableNames, vector<string> tableAcronyms, string attributeSearch, string condition, string targetOperand, string direction); // outerJoin(): fetches intersecting tuple data between file(s)/table(s) by dissecting each file based on attributeNames and attributeValues. However, unlike innerJoin(), outerJoin() will return every row from one specified table, even if the join condition fails. The specified table is based on a directional keyword (e.g., LEFT, RIGHT, and FULL). The function conducts validity checks, verifying that the input’s parameterized data exists (i.e., query options, file/table names, matching acronyms, etc.). If an unknown element is introduced, then the function will prompt an error. In addition, the function prevents users from fetching information outside a database. Furthermore, the function prevents users from fetching information from a table/file that doesn't exist. Using a given set of parameterized data, the function uses "on" comparison conditions to filter out rows of data that contain matching values. Tuples that display matching values (intersection) will be included in the query. If the tuple does not contain matching values no intersection), then the program will print the tuple information of the specified directional table. The program will then initialize the empty cells of failed join tuples to NULL. Algorithm is very similar to queryTable(); however, uses an additional for-loop to fetch information for an additional file.

// Helper Functions //

bool isNumber(const string& s); // isNumber(): identifies if the value of a string is a real number by looking for non-numerical characters

// Main Program //

int main() {

    // Variable Declaration / Initialization //

    int found; // helper-variable: used to record the position of a substring from str.find(); solution to prevent different signedness comparison issues (better than type-casting)
    size_t commandCounter = 0; // helper-variable: used to track the index position of the command (especially when reading multiple attributes/conditions) by using list of words

    bool error = false; // boolean variable: fail-safe helper (record error status)
    bool joinInner = false; // boolean variable: fail-safe helper (inner join)
    bool joinOuter = false; // boolean variable: fail-safe helper (outer join)
    bool outerDirection = false; // boolean variable: fail-safe helper (outer join) to determine if OUTER JOIN direction exists

    char delimiter(';'); // delimiter variable: used for getline() delimiter (stop reading when reach ";")
    char cwdHome[PATH_MAX]; // buffer variable: used for getcwd() to reflect home/root directory

    string caseTemp; // variable (helper): used to store UPPER-CASE version of a given string (CASE SENSITIVE COMPARISON CHECKS)
    string command; // variable: used to take user-input database command input
    string nameStructure; // variable: used to record database | table label (provided by command var.)
    string keyword; // variable: used to store parsed keyword of user-input command (vector[0]): CASE SENSITIVE ISSUES
    string structure; // variable: used to store parsed stucture of user-input command (vector[1]): CASE SENSITIVE ISSUES
    string indication; // variable: used to store parsed target of user-input command (vector[2]): CASE SENSITIVE ISSUES
    string action; // variable: used to store parsed alter option of user-input comman (vector[3]): CASE SENSITIVE ISSUES
    string tupleKey; // variable: used to store parsed conditional tuple data keyword: CASE SENSITIVE ISSUES
    string modifyKey; // variable: used to store parsed modify tuple data keyword: CASE SENSITIVE ISSUES
    string modifyAttribute; // variable: used to store parsed target attribute for tuple modification
    string modifyOperator; // variable: used to store parsed operator for tuple modification
    string modifyValue; // variable: used to store parsed attribute value for tuple modification
    string targetAttribute; // variable: used to store parsed target attribute for condition statement
    string tupleConditon; // variable: used to store parsed tuple condition operator
    string tupleOperand; // variable: used to store parsed tuple condition operand
    string joinCheck; // variable: used to store parsed query command to identify the presence of a JOIN (should multiple files be referenced?)
    string outerType; // variable: used to store parsed query command to identify OUTER JOIN type
    string rootDirectory = getcwd(cwdHome, sizeof(cwdHome)); // comparison variable: record current home directory for future comparisons (i.e., changing between directories, etc.)

    const char * databaseDirectory = rootDirectory.c_str(); // variable: used to check if current directory is root/parent directory; strcmp() only takes in char * inputs

    vector<string> query; // vector isntance: used to store parsed query option(s) of user-input command
    vector<string> words; // vector instance: used to store parsed string to identify keyword, stucture, and label
    vector<string> queryParse; // vector instance: used to store parsed string to identify file queries and conditions (JOIN).
    vector<string> attributeName; // vector instance: used to store parsed string to identify attributes names for "TABLE"
    vector<string> attributeType; // vector instance: used to store parsed string to identify attribute types for "TABLE"
    vector<string> attributeValue; // vector instance: used to store parsed string to identify attribute values for "TUPLE"

    vector<string> queryFiles;
    vector<string> queryAcronyms;
    vector<string> joinType;

    cout << "\n" <<
            "/////////////////////////////\n" <<
            "/////  Database Program /////\n" <<
            "/////////////////////////////" << endl << endl; // test label design */

    while (keyword != ".EXIT") { // while loop: user-interface menu (database/table options) - continue to display menu until user exits the program

        // Cin User Input //
        
        getline(cin, command, delimiter); // read user input and store value into option

        cin.ignore(); // ignore new-line character input: clear cin buffer (prevents accidental new line from being seen as the keyword (first word))

        command.erase(remove(command.begin(), command.end(), '\n'), command.end()); // remove all newline ('\n') characters from user-input (prevents '\n' in commands)
        command.erase(remove(command.begin(), command.end(), '\r'), command.end()); // remove all return ('\r') characters from user-input (prevents '\r' in commands)

        replace(command.begin(), command.end(), '\t', ' '); // replace(): replace all tab characters from user-input string with a space " " (prevents '/t' in commands, and is used to seperate parameter values): solves issue in which tab characters are not considered as space characters for parsing
        
        size_t n = count(command.begin(), command.end(), ')'); // variable: stores the number of character ',' occurrences

        while (n != 0) { // while loop: while the num. of commas [','] is != 0, continue to continue to insert empty space after each ',' (seperation) 
            found = command.find(","); // some paramterized entries do not have a space seperating each element; therefore, string will be parsed incorrectly -> insert a space between phrases and ','

            if(found != -1) { // if statement: if ',' is a substring of a given phrase (combined with a word), insert a space " " after to ensure proper parsing
                command = command.insert(found + 1, " "); // insert space " " after ','
            }

            n--; // num. of commas derement
        }

        found = command.find("("); // some paramterized entries do not have a space seperating structure/tuple keyword; therefore, string will be parsed incorrectly -> insert a space between phrases and '('

        if (found != -1) { // if statement: if first '(' is a substring of a given phrase (combined with a word), insert a space " " to ensure proper parsing
            command = command.insert(found, " "); // insert space " " between word and '('
        }

        if (command.front() != '-' && !command.empty()) { // if statement: if command begins with '-' (comments) or isEmpty(), ignore and getline() once again (do not proceed with program)
            stringstream ss(command); // stringstream classs ss: string command treated as stream (parsing -> extraction)
            string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (words)

            while (getline(ss, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                size_t n = count(temp.begin(), temp.end(), ')'); // variable: stores the number of character ')' occurrences

                found = temp.find("char"); // varchar/char variables use '()' to signify variable size -> if command contains varchar(x), do not remove ending ')'

                while (temp.front() == '(') { // while loop: while the string begins with a '(' (signifies start of parameter), remove all '(' for clean parsing
                    temp.erase(temp.find_first_of('('), 1); // str.erase(): remove '(' from string
                }

                if (found == -1) { // while loop: if "char" is a not substring of the user-input, remove all ')' for clean parsing
                    while (temp.back() == ')') { // while loop: while the string ends with a ')' (signifies end of parameter), remove all ')' for clean parsing
                        temp.erase(temp.find_last_of(')'), 1); // str.erase(): remove ')' from string
                    }
                }
                else if (found != 1 && n > 1) { // else if statement: else if "char" is a substring of the user-input, and the variable's parameters has more than one ')' character
                    while (n != 1) { // while loop: while the num. of ending parenthesis ['('] is != 1, continue to remove until matching parenthesis  
                        temp.erase(temp.find_last_of(')'), 1); // str.erase(): remove ')' from string
                        n--; // num. of ending parenthesis decrement
                    }
                }

                if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                    words.push_back(temp); // push_back(): push current string value into vector (list of words)
                }
            }

            //              TESTING SCRIPT              //
            //                                          //
            // for (size_t i = 0; i < words.size(); i++) {  //
            //    cout << words.at(i) << endl;          //
            //}                                         //

            keyword = words.at(0); // variable: record command keyword (first element of the vector -> word[0])
            transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper); // transform(..., ::toupper): var. keyword is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues                

            found = keyword.find(".EXIT"); // ".EXIT" does not use ';' as a delimiter; therefore, program needs to parse ".EXIT" to determine if the program should end (discard anything that follows ".EXIT")

            if (found != -1) { // if statement: if ".EXIT" is a substring of the user-input, parse ".EXIT"
                keyword = (keyword.substr(found , 5)).c_str(); // re-assign original command string to reflect ".EXIT"
            }

            // DECISION TREE BASED ON WORD PARSING //

            if (keyword == "CREATE") { // if statement: if the keyword is create, navigate decision tree to determine structure and label, then apply corresponding algorithm (i.e., create -> database > database name, create -> table -> table features)
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    structure = words.at(1); // variable: record command structure (second element of the vector -> vector[1])
                    transform(structure.begin(), structure.end(), structure.begin(), ::toupper); // transform(..., ::toupper): var. structure is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                    nameStructure = words.at(2); // variable: record command structure (database | table) name (third element of the vector -> vector[2])// variable: record command structure (database | table) name (third element of the vector -> vector[2])
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }
        
                if (error != true) {  // if statement: if no error occured (parameter read was successful), attempt to create a new file/table
                    if (structure == "DATABASE") { // if statement: if the structure is a database, conduct database creation algorithm
                        createDatabase(rootDirectory, nameStructure); // createDatabase(): create new database / directory
                    }
                    else if (structure == "TABLE") { // else if statement: if the structure is a table, conduct table creation algorithm
                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                            for (size_t i = 3; i < words.size(); i += 2) { // for loop: if structure was a "TABLE", record elements > 3 as parameter variables for "TABLE" (e.g., attributeName and attributeType)
                                attributeName.push_back(words.at(i)); // push_back(): push current string value into vector (list of attribute names)

                                found = words.at(i).find(','); // variable: stores position of substring ',' if found

                                if (found != -1) { // if statement: if attributeName (first string of first parameter entry) contains a ',', then next string is not an attributeType (is another attributeName) -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }
                                
                                attributeType.push_back(words.at(i + 1)); // push_back(): push current string value into vector (list of attribute types)
                            }
                        }
                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                            error = true; // bool: ERROR = TRUE -> failsafe
                        }

                        for (size_t i = 0; i < attributeType.size(); i++) { // for loop: used to traverse vector attributeType elements: remove ',' characters
                            attributeType.at(i).erase(remove(attributeType.at(i).begin(), attributeType.at(i).end(), ','), attributeType.at(i).end()); // str.erase(): remove all ',' from user-input string for clean parsing
                        }

                        if (error != true) { // if statement: if no error occured (parameter read was successful), create a file/table
                            if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                                cerr << "ERROR: Table " << nameStructure << " cannot be created outside a database." << endl; // error message: cannot make a table without a database
                            }
                            else { // else statement: else, if inside a non-root directory (a.k.a. inside a database directory), allow user to create file/table
                                createTable(nameStructure, attributeName, attributeType); // createTable(): create new file/table
                            }
                        }
                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not create table)
                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name (attr.name attr.type, ...)]" << endl; // error message (unstable parameters)
                        }
                    }
                    else { // else statement: if structure is not recognized, then output an error code to the user
                        if (!structure.empty()) { // if structure is not recognized && structure is not empty ("" | " "), then prompt error code (structure not recognized)
                            cerr << "ERROR: Structure \"" << structure << "\" not recognized! Database cannot be created. Please try again!" << endl; // error message: user structure parameter is not recognized (!DATABASE | !TABLE) 
                        }
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not attempt to alter an existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "DROP") { // else if statement: if the keyword is delete, navigate decision tree to determine structure and label, then apply corresponding algorithm (i.e., delete -> database > database name, delete -> table -> table features)
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    structure = words.at(1); // variable: record command structure (second element of the vector -> vector[1])
                    transform(structure.begin(), structure.end(), structure.begin(), ::toupper); // transform(..., ::toupper): var. structure is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                    nameStructure = words.at(2); // variable: record command structure (database | table) name (third element of the vector -> vector[2])
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if (error != true) { // if statement: if no error occured (parameter read was successful), delete an existing file/table

                    if (structure == "DATABASE") { // if statement: if the structure is a database, conduct database deletion algorithm
                        deleteDatabase(rootDirectory, nameStructure); // deleteDatabase(): delete existing database / directory
                    }
                    else if (structure == "TABLE") { // else if statement: if the structure is a table, conduct table deletion algorithm
                        if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot drop a table (prompt error message)
                            cerr << "ERROR: Table " << nameStructure << " cannot be deleted outside a database." << endl; // error message: cannot search a table without a database
                        }
                        else { // else statement: if not inside of root directory (inside database), delete target file/table
                            deleteTable(nameStructure); // deleteTable(): delete exisiting database/directory
                        }
                    }
                    else{  // else statement: if structure is not recognized, then output an error code to the user
                        cerr << "ERROR: Structure \"" << structure << "\" not recognized! Database cannot be deleted. Please try again!" << endl; // error message: user structure parameter is not recognized (!DATABASE | !TABLE) 
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do delete an existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "USE") { // else if statement: if the keyword is use, enter corresponding database instance for data usage
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    nameStructure = words.at(1); // record database name from 2nd element in the vector (FORMAT: keyword name)
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }
                
                if (error != true) { // if statement: if no error occured (parameter read was successful), access an existing file/table
                    useDatabase(rootDirectory, nameStructure); // useDatabase(): open existing database / directory
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not access existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword name]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "SELECT") { // else if statement: if the keyword is select, fetch corresponding table information for data usage
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                    commandCounter = 0; // reset commandCounter for new index tracking
                    
                    do {
                        commandCounter++; // commandCounter increment (command index position)
                        query.push_back(words.at(commandCounter)); // push_back(): push current query option into a query vector (list of queries)

                        found = words.at(commandCounter).find(","); // look for ',' characters within possible query options (check if there are multiple queries)
                    } while (found != -1); // do-while loop: continue to record query option until entry does not have a ',' character

                    for (size_t i = 0; i < query.size(); i++) { // for loop: for each query option, remove all ',' character from user-input string for clean parsing
                        query.at(i).erase(remove(query.at(i).begin(), query.at(i).end(), ','), query.at(i).end()); // remove all ',' characters from query.element string
                    }

                    indication = words.at(commandCounter + 1); // record target from commandCounter + 1 element in the vector [FORMAT: keyword query indication name (where attribute condition operand)]
                    transform(indication.begin(), indication.end(), indication.begin(), ::toupper); // transform(..., ::toupper): var. indication is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if (error != true) { // if statement: if no error occured (parameter read was successful), query an existing file/table
                    if (indication == "FROM") { // if statement: if indication == "FROM", attempt to query existing table using user parameters
                        for (size_t i = commandCounter + 2; i < words.size(); i++) { // for loop: for each parsed query phrase, re-construct the string for futuer joinCheck condition
                            joinCheck = joinCheck.append(words.at(i) + " "); // create a string using remaining query phrases
                        }

                        found = joinCheck.find(","); // using re-constructued string, look for ',' characters; if nameStructure utilizes ',', then the command is referencing multiple files for the designatede query -> JOIN

                        if (found != -1) { // if statement: if ',' found in remaining query command, replace ',' with default join key word(s) : DEFAULT = INNER JOIN
                            joinCheck = regex_replace(joinCheck, regex(","), " INNER JOIN"); // replace occurences of ',' with "inner join" key phrase
                            
                            caseTemp = joinCheck; // create temp string instance (case-sensitivity)
                            transform(caseTemp.begin(), caseTemp.end(), caseTemp.begin(), ::toupper); // transform(..., ::toupper): var. caseTemp (tupleKey) is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                            found = caseTemp.find("WHERE"); // using default join string, look for "where" conditional statement; if newly generated command utilizes "where" in combination with "join", then query command must change conditional keyword to reflect proper structure

                            if (found != -1) { // if statement: if "where" found in new default join string, replace "where" with default join condition "on"
                                joinCheck.replace(found, 5, "ON"); // replace occurence of "WHERE" conditional statement with "ON" to represent default join conditional keyword
                            }

                            caseTemp.clear(); // reset variable value for new temp string instance
                        }

                        caseTemp = joinCheck; // create temp string instance (case-sensitivity)
                        transform(caseTemp.begin(), caseTemp.end(), caseTemp.begin(), ::toupper); // transform(..., ::toupper): var. caseTemp is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                        found = caseTemp.find("JOIN"); // using re-constructed string, look for "join" keyword to dictate if query command requires JOIN execution

                        if (found != -1) { // if statement: if "JOIN" found in remaining query command, conduct JOIN query instruction (multiple file(s)/table(s))

                            // cout << joinCheck << endl; // cout confirmation (TESTING) //

                            stringstream joinstr(joinCheck); // stringstream classs joinstr: string command treated as stream (parsing -> extraction)
                            string joinTemp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (queryParse)

                            while (getline(joinstr, joinTemp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                                if (!joinTemp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                                    queryParse.push_back(joinTemp); // push_back(): push current string value into vector (list of words)
                                }
                            }

                            if (queryParse.size() >= 10) { // if statement: if queryParse contains 10 or more elements, then there exists sufficient parameters to conduct a normal join operation
                                size_t queryCounter = 0; // variable: used as a query phrase index tracker

                                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                                    queryFiles.push_back(queryParse.at(queryCounter)); // record first file to be referenced during join execution from queryCounter in the vector [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                    queryAcronyms.push_back(queryParse.at(queryCounter + 1)); // record first query file acronym to be referenced during join execution from queryCounter in the vector [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                }
                                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }

                                if (error != true) { // if statement: if no error occured (parameter read was successful), fetch query join statement
                                    try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                                        commandCounter = queryCounter + 1; // re-assign commandCounter (index tracking variable)
                                        
                                        do {
                                            commandCounter++; // commandCounter increment (command index position)

                                            joinType.push_back(queryParse.at(commandCounter)); // push_back(): push join statement phrase into a joinType vector (parsed join statement) [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                            transform(joinType.at(commandCounter - 2).begin(), joinType.at(commandCounter - 2).end(), joinType.at(commandCounter - 2).begin(), ::toupper); // transform(..., ::toupper): join statemets are case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                                            found = joinType.at(commandCounter - 2).find("JOIN"); // look for "JOIN" substring within join statement (check when join statement has been recorded entirely)
                                        } while (found == -1); // do-while loop: continue to record join statement until "JOIN" keyword is detected
                                    }
                                    catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                        error = true; // bool: ERROR = TRUE -> failsafe
                                    }

                                    if (error != true) { // if statement: if no error occured (parameter read was successful), determine query join statement (inner || outer)
                                        int listSize = joinType.size(); // variable: size_t (long unsigned int) -> int (int) 
                                        
                                        for (int i = 0; i < listSize; i++) { // for loop: for each parsed join-statement phrase, conduct a search to determine whether join statement is inner or outer (seperation of join statements)
                                            found = joinType.at(i).find("INNER"); // look for "INNER" keyword to determine type of join execution

                                            if (found != -1) { // if statement: if "INNER" found in join statement, conduct an INNER JOIN (joinInner = true)
                                                joinInner = true; // boolean: joinInner = true
                                            }

                                            found = joinType.at(i).find("OUTER"); // look for "OUTER" keyword to determine type of join execution

                                            if (found != -1) { // if statement: if "OUTER" found in join statement, conduct an OUTER [DIRECTION] JOIN (joinOuter = true)
                                                joinOuter = true; // boolean: joinOuter = true

                                                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                                                    found = joinType.at(i - 1).find("LEFT"); // if join statement is OUTER JOIN, look for direction LEFT

                                                    if (found != -1) { // if statement: if LEFT OUTER JOIN (record LEFT keyword) 
                                                        outerType = joinType.at(i - 1); // LEFT OUTER JOIN
                                                        outerDirection = true; // boolean: OUTER JOIN direction found
                                                    }

                                                    found = joinType.at(i - 1).find("RIGHT"); // if join statement is OUTER JOIN, look for direction RIGHT

                                                    if (found != -1) { // if statement: if RIGHT OUTER JOIN (record RIGHT keyword) 
                                                        outerType = joinType.at(i - 1); // RIGHT OUTER JOIN 
                                                        outerDirection = true; // boolean: OUTER JOIN direction found
                                                    }

                                                    found = joinType.at(i - 1).find("FULL"); // if join statement is OUTER JOIN, look for direction FULL

                                                    if (found != -1) { // if statement: if FULL OUTER JOIN (record FULL keyword) 
                                                        outerType = joinType.at(i - 1); // FULL OUTER JOIN 
                                                        outerDirection = true; // boolean: OUTER JOIN direction found
                                                    }

                                                    if (outerDirection != true) { // if statement: if OUTER JOIN statement does not indicate a direction, prompt an error message                                                    
                                                        error = true; // error = true
                                                    }
                                                }
                                                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                                    error = true; // error = true
                                                }
                                            }
                                        }

                                        if (error != true) { // if statement: if no error occured (parameter read was successful), check for additional parameters
                                            commandCounter += 1; // re-assign commandCounter (index tracking variable)

                                            try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                                                queryFiles.push_back(queryParse.at(commandCounter)); // record second file to be referenced during join execution from queryCounter in the vector [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                                queryAcronyms.push_back(queryParse.at(commandCounter + 1)); // record second query file acronym to be referenced during join execution from queryCounter in the vector [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]

                                                tupleKey = queryParse.at(commandCounter + 2); // fetch conditional keyword from commandCounter + 2 in the vector for conditional comparion [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                                transform(tupleKey.begin(), tupleKey.end(), tupleKey.begin(), ::toupper); // transform(..., ::toupper): var. tupleKey is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                                            }
                                            catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                                error = true; // bool: ERROR = TRUE -> failsafe
                                            }

                                            if (error != true) { // if statement: if no error occured (parameter read was successful), attempt to fetch query conditional statement 
                                                if (tupleKey == "ON") { // if statement: if tupleKey == "ON", fetch comparison condition
                                                    try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                                                        targetAttribute = queryParse.at(commandCounter + 3); // record target attribute from commandCounter + 3 element in the vector for conditional comparison [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                                        tupleConditon = queryParse.at(commandCounter + 4); // record tuple condition operator from commandCounter + 4 element element in the vector for conditional comparison [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                                            
                                                        queryParse.at(commandCounter + 5).erase(remove(queryParse.at(commandCounter + 5).begin(), queryParse.at(commandCounter + 5).end(), '\''), queryParse.at(commandCounter + 5).end()); // str.erase(): remove all ' \' ' from user-input string for clean parsing
                                                        tupleOperand = queryParse.at(commandCounter + 5); // record tuple operand from commandCounter + 5 in the vector for conditional comparison [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]
                                                    }
                                                    catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                                        error = true; // bool: ERROR = TRUE -> failsafe
                                                    }

                                                    if (error != true) { // if statement: if no error occured (parameter read was successful), check for additional parameters
                                                        if (queryParse.size() > (commandCounter + 6)) { // if statement: if queryParse.size() > commandCounter + 5 -> too many parameters passed (TEMP SOLUTION: limit to a single conditional statement)
                                                            cerr << "ERROR: Too many parameters. Please try again! [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]" << endl; // error message (unstable parameters)
                                                        }
                                                        else { // else statement: query tuple data
                                                            if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                                                                cout << "ERROR: Table(s) "; // cout confirmation

                                                                for (size_t i = 0; i < queryFiles.size(); i++) { // for loop: include each queryFile in the query instruction in the prompted error message
                                                                    cout << queryFiles.at(i); // cout confirmation

                                                                    if (i != queryFiles.size() - 1) { // if statement: if multiple file(s), add ' \' ' character for grammatical seperation
                                                                        cout << ", "; // cout confirmation

                                                                        if (i == queryFiles.size() - 2) { // if statement: if on final file, add "and" for grammatical seperation 
                                                                            cout << "and "; // cout confirmation
                                                                        }
                                                                    }
                                                                }
                                                                
                                                                cerr << " cannot be searched outside a database." << endl; // error message: cannot search a table without a database
                                                            }
                                                            else { // else statement: if not inside of root directory (inside database), query target file/table
                                                                if (joinInner == true && joinOuter == false) { // if statement: if join statement only contains the "INNER" keyword, execute INNER JOIN algorithm
                                                                    innerJoin(query, queryFiles, queryAcronyms, targetAttribute, tupleConditon, tupleOperand); // innerJoin(): conduct a query inner join on an existing table using a comparison condition
                                                                }
                                                                else if (joinOuter == true && joinInner == false) { // else if statement: else if join statement only contains the "OUTER" keyword, execute OUTER [DIRECTION] JOIN algorithm
                                                                    outerJoin(query, queryFiles, queryAcronyms, targetAttribute, tupleConditon, tupleOperand, outerType); // outerJoin(): conduct a query outer join on an existing table using a comparison condition
                                                                }
                                                                else { // else statement: else if join statement contains both "INNER" and "OUTER" keywords, prompt error message (NOT ALLOWED)
                                                                    cerr << "ERROR: Invalid Table Query. Table queries cannot combine inner and outer join statements. Please try again!" << endl; // error message (too many join types)
                                                                }
                                                            }
                                                        }
                                                    }   
                                                    else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                                                        cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]" << endl; // error message (unstable parameters)
                                                    }
                                                }
                                                else { // else statement: if tupleKey string not recognized, prompt error message (do not query existing file/table)
                                                    cerr << "ERROR: Conditional Keyword \"" << tupleKey << "\" not recognized! Tuple cannot be queried. Please try again!" << endl; // error message: user value parameter is not recognized (!WHERE)  
                                                }
                                            }
                                            else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                                                cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]" << endl; // error message (unstable parameters)
                                            }
                                        }
                                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                                            cerr << "ERROR: Invalid Outer Join Statement! No Valid Direction Indicated! (e.g., left outer join | right outer join)" << endl;
                                        }
                                    }
                                    else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                                        cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]" << endl; // error message (unstable parameters)
                                    }
                                }
                                else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication table_1 acronym_1 join_statement table_2 acronym_2 on (attribute condition operand)]" << endl; // error message (unstable parameters)
                                }
                            }
                            else { // else statement: if queryParse contains less than 10 elements, then there are not enough parameters to conduct a normal join operation
                                cerr << "ERROR: Insufficient Parameters to Conduct a Join Operation! Please try again!" << endl;
                            }
                        }
                        else { // else statement: else if "join" is not found in remaining query command, conduct normal query instruction (single file/table)           
                            nameStructure = words.at(commandCounter + 2); // record table name from commandCounter + 2 element in the vector [FORMAT: keyword query indication name (where attribute condition operand)]

                            if (words.size() > (commandCounter + 3)) { // if statement: if more parameters exist, fetch remaining parameters as a conditional statement
                                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                                    commandCounter = commandCounter + 3; // reset commandCounter for new index tracking
                                    
                                    tupleKey = words.at(commandCounter); // record comparison tuple keyword from commoundCounter + 3 element in the vector [FORMAT: keyword name set attribute value where attribute condition operand]
                                    transform(tupleKey.begin(), tupleKey.end(), tupleKey.begin(), ::toupper); // transform(..., ::toupper): var. tupleKey is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                                }
                                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }

                                if (error != true) { // if statement: if no error occured (parameter read was successful), check for comparison keyword and fetch comparison condition
                                    if (tupleKey == "WHERE") { // if statement: if tupleKey == "WHERE", fetch comparison condition
                                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error                    
                                            targetAttribute = words.at(commandCounter + 1); // record target attribute from commandCounter + 1 element in the vector for conditional comparison [FORMAT: keyword name set attribute value where attribute condition operand]
                                            tupleConditon = words.at(commandCounter + 2); // record tuple condition operator from commandCounter + 2 element element in the vector for conditional comparison [FORMAT: keyword name set attribute value where attribute condition operand]
                                                
                                            words.at(commandCounter + 3).erase(remove(words.at(commandCounter + 3).begin(), words.at(commandCounter + 3).end(), '\''), words.at(commandCounter + 3).end()); // str.erase(): remove all ' \' ' from user-input string for clean parsing
                                            tupleOperand = words.at(commandCounter + 3); // record tuple operand from commandCounter + 3 in the vector for conditional comparison [FORMAT: keyword name set attribute value where attribute condition operand]
                                        }
                                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                            error = true; // bool: ERROR = TRUE -> failsafe
                                        }
                                        
                                        if (error != true) { // if statement: if no error occured (parameter read was successful), check for additional parameters
                                            if (words.size() > (commandCounter + 4)) { // if statement: if words.size() > commandCounter + 4 -> too many parameters passed (TEMP SOLUTION: limit to a single conditional statement)
                                                cerr << "ERROR: Too many parameters. Please try again! [FORMAT: keyword query indication name (where attribute condition operand)]" << endl; // error message (unstable parameters)
                                            }
                                            else { // else statement: query tuple data
                                                if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                                                    cerr << "ERROR: Table " << nameStructure << " cannot be searched outside a database." << endl; // error message: cannot search a table without a database
                                                }
                                                else { // else statement: if not inside of root directory (inside database), query target file/table
                                                    queryTable(query, nameStructure, targetAttribute, tupleConditon, tupleOperand); // queryTable(): query an existing table using a comparison condition
                                                }
                                            }
                                        }
                                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication name (where attribute condition operand)]" << endl; // error message (unstable parameters)
                                        }
                                    }
                                    else { // else statement: if tupleKey string not recognized, prompt error message (do not query existing file/table)
                                        cerr << "ERROR: Conditional Keyword \"" << tupleKey << "\" not recognized! Tuple cannot be queried. Please try again!" << endl; // error message: user value parameter is not recognized (!WHERE)  
                                    }
                                }
                                else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication name (where attribute condition operand)]" << endl; // error message (unstable parameters)
                                }
                            }
                            else { // else statement: if no error occured (parameter read was successful), and no more remaining parameters (no comparison condition), query an existing file/table
                                targetAttribute.clear(); // initialize targetAttribute to NULL
                                tupleConditon.clear(); // initialize tupleCondition to NULL
                                tupleOperand.clear(); // initialize tupleOperand to NULL

                                if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                                    cerr << "ERROR: Table " << nameStructure << " cannot be searched outside a database." << endl; // error message: cannot search a table without a database
                                }
                                else { // else statement: if not inside of root directory (inside database), query target file/table
                                    queryTable(query, nameStructure, targetAttribute, tupleConditon, tupleOperand); // queryTable(): query an existing table
                                }
                            }
                        }
                    }
                    else { // else statement: if indication string not recognized, prompt error message (cannot query file/table source)
                        cerr << "ERROR: Indication \"" << indication << "\" not recognized. Please try again!" << endl; // error message: user indication parameter is not recognized (!FROM)
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not query existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword query indication name (where attribute condition operand)]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "ALTER") { // else if statement: if the keyword is alter, alter corresponding table (attribute) information for data modification
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    structure = words.at(1); // variable: record command structure (second element of the vector -> vector[1])
                    transform(structure.begin(), structure.end(), structure.begin(), ::toupper); // transform(..., ::toupper): var. structure is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if (error != true) { // if statement: if no error occured (parameter read was successful), attempt to alter an existing file/table
                    if (structure == "TABLE") { // if statement: if structure == "TABLE", attempt to alter/update table using user parameters
                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                            nameStructure = words.at(2); // record table name from 3rd element in the vector (FORMAT: keyword structure name action attributeName attributeType)

                            action = words.at(3); // record alter instruction from 4th element in the vector (FORMAT: keyword structure name action attributeName attributeType)
                            transform(action.begin(), action.end(), action.begin(), ::toupper); // transform(..., ::toupper): var. action is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues

                            for (size_t i = 4; i < words.size(); i += 2) { // for loop: if structure was a "TABLE", record elements > 4 as parameter variables for "TABLE" (e.g., attributeName and attributeType)
                                attributeName.push_back(words.at(i)); // push_back(): push current string value into vector (list of attribute names)

                                found = words.at(i).find(','); // variable: stores position of substring ',' if found

                                if (found != -1) { // if statement: if attributeName (first string of first parameter entry) contains a ',', then next string is not an attributeType (is another attributeName) -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }
                                
                                attributeType.push_back(words.at(i + 1)); // push_back(): push current string value into vector (list of attribute types)
                            }
                        }
                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                            error = true; // bool: ERROR = TRUE -> failsafe
                        }

                        for (size_t i = 0; i < attributeType.size(); i++) { // for loop: used to traverse vector attributeType elements: remove ',' characters
                            attributeType.at(i).erase(remove(attributeType.at(i).begin(), attributeType.at(i).end(), ','), attributeType.at(i).end()); // str.erase(): remove all ',' from user-input string for clean parsing
                        }

                        if (error != true) { // if statement: if no error occured (parameter read was successful), create a file/table
                            if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot create a table (prompt error message)
                                cerr << "ERROR: Table " << nameStructure << " cannot be altered outside a database." << endl; // error message: alter a table without a database
                            }
                            else { // else statement: else, if inside a non-root directory (a.k.a. inside a database directory), allow user to create file/table
                                updateTable(action, nameStructure, attributeName, attributeType); // updateTable(): update existing file/table
                            }
                        }
                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not create table)
                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name action (attr.name attr.type, ...)]" << endl; // error message (unstable parameters)
                        }
                    }
                    else { // else statement: if structure is not recognized, then output an error code to the user
                        if (!structure.empty()) { // if structure is not recognized && structure is not empty ("" | " "), then prompt error code (structure not recognized)
                            cerr << "ERROR: Structure \"" << structure << "\" not recognized! Database cannot be deleted. Please try again!" << endl; // error message: user structure parameter is not recognized (!TABLE) 
                        }
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not attempt to alter an existing file/table)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword structure name action (attr.name attr.type, ...)]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "INSERT") { // else if statement: if the keyword is insert, insert given tuple data into corresponding table for data collection
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    indication = words.at(1); // record indicator from 2nd element in the vector [FORMAT: keyword indication name values(attri.values)]
                    transform(indication.begin(), indication.end(), indication.begin(), ::toupper); // transform(..., ::toupper): var. indication is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if (error != true) { // if statement: if no error occured (parameter read was successful), fetch indicator value
                    if (indication == "INTO") { // if statement: if indication == "INTO", attempt to fetch existing table using user parameters
                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                            nameStructure = words.at(2); // record table name from 3rd element in the vector [FORMAT: keyword indication name values(attri.values)]

                            tupleKey = words.at(3); // record tuple keyword from 4th element in the vector [FORMAT: keyword indication name values(attri.values)]
                            transform(tupleKey.begin(), tupleKey.end(), tupleKey.begin(), ::toupper); // transform(..., ::toupper): var. tupleKey is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                        }
                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                            error = true; // bool: ERROR = TRUE -> failsafe
                        }

                        if (error != true) { // if statement: if no error occured (parameter read was successful), check indicator value to determine tuple insert
                            if (tupleKey == "VALUES") { // if statement: if tupleKey == "VALUES", insert tuple parameters into the corresponding existing table 
                                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                                    for (size_t i = 4; i < words.size(); i += 1) { // for loop: if tuple value entry, record elements > 4 as attribute values for table entry (e.g., "attri.value(s)")
                                        words.at(i).erase(remove(words.at(i).begin(), words.at(i).end(), ','), words.at(i).end()); // str.erase(): remove all ',' from user-input string for clean parsing
                                        words.at(i).erase(remove(words.at(i).begin(), words.at(i).end(), '\''), words.at(i).end()); // str.erase(): remove all ' \' ' from user-input string for clean parsing
                                        words.at(i).erase(remove(words.at(i).begin(), words.at(i).end(), ' '), words.at(i).end()); // str.erase(): remove all whitespace from user-input string

                                        if (!words.at(i).empty()) { // if statement: if current element in items list contains data (not empty), push value into attributeList (list of attribute values)
                                            attributeValue.push_back(words.at(i)); // push_back(): push current string value into vector (list of attribute values)
                                        }
                                    }
                                }
                                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }

                                if (error != true) { // if statement: if no error occured (parameter read was successful), insert tuple entry
                                    if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot insert a tuple (prompt error message)
                                        cerr << "ERROR: " << nameStructure << " tuple cannot be inserted outside its database." << endl; // error message: cannot insert a tuple outside of a database (needs corresponding table)
                                    }
                                    else { // else statement: else, if inside a non-root directory (a.k.a. inside a database directory), allow user to insert a tuple
                                        insertTuple(nameStructure, attributeValue); // insertTuple(): insert given tuple data into the table
                                    }
                                }
                                else { // else statement: if error occured (parameter read not successful), prompt error message (do not insert tuple)
                                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword indication name values(attri.values)]" << endl; // error message (unstable parameters)
                                }
                            }
                            else { // else statement: if tupleKey string no recognized, prompt error message (do not insert tuple data)
                                cerr << "ERROR: Tuple Keyword \"" << tupleKey << "\" not recognized! Tuple cannot be inserted. Please try again!" << endl; // error message: user value parameter is not recognized (!VALUES)
                            }
                        }
                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not insert tuple data)
                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword indication name values(attri.values)]" << endl; // error message (unstable parameters)
                        }
                    }
                    else { // else statement: if indication string not recognized, prompt error message (cannot insert tuple data)
                        cerr << "ERROR: Indication \"" << indication << "\" not recognized. Please try again!" << endl; // error message: user indication parameter is not recognized (!INTO)
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not insert tuple data)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword indication name values(attri.values)]" << endl; // error message (unstable parameters)
                }
            }
            else if (keyword == "DELETE") { // else if statement: if the keyword is delete, delete tuple data that satisfies given condition for data modification
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    indication = words.at(1); // record target from 2nd element in the vector (FORMAT: keyword target name where condition)
                    transform(indication.begin(), indication.end(), indication.begin(), ::toupper); // transform(..., ::toupper): var. indication is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if (error != true) { // if statement: if no error occured (parameter read was successful), fetch indication instruction
                    if (indication == "FROM") { // if statement: if indication == "FROM", attempt to query existing table using user parameters                        
                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                            nameStructure = words.at(2); // record table name from 3rd element in the vector [FORMAT: keyword target name where attribute condition operand]

                            tupleKey = words.at(3); // record tuple keyword from 4th element in the vector [FORMAT: keyword target name where attribute condition operand]
                            transform(tupleKey.begin(), tupleKey.end(), tupleKey.begin(), ::toupper); // transform(..., ::toupper): var. tupleKey is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                        }
                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                            error = true; // bool: ERROR = TRUE -> failsafe
                        }

                        if (error != true) { // if statement: if no error occured (parameter read was successful), fetch tupleKey instruction
                            if (tupleKey == "WHERE") { // if statement: if tupleKey == "WHERE", combine remianing parameters into a conditonal statement for tuple deletion
                                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                                    
                                    targetAttribute = words.at(4); // record target attribute from 5th element in the vector for conditional comparison [FORMAT: keyword target name where attribute condition operand]
                                    tupleConditon = words.at(5); // record tuple condition operator from 6th element in the vector for conditional comparison [FORMAT: keyword target name where attribute condition operand]
                                    
                                    words.at(6).erase(remove(words.at(6).begin(), words.at(6).end(), '\''), words.at(6).end()); // str.erase(): remove all ' \' ' from user-input string for clean parsing
                                    tupleOperand = words.at(6); // record tuple operand from 7th element in the vector for conditional comparison [FORMAT: keyword target name where attribute condition operand]
                                }
                                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }

                                if (error != true) { // if statement: if no error occured (parameter read was successful), check for additional parameters
                                    if (words.size() > 7) { // if statement: if words.size() > 7 -> too many parameters passed (TEMP SOLUTION: limit to a single conditional statement)
                                        cerr << "ERROR: Too many parameters. Please try again! [FORMAT: keyword target name where attribute condition operand]" << endl; // error message (unstable parameters)
                                    }
                                    else { // else statement: delete tuple entry
                                        if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot delete a tuple entry (TABLE REQUIRED) (prompt error message)
                                            cerr << "ERROR: " << nameStructure << " tuple cannot be deleted outside its database." << endl; // error message: cannot delete table tuple without a database
                                        }
                                        else { // else statement: if not inside of root directory (inside database), delete target tuple data
                                            deleteTuple(nameStructure, targetAttribute, tupleConditon, tupleOperand); // deleteTuple: delete tuple data based on 'where' conditonal comparison
                                        }
                                    }
                                }
                                else { // else statement: if error occured (parameter read not successful), prompt error message (do not delete tuple data)
                                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword target name where attribute condition operand]" << endl; // error message (unstable parameters)
                                }
                            }
                            else { // else statement: if tupleKey string not recognized, prompt error message (do not delete tuple data)
                                cerr << "ERROR: Conditional Keyword \"" << tupleKey << "\" not recognized! Tuple cannot be deleted. Please try again!" << endl; // error message: user value parameter is not recognized (!WHERE)
                            }
                        }
                        else { // else statement: if error occured (parameter read not successful), prompt error message (cannot delete tuple data)
                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword target name where attribute condition operand]" << endl; // error message (unstable parameters)
                        }
                    }
                    else { // else statement: if indication string not recognized, prompt error message (cannot delete target tuple data)
                        cerr << "ERROR: Indication \"" << indication << "\" not recognized. Please try again!" << endl; // error message: user indication parameter is not recognized (!FROM)
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (cannot delete tuple data)
                        cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword target name where attribute condition operand]" << endl; // error message (unstable parameters)
                }               
            }
            else if (keyword == "UPDATE") { // else if statement: if the keyword is update, update target tuple data that satisfies given condition for data modification
                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                    nameStructure = words.at(1); // record table name from 2nd element in the vector [FORMAT: keyword name set attribute value where attribute condition operand]
                    
                    modifyKey = words.at(2); // record tuple keyword from 3rd element in the vector [FORMAT: keyword name set attribute value where attribute condition operand]
                    transform(modifyKey.begin(), modifyKey.end(), modifyKey.begin(), ::toupper); // transform(..., ::toupper): var. modifyKey is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                }
                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                    error = true; // bool: ERROR = TRUE -> failsafe
                }

                if (error != true) { // if statement: if no error occured (parameter read was successful), fetch modification parameters and conditonal keyword
                    if (modifyKey == "SET") { // if statement: if modifyKey == "SET", attempt to fetch modification data for an existing table using user parameters                        
                        try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                            modifyAttribute = words.at(3); // record modification attribute from 4th element in the vector [FORMAT: keyword name set attribute value where attribute condition operand]
                            modifyOperator = words.at(4); // record modification operator from 5th element in the vector [FORMAT: keyword name set attribute value where attribute condition operand]
                            
                            words.at(5).erase(remove(words.at(5).begin(), words.at(5).end(), '\''), words.at(5).end()); // str.erase(): remove all ' \' ' from user-input string for clean parsing
                            modifyValue = words.at(5); // record modification value from 6th element in the vector [FORMAT: keyword name set attribute value where attribute condition operand]

                            tupleKey = words.at(6); // record tuple keyword from 7th element in the vector [FORMAT: keyword name set attribute value where attribute condition operand]
                            transform(tupleKey.begin(), tupleKey.end(), tupleKey.begin(), ::toupper); // transform(..., ::toupper): var. tupleKey is case sensitive; therefore, transform() makes entire string UPPERCASE to ignore case-sensitive issues
                        }
                        catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                            error = true; // bool: ERROR = TRUE -> failsafe
                        }

                        if (error != true) { // if statement: if no error occured (parameter read was successful), fetch conditional statement
                            if (tupleKey == "WHERE") { // if statement: if tupleKey == "WHERE", combine remianing parameters into a conditonal statement for tuple deletion
                                try { // try statement: attempt to read parameter data; if each parameter entries are insufficient, throw error
                                    targetAttribute = words.at(7); // record target attribute from 7th element in the vector for conditional comparison [FORMAT: keyword name set attribute value where attribute condition operand]
                                    tupleConditon = words.at(8); // record tuple condition operator from 8th element in the vector for conditional comparison [FORMAT: keyword name set attribute value where attribute condition operand]
                                    
                                    words.at(9).erase(remove(words.at(9).begin(), words.at(9).end(), '\''), words.at(9).end()); // str.erase(): remove all ' \' ' from user-input string for clean parsing
                                    tupleOperand = words.at(9); // record tuple operand from 9th element in the vector for conditional comparison [FORMAT: keyword name set attribute value where attribute condition operand]
                                }
                                catch (const out_of_range& vec) { // catch statement: if each parameter entry was insufficient, catch vector "out of range" error -> ERROR = TRUE -> failsafe
                                    error = true; // bool: ERROR = TRUE -> failsafe
                                }

                                if (error != true) { // if statement: if no error occured (parameter read was successful), check for additional parameters
                                    if (words.size() > 10) { // if statement: if words.size() > 6 -> too many parameters passed (TEMP SOLUTION: limit to a single conditional statement)
                                        cerr << "ERROR: Too many parameters. Please try again! [FORMAT: keyword name set attribute value where attribute condition operand]" << endl; // error message (unstable parameters)
                                    }
                                    else { // else statement: update tuple entry
                                        if (!strcmp(databaseDirectory, getcwd(cwdHome, sizeof(cwdHome)))) { // if statement: if inside of root directory (a.k.a. not inside a database directory), user cannot update a tuple entry (TABLE REQUIRED) (prompt error message)
                                            cerr << "ERROR: " << nameStructure << " tuple cannot be modified outside its database." << endl; // error message: cannot modify table tuple without a database 
                                        }
                                        else { // else statement: if not inside of root directory (inside database), update target tuple data
                                            modifyTuple(nameStructure, modifyAttribute, modifyOperator, modifyValue, targetAttribute, tupleConditon, tupleOperand); // modifyTuple(): modify tuple data based on 'where' conditonal comparison
                                        }
                                    }
                                }
                                else { // else statement: if error occured (parameter read not successful), prompt error message (do not update tuple data)
                                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword name set attribute value where attribute condition operand]" << endl; // error message (unstable parameters)
                                }
                            }
                            else { // else statement: if tupleKey string not recognized, prompt error message (do not update tuple data)
                                cerr << "ERROR: Conditional Keyword \"" << tupleKey << "\" not recognized! Tuple cannot be deleted. Please try again!" << endl; // error message: user value parameter is not recognized (!WHERE)
                            }
                        }
                        else { // else statement: if error occured (parameter read not successful), prompt error message (do not update tuple data)
                            cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword name set attribute value where attribute condition operand]" << endl; // error message (unstable parameters)
                        }
                    }
                    else { // else statement: if modifyKey string not recognized, prompt error message (do not update tuple data)
                        cerr << "ERROR: Modification Keyword \"" << modifyKey << "\" not recognized! Tuple cannot be modified. Please try again!" << endl; // error message: user value parameter is not recognized (!SET)
                    }
                }
                else { // else statement: if error occured (parameter read not successful), prompt error message (do not update tuple data)
                    cerr << "ERROR: Not enough parameters. Please try again! [FORMAT: keyword name set attribute value where attribute condition operand]" << endl; // error message (unstable parameters)
                }
            }
            else { // else statement: if keyword is not recognized, then output an error code to the user
                if (keyword != ".EXIT") { // if statement: if keyword is not recognized && if keyword is not ".EXIT (end program)", then output error code to user.
                    cerr << "ERROR: Command not recognized! Please try again!" << endl; // error message: user command does not match available database commands
                }
            }
            
            // RESET VARIABLES BEFORE NEW READ //

            error = false; // reset boolean value for new error retrieval (fail-safe)
            joinInner = false; // reset boolean value for new inner join statement retrieval (fail-safe)
            joinOuter = false; // reset boolean value for new outer join statement retrieval (fail-safe)
            outerDirection = false; // reset boolean value for new outer join statement retrieval (fail-safe)

            command.clear(); // reset variable value for new command retrieval 
            temp.clear(); // reset variable value for new parsing retrieval
            outerType.clear(); // reset variable instance for new outer join parsing retrieval
            words.clear(); // reset vector instance for new parsing retrieval
            joinCheck.clear(); // reset variable value for new query parsing retrieval
            joinType.clear(); // reset vector instance for new join parsing retrieval
            queryFiles.clear(); // reset vector instance for new query join file/table retrieval
            queryAcronyms.clear(); // reset vector instance for new query join acronym retrieval
            queryParse.clear(); // reset vector instance for new query join parsing retrieval
            query.clear(); // reset vector instance for new parsing retrieval
            attributeName.clear(); // reset vector instance for new parsing retrieval
            attributeType.clear(); // reset vector instance for new parsing retrieval
            attributeValue.clear(); // reset vector instance for new parsing retrieval
            ss.str(string()); // reset stream for new parsing retrieval
        }
    }

    cout << "All done." << endl; // cout confirmation (end of program)

    return 0; // end program
}

// Function Prototype Definitions //

void createDatabase(string initialPath, string dataBase) { // createDatabase(): create new database/directory
    int check; // placeholder variable: used to determine if execution was sucessful (make-shift boolean)

    char cwdCurr[PATH_MAX]; // buffer variable: used for getcwd() to reflect current directory

    // convert parameter string data to const char * for comparisons (i.e., strcmp(), chdir(), getcwd(), etc.)

    const char * homeDirectory = initialPath.c_str();
    const char * directoryName = dataBase.c_str();

    if (strcmp(homeDirectory, getcwd(cwdCurr, sizeof(cwdCurr)))) { // if statement: if inside a database (a non-root directory), change directory back to the root to create a new database (cannot have database within a database)
        chdir(homeDirectory); // change back to root/home directory
    }

    check = mkdir(directoryName, 0777); // create specified database (directory), and return value (if -1, then directory could not be created)

    if (check == -1) { // if statement: if directory cannot be created
       cerr << "!Failed to create database " << dataBase << " because it already exists." << endl; // error message: directory already exists
    }
    else { // else statement: if directory created successfully
        cout << "Database " << dataBase << " created." << endl; // cout confirmation (success)
    }
}

void deleteDatabase(string initialPath, string dataBase) { // deleteDatabase(): delete existing database/directory
    int check; // placeholder variable: used to determine if execution was sucessful (make-shift boolean)

    char cwdCurr[PATH_MAX]; // buffer variable: used for getcwd() to reflect current directory

    string systemCall = "rm -r " + dataBase + " 2> /dev/null"; // variable: used to store systemCall command (remove directory/database recursively) - added error message redirect to NULL space for proper output (no system() error message)

    // convert parameter string data to const char * for comparisons (i.e., strcmp(), chdir(), getcwd(), etc.)

    const char * homeDirectory = initialPath.c_str();
    const char * execute = systemCall.c_str();

    if (strcmp(homeDirectory, getcwd(cwdCurr, sizeof(cwdCurr)))) { // if statement: if inside a database (a non-root directory), change directory back to the root to remove an existing database (cannot delete a database within a database)
        chdir(homeDirectory); // change back to root/home directory
    }

    check = system(execute); // remove specified database (directory), and return value (if check != 0, then directory could not be removed)

    if (check != 0) { // if statement: if check != 0 -> database (directory) cannot be removed (e.g., does not exist) 
        cerr << "!Failed to delete " << dataBase << " because it does not exist." << endl; // error message: directory does not exist
    }
    else { // else statement: if check == 0 -> database (directory) removed successfully
        cout << "Database " << dataBase << " deleted." << endl; // cout confirmation (success)
    }
}

void useDatabase(string initialPath, string dataBase) { // useDatabase(): access existing database/directory
    int check; // placeholder variable: used to determine if execution was sucessful (make-shift boolean)

    char cwdCurr[PATH_MAX]; // buffer variable: used for getcwd() to reflect current directory

    // convert parameter string data to const char * for comparisons (i.e., strcmp(), chdir(), getcwd(), etc.)

    const char * homeDirectory = initialPath.c_str();
    const char * directoryName = dataBase.c_str();

    if (strcmp(homeDirectory, getcwd(cwdCurr, sizeof(cwdCurr)))) { // if statement: if inside a database (a non-root directory), change directory back to the root to access a new database (switch between databases)
        chdir(homeDirectory); // change back to root/home directory
    }

    check = chdir(directoryName); // change to specified database (directory), and return value (if -1, then directory could not be created)

    if (check == -1) { // if statement: if directory cannot be created
        cerr << "!Failed to access database " << dataBase << " because it does not exist." << endl; // error message: directory already exists
    }
    else { // else statement: if directory created successfully
        cout << "Using database " << dataBase << "." << endl; // cout confirmation (success)

        // IGNORE FOLLOWING CODE: Used for testing to see if database was accessed properly //
        // cout << "Current Path: " << getcwd(cwdCurr, sizeof(cwdCurr)) << endl; // cout confirmation: current path/database
    }
}

void createTable(string tableName, vector<string> attributesName, vector<string> attributesType) { // createTable(): create a new table/file
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for creation algorithm

    fileName[0] = toupper(fileName[0]); // toupper(): capitalize first letter fileName string to prevent case sensitivity issues
    
    for (size_t i = 1; i < fileName.length(); i++) { // for loop: for each letter in fileName string (except the first), tolower() to prevent case-sensitivity issues
        fileName[i] = tolower(fileName[i]);
    }

    ofstream file; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only

    if (!file) { // if statement: if file/table does not exist, create a new table
        file.close(); // file.close(): close file from read-only

        file.open(fileName, ios::out); // file.open(): attempt to open target file in write-only

        for (size_t i = 0; i < attributesName.size(); i ++) { // for loop: create a table using attributes (seperate section for each attribute)
            if (i == attributesName.size() - 1) { // if statement: if there is only 1 table attribute, export only 1 attribute section
                file << attributesName[i] << " " << attributesType[i]; // export table attribute (single)
            }
            else { // else statement: else if there are multiple table attributes, export all attributes seperated by '|'
                file << attributesName[i] << " " << attributesType[i] << setw(5) << "|" << setw(6); // export table attributes (multiple)
            }
        }

        cout << "Table " << tableName << " created." << endl; // cout confirmation (success)

        file.close(); // file.close(): close file from write-only
    }
    else { // else statement: else if file/table does exist, do not create a new table (prompt error message)
        cerr << "!Failed to create table " << tableName << " because it already exists." << endl; // error message (file already exists)
    }
}

void deleteTable(string tableName) { // deleteTable(): delete existing table/file
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for deletion algorithm

    fileName[0] = toupper(fileName[0]); // toupper(): capitalize first letter fileName string to prevent case sensitivity issues
    
    for (size_t i = 1; i < fileName.length(); i++) { // for loop: for each letter in fileName string (except the first), tolower() to prevent case-sensitivity issues
        fileName[i] = tolower(fileName[i]);
    }

    const char * targetFile = fileName.c_str(); // convert parameter string data to const char*; needed var. type for remove()

    ifstream file; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only

    if (!file) { // if statement: if file/table does not exist, do not delete an existing table (prompt error message)
        cerr << "!Failed to delete "<< tableName << " because it does not exist." << endl; // error message (file already exists)
    }
    else { // else statement: else if file/table does exist, delete existing table
        remove(targetFile); // remove(): deletes file 

        cout << "Table " << tableName << " deleted." << endl; // cout confirmation (success)
    }
}

void queryTable(vector<string> query, string tableName, string attributeSearch, string condition, string targetOperand) { // queryTable(): fetch information from an existing table/file
    size_t count = 0; // variable: used to store while-loop iterations (current record/sample (num. of valid queries in each))
    int comparisonFound = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label); used for 'where' comparison
    
    bool all = false; // boolean variable: used to store query instruction state -> Does the command use a * to fetch all relevant tuple data?
    bool comparison = false; // boolean variable: used to store conditional state -> Does the command use a conditional statement to filter out information?
    bool error = false; // boolean variable: fail-safe helper (record error status)

    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for creation algorithm

    fileName[0] = toupper(fileName[0]); // toupper(): capitalize first letter fileName string to prevent case sensitivity issues
    
    for (size_t i = 1; i < fileName.length(); i++) { // for loop: for each letter in fileName string (except the first), tolower() to prevent case-sensitivity issues
        fileName[i] = tolower(fileName[i]);
    }

    string line; // variable: used to store current state of getline() from file data
    string currLine; // variable: used to store current state of getline() from file data
    string attributeLine; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType

    vector<size_t> attributeFound; // vector instance: used to store relevant indexes from attributeList (list of attributes)
    vector<int> targetTuple; // vector instance: used to store designated tuple indexes (integers) for fetching (which lines are to be read)

    vector<string> wordList; // vector instance: used to store parsed string to identify attributeName and attributeType
    vector<string> attributeList; // vector instance: used to store parsed string to identify attributeName
    vector<string> attributeType; // vector instance: used to stored parsed string to identify attributeType 
    vector<string> attributeValue; // vector instance: used to store attribute values for each record/sample for fetching
    vector<string> comparisonValue; // vector instace: used to store attribute valuess for each record/sample for comparison

    ifstream file; // fstream initialization
    fstream fileCopy; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only

    if (file) { // if statement: if file/table exists, fetch query
        if (!attributeSearch.empty() && !condition.empty() && !targetOperand.empty()) { // if statement: if conditional variables (e.g., attributeSearch, condition, and targetOperand) are not empty, conduct conditional comparison to filter data (boolean)
            comparison = true; // boolean: comparison = true (use conditional statements)
        }

        fileCopy.open("copyTemp.txt", ios::out); // fileCopy.open(): create a temp target file that will store updated table data (post-filter)

        attributeLine.clear(); // reset variable value for new parsing retrieval
        getline(file, attributeLine); // read line from file input and store value into attributeLine
        attributeLine.erase(remove(attributeLine.begin(), attributeLine.end(), '|'), attributeLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
        
        stringstream is(attributeLine); // stringstream class is: string command treated as stream (parsing -> extraction)
        string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)
    
        while (getline(is, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
            if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
            }
        }

        for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attribute metadata: attributeName/attributeType (every other element -> i += 2)
            attributeList.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
            attributeType.push_back(wordList.at(i + 1)); // push_back(): push attributeType value into vector (list of attributeTypes)
        }

        for (size_t i = 0; i < query.size(); i++) { // for loop: for each query target, check to see if instruction = * (all data)
            if (query.at(i).compare("*") == 0) { // if statement: query.element == "*" (all info) -> print all relevant table information to the user
                all = true; // all -> true
            }
        }

        if (all == true) { // if statement: all == true : instruction == * ; therefore, query all attributes
            query.clear(); // reset vector instance for new parsing retrieval

            for (size_t i = 0; i < attributeList.size(); i++) { // for loop: for each available attribute, push attribute label into query (list of relevant attributes)
                query.push_back(attributeList.at(i)); // push_back(): push current string value into vector (table attribute)
            }
        }

        for (size_t i = 0; i < query.size(); i++) { // for loop: for each given query option, determine attribute validity (is the query attribute in the table?)
            for (size_t j = 0; j < attributeList.size(); j++) { // for loop: for each attribute in the table, conduct a comparison to see if given attributes exists in the table (record pos. of attributes to identify attribute values in each record/sample)
                if (query.at(i) == attributeList.at(j)) { // if statement: if target attribute found in list of attributes
                    attributeFound.push_back(j); // record pos. of target attribute (which dimension corresponds with target attribute)

                    // cout << attributeList.at(j) << endl; // PRINT TESTING
                }
            }
        }

        if (attributeFound.size() != query.size()) { // if statement: if one or more target attribute do/does not exist in the table, prompt error message 
            if (query.size() == 1) { // if statement: if query contains a single target attribute (print invalid error (single))
                cerr << "ERROR: Invalid Attribute Selection. Please try again!" << endl; // cout confirmation: target attribute(s) do/does not exist in the table (error message)

                attributeFound.clear(); // reset vector instance (error: do not fetch tuple data)
            }
            else if (query.size() > 1) { // else if statement: if query contains multiple target attributes (print invalid error (multiple))
                cerr << "ERROR: One or More Invalid Attribute Selection(s). Please try again!" << endl; // cout confirmation: target attribute(s) do/does not exist in the table (error message)

                attributeFound.clear(); // reset vector instance (error: do not fetch tuple data)
            }
        }

        if (comparison == true) { // if statement: if query is using conditional statements, check to see if comparison attribute exists
            for (size_t i = 0; i < attributeList.size(); i++) { // for loop: for each attribute in the table, conduct a comparison to see if comparison attribute exists in the table (record pos. of attribute to identify attribute values in each record/sample)
                if (attributeList.at(i) == attributeSearch) { // if statement: if target attribute found in list of attributes
                    comparisonFound = i; // record pos. of target attribute (which dimension corresponds with target attribute)
                }
            }

            if (comparisonFound == -1) { // if statement: if target attribute does not exist in the table, prompt error message 
                cerr << "ERROR: Invalid Attribute Operand. " << tableName << " does not include: " << attributeSearch << "." << endl; // error message: target attribute does not exist in the table
            }
        }

        while (!file.eof()) { // while loop: while getline() has not reach the end of the file (EOF), parse each record's/sample's attribute data for filtered comparison
            is.str(string()); // reset stream for new parsing retrieval
            attributeLine.clear(); // reset variable value for new parsing retrieval
            wordList.clear(); // reset vector instance for new parsing retrieval

            getline(file, attributeLine); // read line from file input and store value into attributeLine

            attributeLine.erase(remove(attributeLine.begin(), attributeLine.end(), '|'), attributeLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
        
            stringstream fs(attributeLine); // stringstream class fs: string command treated as stream (parsing -> extraction)
            string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

            while (getline(fs, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                    wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
                }
            }

            for (size_t i = 0; i < attributeFound.size(); i++) { // for loop: for each valid query attribute, fetch corresponding attribute value(s)
                for (size_t j = 0; j < wordList.size(); j++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute's value(s) of the given record/sample 
                    if (attributeFound.at(i) == j) { // if statement: if the current index matches the target attribute's index, push record's/sample's attribute data into a list of attribute data. 
                        attributeValue.push_back(wordList.at(j)); // push_back(): push current string value into vector (list of attribute's data)

                        // cout << wordList.at(j) << endl; // PRINT TESTING
                    }
                }
            }

            if (comparison == true) { // if statement: if query is using conditional statements, fetch comparison attribute values for all records/labels
                int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)
                
                for (int i = 0; i < listSize; i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute value of the given record/sample 
                    if (i == comparisonFound) { // if statement: if the current index matches the target attribute index, push record's/sample's attribute data into a list of comparison attribute data. 
                        comparisonValue.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of comparison attribute's data)
                    }
                }               
            }
        }

        if (comparison == true) { // if statement: if query is using conditional statements, conduct comparison checks (filter relevant tuples)
            for (size_t i = 0; i < comparisonValue.size(); i++) { // for loop: for each record's/sample's attribute data, conduct 'where' comparison. If attribute value passes comparison, record its corresponding row for selection (filter).
                if (isNumber(targetOperand)) { // if statement: if the target operand (value its being comapred to) is a number [isNumber() helper function], conduct numerical arithmetic (e.g., >, <, >=, <=, and =)
                    float operand_1 = 0; // variable: left-side operand (float -> scenarios that involve decimal values (i.e., pricing, income, etc.))
                    float operand_2 = 0; // variable: right-side operand (float -> scenarios that involve decimal values ...)
                    
                    stringstream op1(comparisonValue.at(i)); // stringstream class op1: string command treated as stream (extraction) -> string to float conversion
                    stringstream op2(targetOperand); // stringstream class op2: string command treated as stream (extraction) -> string to float conversion

                    op1 >> operand_1; // string to float conversion (left-side)
                    op2 >> operand_2; // string to float conversion (right-side)

                    if (condition == ">") { // if statement: if the condition operation == >, conduct a greater than comparison
                        if (operand_1 > operand_2) { // if statement: if operand_1 > operand_2
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }
                    }
                    else if (condition == "<") {  // else if statement: if the condition operation == <, conduct a less than comparison
                        if (operand_1 < operand_2) { // if statement: if operand_1 < operand_2
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }
                    }
                    else if (condition == "<=") { // else if statement: if the condition operation == <=, conduct a less than or equal to comparison
                        if (operand_1 <= operand_2) { // if statement: if operand_1 <= operand_2
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }
                    }
                    else if (condition == ">=") { // else if statement: if the condition operation == <=, conduct a greater than or equal to comparison
                        if (operand_1 >= operand_2) { // if statement: if operand_1 >= operand_2
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }
                    }
                    else if (condition == "=") { // else if statement: if the condition operation == =, conduct an equal comparison
                        if (operand_1 == operand_2) { // if statement: if operand_1 == operand_2
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }                    
                    }
                    else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a not equal comparison
                        if (operand_1 != operand_2) { // if statement: if operand_1 != operand_2
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }
                    }
                    else { // else statement: if the condition operation is invalid (none of the aforementioned cases), prompt error message
                        cerr << "ERROR: Invalid Numerical Condition. Please try one of the following: >, <, >=, <=, =" << endl; // error message: Invalid Numerical Condition (not a comparison between numbers)
                        break;
                    }   

                    op1.str(string()); // reset stream for new parsing retrieval [operand_1 is the only operand changing with each comparison (different record/sample data)]
                }
                else { // else statement: if targetOperand is not a number, then targetOperand is a string/object and can not support Numerical Comparions (only ==)
                    if ((condition == "=") || (condition == "==")) { // if statement: if the condition operation == = || ==, conduct a string comparison [are the strings the same (i.e., filter by name, tile, etc.)]
                        if (comparisonValue.at(i) == targetOperand) { // if statement: if the record's/sample's current string value matches the value of the target operand, fetch record/sample data 
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }
                    }
                    else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a string NOT comparison 
                        if (comparisonValue.at(i) != targetOperand)  { // if statement: if the record's/sample's current string value does not match the value of the target operand, fetch record/sample data
                            targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                        }
                    }
                    else { // else statement: if the condition is not a string comparison, prompt error message 
                        cerr << "ERROR: Invalid String Condition. Please try one of the following: =, ==" << endl; // error message: Invalid String Condition (not a comparison between strings)
                        break;
                    }    
                }
            }

            if (targetTuple.empty()) { // if statement: if targetTuple (list of tuples to query) empty, prompt an error message
                cerr << "ERROR: Invalid Query. Cannnot find tuples that pass the following condition: " << attributeSearch << " " << condition << " " << targetOperand << endl; // error message (failed conditional statement)
                error = true; // boolean: error = true -> failsafe
            }
        }

        if (error != true) { // if statement: if no error occured (successful filtered query), fetch relevant tuple data by creating/reading a temp copy
            int listSize = attributeFound.size(); // variable: size_t (long unsigned int) -> int (int)
            
            for (int i = 0; i < listSize; i++) { // for loop: create a table using valid query attributes (seperate section for each attribute)
                if (i == listSize - 1) { // if statement: if there is only 1 table attribute, export only 1 attribute section
                    fileCopy << attributeList[attributeFound.at(i)] << " " << attributeType[attributeFound.at(i)]; // export table attribute (single)
                }
                else { // else statement: else if there are multiple table attributes, export all attributes seperated by '|'
                    fileCopy << attributeList[attributeFound.at(i)] << " " << attributeType[attributeFound.at(i)] << setw(5) << "|" << setw(6); // export table attributes (multiple)
                }
            }

            int lineIndex = 1; // variable: used to store the current line index of getline() -> tracks the # of '\n' (new line) characters [row(0) -> column header(s); therefore start at row(1) -> attribute data]
            bool match = true; // boolean variable: used to store the boolean state of index comparison with targetTuple -> Is currentLine exported/read? : default = true (no conditions)

            for (size_t i = 0; i < attributeValue.size(); i += count) { // for loop: for each record's/sample's (valid) attribute value, export relevant table data to a temp target file for proper getline() reading [attributeValue format: queryNum -> 3 attr. : attributeValue[0] = attr.1, attributeValue[1] = attr.2, attributeValue[2] = attr. 3, attributeValue[4] = attr. 1, attributeValue[5] = attr.2, etc.]
                count = 0; // reset count variable to keep track of num. of attributes in each record/sample
                
                if (comparison == true) { // if statement: if query is using conditional statements, conduct index checks (filter relevant tuples)
                    match = false; // boolean: match = false -> default (conditions)

                    for (size_t j = 0; j < targetTuple.size(); j++) { // for loop: for each recorded tuple index (pending selection), compare with current lineIndex to identify if data is exported to target file (updated table)                                    
                        if (lineIndex == targetTuple.at(j)) { // if statement: if current lineIndex == any(targetTuple.list)
                            match = true; // change boolean status (true) -> export record's/sample's attribute data for reading
                        }
                    }
                }

                if (match == true) { // if statement: if match == true -> if current tuple data is relevant, export to temp copy
                    fileCopy << endl; // export '\n' (new line) character : seperation between rows of data

                    while (count < attributeFound.size()) { // while loop: while count < attributeFound.size() : [index of record/sample attr.] < [(valid) max num. of record/sample attr.] -> output relevent record/sample data row by row
                        if(attributeValue.at(i + count).empty()) { // if statement: if attribute.Value is empty(), initialize entry to NULL
                            attributeValue.at(i + count).assign("NULL"); // empty attributeValue = NULL
                        }

                        if (count == attributeFound.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                            fileCopy << attributeValue.at(i + count); // export tuple data (single)
                        }
                        else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                            fileCopy << attributeValue.at(i + count) << setw(10) << "|" << setw(12); // export tuple data (multiple) 
                        }

                        count++; // count increment (next attribute in the record/sample)
                    }
                }
                else { // else statement: else if match == false -> if current tuple data is not relevant, omit from temp copy
                    while (count < attributeFound.size()) { // while loop: while count < attributeFound.size() : [index of record/sample attr.] < [(valid) max num. of record/sample attr.] -> increment attr. counter to skip over relevent record/sample data
                        count++; // count increment (next attribute in the record/sample)
                    }
                }

                lineIndex++; // lineIndex increment (represents next tuple data index/row)
            }

            fileCopy.close(); // fileCopy.close(): close file from write-only

            fileCopy.open("copyTemp.txt", ios::in); // fileCopy.open(): attempt to open file in read-only (getline() fetch)

            while (getline(fileCopy, line)) { // while loop: while getline() continues to extract file data line-by-line, print to user
                cout << line << endl; // cout table data
            }

            file.close(); // file.close(): close file from read-only
            fileCopy.close(); // fileCopy.close(): close file from read-only

            remove("copyTemp.txt"); // after displaying relevant data, remove/delete temp target
        }
    }
    else { // else statement: else if file/table does not exist, do not fetch a query (prompt error message)
        cerr << "!Failed to query table " << tableName << " because it does not exist." << endl; // error message (file does not exist)
    }
}

void updateTable(string alter, string tableName, vector<string> attributesName, vector<string> attributesType) { // updateTable(): modify an existing table/file
    int lineIndex = 0; // variable: used to store the current line index of getline() -> tracks the # of '\n' (new line) characters
    
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for creation algorithm
    string currLine; // variable: used to store current state of getline()

    fileName[0] = toupper(fileName[0]); // toupper(): capitalize first letter fileName string to prevent case sensitivity issues
    
    for (size_t i = 1; i < fileName.length(); i++) { // for loop: for each letter in fileName string (except the first), tolower() to prevent case-sensitivity issues
        fileName[i] = tolower(fileName[i]);
    }

    ifstream file; // fstream initialization
    ofstream fileCopy; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only (does it exist?)
    fileCopy.open("copyTemp.txt", ios::out); // file.open(): attempt to open target file in write-only

    if (file) { // if statement: if file/table exists, fetch alter instruction
        if (alter.compare("ADD") == 0) { // if statement: if alter instruction == "ADD" -> append new attributeName/attributeType to file/table
            while (!file.eof()) {  // while loop: while getline() has not reach the end of the file (EOF), acquire each record's/sample's attribute data. If record's/sample's index matches the indexes of those who passed the comparison, ignore and do not output to target file.
                currLine.clear(); // reset variable value for new data retrieval

                getline(file, currLine); // read line from file input and store value into currLine

                if (lineIndex == 0) { // if statement: if current file line index == 0; file read represents list of tuple attributes (columns) -> append new attributes to first line of file
                    fileCopy << currLine; // export current attribute data to target file
                    
                    for (size_t i = 0; i < attributesName.size(); i ++) { // for loop: append requested attributes (seperate section for each additional attribute)
                        fileCopy << setw(5) << "|" << setw(6) << attributesName[i] << " " << attributesType[i]; // export table attributes (multiple)
                    }
                }
                else { // else statement: if current file line index != 0; file read represents list of tuple values -> export remaining data (unchanged -> promote data integrity)
                    fileCopy << endl; // export '\n' (new line) character : seperation between rows of data
                    fileCopy << currLine; // export current attribute data to target file

                    for (size_t i = 0; i < attributesName.size(); i++) { // for loop: insert a table tuple using attribute values (seperate section for each attribute)
                        fileCopy << setw(10) << "|" << setw(12) <<  "NULL"; // export tuple data (multiple)
                    }
                }

                lineIndex++; // lineIndex increment: represents total number of lines in the file (currLine index tracker)
            }

            cout << "Table " << tableName << " modified." << endl; // cout confirmation (success)
        }
        else { // else statement: if alter instruction not recognized, then output an error code to the user
            cerr << "ERROR: Unrecognized alter instruction! Please try again!" << endl; // error message (alter does not exist)
        }

        file.close(); // file.close(): close file
        fileCopy.close(); // fileCopy.close(): close file

        const char * oldFile = fileName.c_str(); // string to const char * conversion [remove(const char *)]

        remove(oldFile); // remove/delete original table structure 
        rename("copyTemp.txt", oldFile); // rename updated table structure to reflect the same metadata as the original (same title)
    }
    else { // else statement: else if file/table does not exist, do not alter an existing table (prompt error message)
        cerr << "!Failed to modify " << tableName << " because it does not exist." << endl; // error message (file does not exist)
    }
}

void insertTuple(string tableName, vector<string> attributesValue) { // insertTuple(): insert tuple data
    string fileName = tableName + (".txt"); // variable: used to record full name of file/table for tuple insertion algorithm
    string attributeLine; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType

    fileName[0] = toupper(fileName[0]); // toupper(): capitalize first letter fileName string to prevent case sensitivity issues
    
    for (size_t i = 1; i < fileName.length(); i++) { // for loop: for each letter in fileName string (except the first), tolower() to prevent case-sensitivity issues
        fileName[i] = tolower(fileName[i]);
    }

    vector<string> wordList; // vector instance: used to store parsed string to identify attributeName and attributeType
    vector<string> attributeList; // vector instance: used to store parsed string to identify attributeName (ignore of attributeType)

    fstream file; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only (does it exist?)

    if (file) { // if statement: if file/table exists, fetch insert tuple instruction
        int counter = 0; // variable: used to track the number of new records/samples

        attributeLine.clear(); // reset variable value for new parsing retrieval

        getline(file, attributeLine); // read line from file input and store value into attributeLine
        attributeLine.erase(remove(attributeLine.begin(), attributeLine.end(), '|'), attributeLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
        
        stringstream ns(attributeLine); // stringstream class ns: string command treated as stream (parsing -> extraction)
        string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)
    
        while (getline(ns, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
            if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
            }
        }

        for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attributeName (every other element -> i += 2)
            attributeList.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
        }

        size_t numAttributes = attributesValue.size(); // variable: used to store the number of attributes given by user-input 
        size_t maxAttributes = attributeList.size(); // variable: used to store the totol number of attributes in a given table (max num. of parameter entries)

        file.close(); // file.close(): close file from read-only

        if (numAttributes <= maxAttributes) { // if statement: if num. of tuple (input) attributes is less than or equal to the num. of max attributes in the table -> DO NOT EXCEED # OF COLUMNS

            file.open(fileName, ios::app); // file.open(): attempt to open target file in append-only 

            if (!attributesValue.empty() && numAttributes != 0) { // if statement: if the list of attribute values contains some/all data (not empty), create a new row/record, and increment the total number of new records/samples 
                file << endl; // text formatting : '\n' (new line) seperates each row of data 
                counter++; // counter increment : represents total number of new rows of data

                for (size_t i = 0; i < maxAttributes; i++) { // for loop: insert a table tuple using attribute values (seperate section for each attribute)
                    if (i < numAttributes) { // if statement: if the current index is less than the total num. of attributes passed through user-input, export tuple data
                        if (attributesValue.at(i).empty()) { // if statement: if the current element of attributesValue is empty, initialize element to NULL
                            attributesValue.at(i).assign("NULL"); // empty attributesValue = NULL
                        }

                        if (i == maxAttributes - 1) { // if statement: if there is only 1 table attribute, export only 1 attribute value
                            file << attributesValue.at(i); // export tuple data (single)
                        }
                        else { // else statement: else if there are multiple table attributes, export all attribute values seperated by '|'
                            file << attributesValue.at(i) << setw(10) << "|" << setw(12); // export tuple data (multiple)
                        }
                    }
                    else { // else statement: else if the current index is greater than or equal to the total num. of attributes passed through user-input, initialize remaining tuple entries (maxAttributes - numAttributes) to NULL
                        if (i == maxAttributes - 1) { // if statement: if there is only 1 table attribute, export only 1 attribute NULL
                            file << "NULL"; // export tuple data (single)
                        }
                        else { // else statement: else if there are multiple table attributes, export all attribute  seperated by '|'
                            file << "NULL" << setw(10) << "|" << setw(12); // export tuple data (multiple)
                        }
                    }
                }
            }

            if (counter == 1) { // if statement: if counter == 1 (only 1 new record/sample), print cout confirmation (single)
                cout << counter << " new record inserted." << endl; // cout confirmation (success : single)
            }
            else if (counter > 1) { // else if statement: else if counter > 1 (more than 1 new record/sample), print cout confirmation (multiple)
                cout << counter << " new records inserted." << endl; // cout confirmation (success : multiple)
            }
            else { // else statement: if counter <= 0 (no new record/sample), print cout confirmation (nothing)
                cout << "No new records inserted." << endl; // cout confirmation (failure : nothing)
            }
        }
        else { // else statement: else if given tuple data exceeds num. of attribute columns, promppt error message
            cerr << "ERROR: Too Many Attribute Values [max = " << maxAttributes << "]." << endl; // error message (too many tuples!)
        }

        file.close(); // file.close(): close file from append-only
    }
    else { // else statement: else if file/table does not exist, do not insert tuple to a non-existing table (prompt error message)
        cerr << "!Failed to insert tuple into " << tableName << " because it does not exist." << endl; // error message (file does not exist)
    }
}

void deleteTuple(string tableName, string attributeSearch, string condition, string targetOperand) { // deleteTuple(): delete tuple data
    int counter = 0; // variable: stores total number records/samples deleted by conditional statement
    int attributeFound = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label)

    string fileName = tableName + (".txt"); // variable: used to store full name of file/table for tuple insertion algorithm

    fileName[0] = toupper(fileName[0]); // toupper(): capitalize first letter fileName string to prevent case sensitivity issues
    
    for (size_t i = 1; i < fileName.length(); i++) { // for loop: for each letter in fileName string (except the first), tolower() to prevent case-sensitivity issues
        fileName[i] = tolower(fileName[i]);
    }

    string attributeLine; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType
    string currLine; // variable: used to store current state of getline()
    
    vector<int> targetTuple; // vector instance: used to store designated tuple indexes (integers) for deletion

    vector<string> wordList; // vector instance: used to store parsed string to identify attributeName and attributeType
    vector<string> attributeList; // vector instance: used to store parsed string to identify attributeName (ignore of attributeType)
    vector<string> attributeValue; // vector instance: used to store attribute values for each record/sample

    ifstream file; // fstream initialization
    ofstream fileCopy; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only (does it exist?)
    fileCopy.open("copyTemp.txt", ios::out); // fileCopy.open(): create a temp target file that will store updated table data (post-deletion)

    if (file) { // if statement: if file/table exists, identify filtered attribute, apply comparisons, and remove tuples that satisfy the conditional statement
        attributeLine.clear(); // reset variable value for new parsing retrieval

        getline(file, attributeLine); // read line from file input and store value into attributeLine

        attributeLine.erase(remove(attributeLine.begin(), attributeLine.end(), '|'), attributeLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
        
        stringstream fs(attributeLine); // stringstream class fs: string command treated as stream (parsing -> extraction)
        string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)
    
        while (getline(fs, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
            if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
            }
        }

        for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attributeName (every other element -> i += 2)
            attributeList.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
        }

        for (size_t i = 0; i < attributeList.size(); i++) { // for loop: for each attribute in the table, conduct a comparison to see if comparison attribute exists in the table (record pos. of attribute to identify attribute values in each record/sample)
            if (attributeList.at(i) == attributeSearch) { // if statement: if target attribute found in list of attributes
                attributeFound = i; // record pos. of target attribute (which dimension corresponds with target attribute)
            }
        }

        if (attributeFound == -1) { // if statement: if target attribute does not exist in the table, prompt error message 
            cerr << "ERROR: Invalid Attribute Operand. " << tableName << " does not include: " << attributeSearch << "." << endl; // error message: target attribute does not exist in the table
        }

        while (!file.eof()) { // while loop: while getline() has not reach the end of the file (EOF), parse each record's/sample's attribute data for filtered comparison
            fs.str(string()); // reset stream for new parsing retrieval
            attributeLine.clear(); // reset variable value for new parsing retrieval
            wordList.clear(); // reset vector instance for new parsing retrieval

            getline(file, attributeLine); // read line from file input and store value into attributeLine

            attributeLine.erase(remove(attributeLine.begin(), attributeLine.end(), '|'), attributeLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
           
            stringstream fs(attributeLine); // stringstream class fs: string command treated as stream (parsing -> extraction)
            string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

            while (getline(fs, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                    wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
                }
            }

            int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)

            for (int i = 0; i < listSize; i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute value of the given record/sample 
                if (i == attributeFound) { // if statement: if the current index matches the target attribute index, push record's/sample's attribute data into a list of attribute data. 
                    attributeValue.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of attribute's data)
                }
            }
        }

        for (size_t i = 0; i < attributeValue.size(); i++) { // for loop: for each record's/sample's attribute data, conduct 'where' comparison. If attribute value passes comparison, record its corresponding row for deletion (filter).
            if (isNumber(targetOperand)) { // if statement: if the target operand (value its being comapred to) is a number [isNumber() helper function], conduct numerical arithmetic (e.g., >, <, >=, <=, and =)
                float operand_1 = 0; // variable: left-side operand (float -> scenarios that involve decimal values (i.e., pricing, income, etc.))
                float operand_2 = 0; // variable: right-side operand (float -> scenarios that involve decimal values ...)
                
                stringstream op1(attributeValue.at(i)); // stringstream class op1: string command treated as stream (extraction) -> string to float conversion
                stringstream op2(targetOperand); // stringstream class op2: string command treated as stream (extraction) -> string to float conversion

                op1 >> operand_1; // string to float conversion (left-side)
                op2 >> operand_2; // string to float conversion (right-side)

                if (condition == ">") { // if statement: if the condition operation == >, conduct a greater than comparison
                    if (operand_1 > operand_2) { // if statement: if operand_1 > operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "<") {  // else if statement: if the condition operation == <, conduct a less than comparison
                    if (operand_1 < operand_2) { // if statement: if operand_1 < operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "<=") { // else if statement: if the condition operation == <=, conduct a less than or equal to comparison
                    if (operand_1 <= operand_2) { // if statement: if operand_1 <= operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == ">=") { // else if statement: if the condition operation == <=, conduct a greater than or equal to comparison
                    if (operand_1 >= operand_2) { // if statement: if operand_1 >= operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "=") { // else if statement: if the condition operation == =, conduct an equal comparison
                    if (operand_1 == operand_2) { // if statement: if operand_1 == operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }                    
                }
                else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a NOT comparison 
                    if (operand_1 != operand_2) { // if statement: if operand_1 != operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else { // else statement: if the condition operation is invalid (none of the aforementioned cases), prompt error message
                    cerr << "ERROR: Invalid Numerical Condition. Please try one of the following: >, <, >=, <=, =" << endl; // error message: Invalid Numerical Condition (not a comparison between numbers)
                    break;
                }   

                op1.str(string()); // reset stream for new parsing retrieval [operand_1 is the only operand changing with each comparison (different record/sample data)]
            }
            else { // else statement: if targetOperand is not a number, then targetOperand is a string/object and can not support Numerical Comparions (only ==)
                if ((condition == "=") || (condition == "==")) { // if statement: if the condition operation == = || ==, conduct a string comparison [are the strings the same (i.e., filter by name, tile, etc.)]
                    if (attributeValue.at(i) == targetOperand) { // if statement: if the record's/sample's current string value matches the value of the target operand, fetch record/sample data 
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a string NOT comparison 
                    if (attributeValue.at(i) != targetOperand) { // if statement: if the record's/sample's current string value does not match the value of the target operand, fetch record/sample data
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else { // else statement: if the condition is not a string comparison, prompt error message 
                    cerr << "ERROR: Invalid String Condition. Please try one of the following: =, ==" << endl; // error message: Invalid String Condition (not a comparison between strings)
                    break;
                }    
            }
        }

        int lineIndex = 1; // variable: used to store the current line index of getline() -> tracks the # of '\n' (new line) characters [row(0) -> column header(s); therefore start at row(1) -> attribute data]
        bool match; // boolean variable: used to store the boolean state of index comparison with targetTuple -> does currLine need to be deleted?

        file.seekg(0, ios::beg); // seekg(0, ios::beg): re-read input file from the beginning (data comparison)

        getline(file, currLine); // read line from file input and store value into attributeLine [row(0) -> column header(s)]
        fileCopy << currLine; // output attributeLine into temp target file (new table state)

        while (!file.eof()) {  // while loop: while getline() has not reach the end of the file (EOF), acquire each record's/sample's attribute data. If record's/sample's index matches the indexes of those who passed the comparison, ignore and do not output to target file.
            match = false; // boolean = false (reset for each iteration)
            currLine.clear(); // reset variable value for new data retrieval

            getline(file, currLine); // read (next) line from file input and store value into attributeLine 
            
            for (size_t i = 0; i < targetTuple.size(); i++) { // for loop: for each recorded tuple index (pending deletion), compare with current lineIndex to identify if data is exported to target file (updated table)
                if (lineIndex == targetTuple.at(i)) { // if statement: if current lineIndex == any(targetTuple.list)
                    match = true; // change boolean status (true) -> omit record's/sample's attribute data
                    counter++; // counter increment : represents total number of deleted rows of data
                }
            }

            if (match != true) { // if statement: if match == false -> if current lineIndex does not need to be deleted, export attribute data into updated table 
                if (!currLine.empty()) { // if statement: if attribute data is not empty, export to target file
                    fileCopy << endl; // export '\n' (new line) character : seperation between rows of data
                    fileCopy << currLine; // export currLine attribute data to target file
                }
            }

            lineIndex++; // lineIndex increment: represents total number of lines in the file (currLine index tracker)
        }

        if (counter == 1) { // if statement: if counter == 1 (only 1 deleted record/sample), print cout confirmation (single)
            cout << counter << " record deleted." << endl; // cout confirmation (success : single)
        }
        else if (counter > 1) { // else if statement: if counter > 1 (multiple deleted records/samples), print cout confirmation (multiple)
            cout << counter << " records deleted." << endl; // cout confirmation (success : multiple)
        }
        else { // else statement: if counter <= 0 (no deleted records/samples), print cout confirmation (none)
            cout << "No records deleted." << endl; // cout confirmation (failure : none)
        }

        file.close(); // file.close(): close file
        fileCopy.close(); // fileCopy.close(): close file

        const char * oldFile = fileName.c_str(); // string to const char * conversion [remove(const char *)]

        remove(oldFile); // remove/delete original table structure 
        rename("copyTemp.txt", oldFile); // rename updated table structure to reflect the same metadata as the original (same title)
    }
    else { // else statement: else if file/table does not exist, do not insert tuple to a non-existing table (prompt error message)
        cerr << "!Failed to delete tuple from " << tableName << " because it does not exist." << endl; // error message (file does not exist)
    }
}

void modifyTuple(string tableName, string changeAttribute, string changeOperator, string changeValue, string attributeSearch, string condition, string targetOperand) { // updateTuple(): update tuple data
    int counter = 0; // variable: stores total number records/samples deleted by conditional statement
    int attributeFound = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label) -> attributeSearch
    int attributeUpdate = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label) -> changeAttribute

    string fileName = tableName + (".txt"); // variable: used to store full name of file/table for tuple insertion algorithm
    
    fileName[0] = toupper(fileName[0]); // toupper(): capitalize first letter fileName string to prevent case sensitivity issues
    
    for (size_t i = 1; i < fileName.length(); i++) { // for loop: for each letter in fileName string (except the first), tolower() to prevent case-sensitivity issues
        fileName[i] = tolower(fileName[i]);
    }

    string attributeLine; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType
    string currLine; // variable: used to store current state of getline()
    
    vector<int> targetTuple; // vector instance: used to store designated tuple indexes (integers) for deletion

    vector<string> wordList; // vector instance: used to store parsed string to identify attributeName and attributeType
    vector<string> attributeList; // vector instance: used to store parsed string to identify attributeName (ignore of attributeType)
    vector<string> attributeValue; // vector instance: used to store attribute values for each record/sample

    ifstream file; // fstream initialization
    ofstream fileCopy; // fstream initialization

    file.open(fileName, ios::in); // file.open(): attempt to open target file in read-only (does it exist?)
    
    if (file) { // if statement: if file/table exists, identify filtered attribute, apply comparisons, and remove tuples that satisfy the conditional statement
        fileCopy.open("copyTemp.txt", ios::out); // fileCopy.open(): create a temp target file that will store updated table data (post-deletion)

        attributeLine.clear(); // reset variable value for new parsing retrieval

        getline(file, attributeLine); // read line from file input and store value into attributeLine

        attributeLine.erase(remove(attributeLine.begin(), attributeLine.end(), '|'), attributeLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
        
        stringstream fs(attributeLine); // stringstream class fs: string command treated as stream (parsing -> extraction)
        string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)
    
        while (getline(fs, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
            if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
            }
        }

        for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attributeName (every other element -> i += 2)
            attributeList.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
        }

        for (size_t i = 0; i < attributeList.size(); i++) { // for loop: for each attribute in the table, conduct a comparison to see if comparison attribute exists in the table (record pos. of attribute to identify attribute values in each record/sample)
            if (attributeList.at(i) == attributeSearch) { // if statement: if target attribute found in list of attributes
                attributeFound = i; // record pos. of target attribute (which dimension corresponds with target attribute)
            }

            if (attributeList.at(i) == changeAttribute) { // if statement: if attribute to be changed found in list of attributes
                attributeUpdate = i; // record pos. of changing attribute (wwhich dimension corresponds with changing attribute)
            }
        }

        if (attributeFound == -1) { // if statement: if target attribute does not exist in the table, prompt error message 
            cerr << "ERROR: Invalid Attribute Operand. " << tableName << " does not include: " << attributeSearch << "." << endl; // error message: target attribute does not exist in the table
        }

        if (attributeUpdate == -1) {
            cerr << "ERROR: Invalid Attribute Modification. " << tableName << " does not include: " << changeAttribute << "." << endl; // error message: target attribute does not exist in the table
        }

        while (!file.eof()) { // while loop: while getline() has not reach the end of the file (EOF), parse each record's/sample's attribute data for filtered comparison
            fs.str(string()); // reset stream for new parsing retrieval
            attributeLine.clear(); // reset variable value for new parsing retrieval
            wordList.clear(); // reset vector instance for new parsing retrieval

            getline(file, attributeLine); // read line from file input and store value into attributeLine

            attributeLine.erase(remove(attributeLine.begin(), attributeLine.end(), '|'), attributeLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
           
            stringstream fs(attributeLine); // stringstream class fs: string command treated as stream (parsing -> extraction)
            string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

            while (getline(fs, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                    wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
                }
            }

            int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)

            for (int i = 0; i < listSize; i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute value of the given record/sample 
                if (i == attributeFound) { // if statement: if the current index matches the target attribute index, push record's/sample's attribute data into a list of attribute data. 
                    attributeValue.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of attribute's data)
                }
            }
        }

        for (size_t i = 0; i < attributeValue.size(); i++) { // for loop: for each record's/sample's attribute data, conduct 'where' comparison. If attribute value passes comparison, record its corresponding row for deletion (filter).
            if (isNumber(targetOperand)) { // if statement: if the target operand (value its being comapred to) is a number [isNumber() helper function], conduct numerical arithmetic (e.g., >, <, >=, <=, and =)
                float operand_1 = 0; // variable: left-side operand (float -> scenarios that involve decimal values (i.e., pricing, income, etc.))
                float operand_2 = 0; // variable: right-side operand (float -> scenarios that involve decimal values ...)
                
                stringstream op1(attributeValue.at(i)); // stringstream class op1: string command treated as stream (extraction) -> string to float conversion
                stringstream op2(targetOperand); // stringstream class op2: string command treated as stream (extraction) -> string to float conversion

                op1 >> operand_1; // string to float conversion (left-side)
                op2 >> operand_2; // string to float conversion (right-side)

                if (condition == ">") { // if statement: if the condition operation == >, conduct a greater than comparison
                    if (operand_1 > operand_2) { // if statement: if operand_1 > operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "<") {  // else if statement: if the condition operation == <, conduct a less than comparison
                    if (operand_1 < operand_2) { // if statement: if operand_1 < operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "<=") { // else if statement: if the condition operation == <=, conduct a less than or equal to comparison
                    if (operand_1 <= operand_2) { // if statement: if operand_1 <= operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == ">=") { // else if statement: if the condition operation == <=, conduct a greater than or equal to comparison
                    if (operand_1 >= operand_2) { // if statement: if operand_1 >= operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "=") { // else if statement: if the condition operation == =, conduct an equal comparison
                    if (operand_1 == operand_2) { // if statement: if operand_1 == operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }                    
                }
                else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a NOT comparison 
                    if (operand_1 != operand_2) { // if statement: if operand_1 != operand_2
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }                
                else { // else statement: if the condition operation is invalid (none of the aforementioned cases), prompt error message
                    cerr << "ERROR: Invalid Numerical Condition. Please try one of the following: >, <, >=, <=, =" << endl; // error message: Invalid Numerical Condition (not a comparison between numbers)
                    break;
                }   

                op1.str(string()); // reset stream for new parsing retrieval [operand_1 is the only operand changing with each comparison (different record/sample data)]
            }
            else { // else statement: if targetOperand is not a number, then targetOperand is a string/object and can not support Numerical Comparions (only ==)
                if ((condition == "=") || (condition == "==")) { // if statement: if the condition operation == = || ==, conduct a string comparison [are the strings the same (i.e., filter by name, tile, etc.)]
                    if (attributeValue.at(i) == targetOperand) { // if statement: if the record's/sample's current string value matches the value of the target operand, fetch record/sample data 
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }
                else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a string NOT comparison 
                    if (attributeValue.at(i) != targetOperand) { // if statement: if the record's/sample's current string value does not match the value of the target operand, fetch record/sample data
                        targetTuple.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples for deletion)
                    }
                }                
                else { // else statement: if the condition is not a string comparison, prompt error message 
                    cerr << "ERROR: Invalid String Condition. Please try one of the following: =, ==" << endl; // error message: Invalid String Condition (not a comparison between strings)
                    break;
                }    
            }
        }

        int lineIndex = 1; // variable: used to store the current line index of getline() -> tracks the # of '\n' (new line) characters [row(0) -> column header(s); therefore start at row(1) -> attribute data]
        bool match; // boolean variable: used to store the boolean state of index comparison with targetTuple -> does currLine need to be deleted?

        file.seekg(0, ios::beg); // seekg(0, ios::beg): re-read input file from the beginning (data comparison)

        getline(file, currLine); // read line from file input and store value into attributeLine [row(0) -> column header(s)]
        fileCopy << currLine; // output attributeLine into temp target file (new table state)

        while (!file.eof()) {  // while loop: while getline() has not reach the end of the file (EOF), acquire each record's/sample's attribute data. If record's/sample's index matches the indexes of those who passed the comparison, ignore and do not output to target file.
            match = false; // boolean = false (reset for each iteration)
            currLine.clear(); // reset variable value for new data retrieval
            wordList.clear(); // reset vector instance for new parsing retrieval

            getline(file, currLine); // read (next) line from file input and store value into attributeLine 
            
            for (size_t i = 0; i < targetTuple.size(); i++) { // for loop: for each recorded tuple index (pending deletion), compare with current lineIndex to identify if data is exported to target file (updated table)
                if (lineIndex == targetTuple.at(i)) { // if statement: if current lineIndex == any(targetTuple.list)
                    match = true; // change boolean status (true) -> omit record's/sample's attribute data
                    counter++; // counter increment : represents total number of deleted rows of data
                }
            }

            if(!currLine.empty()) { // if statement: if attribute data is not empty, export to target file
                fileCopy << endl; // export '\n' (new line) character : seperation between rows of data

                if (match == true) { // if statement: if match == true -> if current lineIndex needs to be modified, modify corresponding attribute data and insert into updated table 
                    currLine.erase(remove(currLine.begin(), currLine.end(), '|'), currLine.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
                
                    stringstream mf(currLine); // stringstream class fs: string command treated as stream (parsing -> extraction)
                    string temp; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

                    while (getline(mf, temp, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                        if (!temp.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                            wordList.push_back(temp); // push_back(): push current string value into vector (list of words)
                        }
                    }

                    int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)

                    for (int i = 0; i < listSize; i++) { // for loop: for each attribute value index, conduct a comparison with chosen column index to be changed. If attribute value corresponds to attribute that needs to be changed, change the value.
                        if (i == attributeUpdate) { // if statement: if attribute value index == changeAttribute index (same column/dimension)
                            if (changeOperator == "=" || changeOperator == "==") { // if statement: check for set operator (=) for both values and strings
                                wordList.at(i) = changeValue; // change attribute value to specified value (changeValue)
                            }
                            else { // else statement: if not set operator (!= | !==): invalid modification statament -> prompt error (break?)
                                cerr << "ERROR: Invalid Modification Condition. Please try the following: set attr.name = attr.value" << endl;
                            }
                        }
                    }

                    for (size_t i = 0; i < wordList.size(); i++) { // for loop: insert a table tuple using attribute values (seperate section for each attribute)
                        if (!wordList[i].empty()) { // if statement: if the current element of attributesValue contains data (not empty), insert tuple data into new row/record
                            if (i == wordList.size() - 1) { // if statement: if there is only 1 table attribute, export only 1 attribute value
                                fileCopy << wordList[i]; // export tuple data (single)
                            }
                            else { // else statement: else if there are multiple table attributes, export all attribute values seperated by '|'
                                fileCopy << wordList[i] << setw(10) << "|" << setw(12); // export tuple data (multiple)
                            }
                        }
                    }
                }
                else { // else statement: else if match == false -> if current lineIndex does not need to be modified, export original attribute data into updated table 
                    fileCopy << currLine; // export original tuple data
                }
            }

            lineIndex++; // lineIndex increment: represents total number of lines in the file (currLine index tracker)
        }

        if (counter == 1) { // if statement: if counter == 1 (only 1 deleted record/sample), print cout confirmation (single)
            cout << counter << " record modified." << endl; // cout confirmation (success : single)
        }
        else if (counter > 1) { // else if statement: if counter > 1 (multiple deleted records/samples), print cout confirmation (multiple)
            cout << counter << " records modified." << endl; // cout confirmation (success : multiple)
        }
        else { // else statement: if counter <= 0 (no deleted records/samples), print cout confirmation (none)
            cout << "No records modified." << endl; // cout confirmation (failure : none)
        }

        file.close(); // file.close(): close file
        fileCopy.close(); // fileCopy.close(): close file

        const char * oldFile = fileName.c_str(); // string to const char * conversion [remove(const char *)]

        remove(oldFile); // remove/delete original table structure 
        rename("copyTemp.txt", oldFile); // rename updated table structure to reflect the same metadata as the original (same title)   
    }
    else { // else statement: else if file/table does not exist, do not insert tuple to a non-existing table (prompt error message)
        cerr << "!Failed to update tuple from " << tableName << " because it does not exist." << endl; // error message (file does not exist)
    }
}

void innerJoin(vector<string> query, vector<string> tableNames, vector<string> tableAcronyms, string attributeSearch, string condition, string targetOperand) { // innerJoin(): fetch intersecting information from existing table(s)/file(s)
    int comparisonFound_1 = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label); used for 'on' comparison : FILE 1
    int comparisonFound_2 = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label); used for 'on' comparison : FILE 2
    
    int found = -1; // helper-variable: used to record the position of a substring from str.find(); solution to prevent different signedness comparison issues (better than type-casting)

    int numIterations = 0; // variable: used to store the number of occurences in which FILE 1 tuple data must be repeated (multiple intersections with FILE 2)
    int lineIndex = 1; // variable: used to track/store the current lineIndex of the exported file 

    bool all = false; // boolean variable: used to store query instruction state -> Does the command use a * to fetch all relevant tuple data?
    bool error = false; // boolean variable: fail-safe helper (record error status)
    bool failCondition = true; // boolean variable: used to indicate if query options contain valid file/table references : default = true = failedCondition
    bool match = false; // boolean variable: used to determine which tuples are filtered for selection


    string firstFile = tableNames.at(0) + (".txt"); // variable: used to record full name of FIRST file/table for query algorithm
    string secondFile = tableNames.at(1) + (".txt"); // variable: used to record full name of SECOND file/table for query algorithm

    firstFile[0] = toupper(firstFile[0]); // toupper(): capitalize first letter firstFile string to prevent case sensitivity issues
    secondFile[0] = toupper(secondFile[0]); // toupper(): capitalize first letter secondFile string to prevent case sensitivity issues
    
    for (size_t i = 1; i < firstFile.length(); i++) { // for loop: for each letter in firstFile string (except the first), tolower() to prevent case-sensitivity issues
        firstFile[i] = tolower(firstFile[i]); // tolower(): change character case to lower-case
    }

    for (size_t i = 1; i < secondFile.length(); i++) { // for loop: for each letter in secondFile string (except the first), tolower() to prevent case-sensitivity issues
        secondFile[i] = tolower(secondFile[i]); // tolower(): change character case to lower-case
    }

    string swap; // variable: used to store temporary strings for the sake of swapping values
    string line; // variable: used to store current state of getline() from file data
    string currLine; // variable: used to store current state of getline() from file data
    string currentFile; // variable: temporary string used to store file names and conduct toupper() and tolower() for case-sensitive comparisons
    string attributeLine_1; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType : FILE 1
    string attributeLine_2; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType : FILE 2

    vector<size_t> attributeFound_1; // vector instance: used to store relevant indexes from attributeList (list of attributes) : FILE 1
    vector<size_t> attributeFound_2; // vector instance: used to store relevant indexes from attributeList (list of attributes) : FILE 2
    
    vector<int> targetTuple_1; // vector instance: used to store designated tuple indexes (integers) for fetching (which lines are to be read) : FILE 1
    vector<int> targetTuple_2; // vector instance: used to store designated tuple indexes (integers) for fetching (which lines are to be read) : FILE 2
    vector<int> matchingTuple; // vector instance (int): used to store corresponding indexes that FILE 1 tuples have with FILE 2 (indexes where interesections exist)

    vector<string> wordList; // vector instance: used to store parsed string to identify attributeName and attributeType
    vector<string> attributeList_1; // vector instance: used to store parsed string to identify attributeName : FILE 1
    vector<string> attributeType_1; // vector instance: used to stored parsed string to identify attributeType : FILE 1
    vector<string> attributeValue_1; // vector instance: used to store attribute values for each record/sample for fetching : FILE 1
    vector<string> comparisonValue_1; // vector instace: used to store attribute valuess for each record/sample for comparison : FILE 1
    vector<string> attributeList_2; // vector instance: used to store parsed string to identify attributeName : FILE 2
    vector<string> attributeType_2; // vector instance: used to stored parsed string to identify attributeType  : FILE 2
    vector<string> attributeValue_2; // vector instance: used to store attribute values for each record/sample for fetching : FILE 2
    vector<string> comparisonValue_2; // vector instance: used to store attribute values for each record/sample for comparison : FILE 2
    vector<string> joinAttributesName; // vector instance: used to store joined attribute field names for multiple file(s)/table(s)
    vector<string> joinAttributesType; // vector instance: used to store join atttribute field types for multiple file(s)/table(s)

    ifstream file; // fstream initialization : FILE 1
    ifstream file2; // fstream initalization : FILE 2
    
    fstream fileCopy; // fstream initialization : TEMP READ

    file.open(firstFile, ios::in); // file.open(): attempt to open target File 1 in read-only
    file2.open(secondFile, ios::in); // file.open(): attempt to open target File 2 in read-only

    attributeSearch[0] = toupper(attributeSearch[0]); // toupper(): capitalize first letter attributeSearch string to prevent case sensitivity issues
    targetOperand[0] = toupper(targetOperand[0]); // toupper(): capitalize first letter targetOperand string to prevent case sensitivity issues

    for (size_t i = 0; i < tableAcronyms.size(); i++) { // for loop: for each recorded file (w/ corresponding acronyms), conduct a toupper() transoffmration to prevent case sensitivity issues
        transform(tableAcronyms.at(i).begin(), tableAcronyms.at(i).end(), tableAcronyms.at(i).begin(), ::toupper); // transform(..., ::toupper): var. tableAcronyms (file indicators) are case sensitive; therefore, transform() each entire string UPPERCASE to ignore case-sensitive issues
    }

    found = attributeSearch.find(tableAcronyms.at(0) + '.'); // find(): search for File 1 Acronym in first conditional parameter (attributeSearch)

    if (found == -1) { // if statement: if File 1 Acronym is not found in first conditional parameter, look in second conditional parameter
        found = targetOperand.find(tableAcronyms.at(0) + '.'); // find(): search for File 2 Acronym in second conditional parameter (targerOperand)

        if (found == -1) { // if statement: if File 1 Acronym is not found in second conditional parameter, prompt an error message (no valid attribute reference)
            cerr << "ERROR: " << firstFile << " acronym (" << tableAcronyms.at(0) << ") does not exist within query instructions. Please try again!" << endl; // error message (invalid attribute reference)
            error = true;
        }
        else { // else statement: else if File 1 Acronym is in the second conditional parameter, swap first and second conditional parameter -> [FORMAT: File 1, File 2 ... Acronym1 [condition] Acronym2] (ORDER MATTERS)    
            swap = attributeSearch; // temp placeholder
            attributeSearch = targetOperand; // move second parameter to first parameter
            targetOperand = swap; // move temp placeholder to second parameter
        }
    }

    found = attributeSearch.find("."); // find(): search for "." characters in first conditional parameter (attributeSearch)

    if (found != -1) { // if statement: if attributeSearch contains a "." character, extract attributeName (string after the ".")
        attributeSearch = attributeSearch.substr(found + 1); // fetch attributeName for File 1
    }
    else { // else statement: else if attributeSearch does not contain a "." character, prompt an error message (invalid table reference)
        cerr << "ERROR: " << attributeSearch << " is an ambiguious parameter. Please try again!" << endl; // error message (reference ambiguity)
        error = true; // error = true
    }

    found = targetOperand.find(tableAcronyms.at(1) + '.'); // find(): search for File 2 Acronym in second conditional parameter (targetOperand)

    if (found == -1) { // if statement: if File 2 Acronym is not found in second conditional parameter, look in first conditional parameter
        found = attributeSearch.find(tableAcronyms.at(1) + '.'); // find(): search for File 2 Acronym in first conditional parameter (attributeSearch)

        if (found == -1) { // if statement: if File 2 Acronym is not found in first conditional parameter, prompt an error message (no valid attribute reference)
            cerr << "ERROR: " << secondFile << " acronym (" << tableAcronyms.at(1) << ") does not exist within query instructions. Please try again!" << endl; // error message (invalid attribute reference)
            error = true; // error = true
        }
        else { // else statement: else if File 2 Acronym is in the first conditional parameter, swap first and second conditional parameter -> [FORMAT: File 1, File 2 ... Acronym1 [condition] Acronym2] (ORDER MATTERS)    
            swap = attributeSearch; // temp placeholder
            attributeSearch = targetOperand; // move second parameter to first parameter
            targetOperand = swap; // move temp placeholder to second parameter
        }
    }

    found = targetOperand.find("."); // find(): search for "." characters in second conditional parameter (targetOperand)

    if (found != -1) { // if statement: if targetOperand contains a "." character, extract attributeName (string after the ".")
        targetOperand = targetOperand.substr(found + 1); // fetch attributeName for File 2
    }
    else { // else statement: else if targetOperand does not contain a "." character, prompt an error message (invalid table reference)
        cerr << "ERROR: " << targetOperand << " is an ambiguious parameter. Please try again!" << endl; // error message (reference ambiguity)
        error = true; // error = true
    }

    if (error != true) { // if statement: if no error occured (user input contains valid parameters), conduct inner join algorithm
        if (file) { // if statement: if File 1 / Table 1 exists, attempt to conduct an inner joined query
            if (file2) { // if statement: if File 2 / Table 2 exists, attempt to conduct an inner joined query
                fileCopy.open("copyTemp.txt", ios::out); // fileCopy.open(): create a temp target file that will store updated table data (post-filter)

                attributeLine_1.clear(); // reset variable value for new parsing retrieval : FILE 1
                getline(file, attributeLine_1); // read line from file input and store value into attributeLine : FILE 1
                attributeLine_1.erase(remove(attributeLine_1.begin(), attributeLine_1.end(), '|'), attributeLine_1.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries) : FILE 1

                attributeLine_2.clear(); // reset variable value for new parsing retrieval : FILE 2
                getline(file2, attributeLine_2); // read line from file input and store value into attributeLine : FILE 22
                attributeLine_2.erase(remove(attributeLine_2.begin(), attributeLine_2.end(), '|'), attributeLine_2.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries) : FILE 2

                stringstream firstSS(attributeLine_1); // stringstream class firstSS: string command treated as stream (parsing -> extraction) : FILE 1
                stringstream secondSS(attributeLine_2); // stringstream class secondSS: string command treated as stream (parsing -> extraction) : FILE 2

                string temp_1; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList) : FILE 1
                string temp_2; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList) : FILE 2
            
                // FILE 1 PARSING //

                while (getline(firstSS, temp_1, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                    if (!temp_1.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                        wordList.push_back(temp_1); // push_back(): push current string value into vector (list of words)
                    }
                }

                for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attribute metadata: attributeName/attributeType (every other element -> i += 2)
                    attributeList_1.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
                    attributeType_1.push_back(wordList.at(i + 1)); // push_back(): push attributeType value into vector (list of attributeTypes)
                }

                // FILE 2 PARSING //

                wordList.clear(); // wordList.clear(): reset vector instance for File 2 parsing (new retrieval)

                while (getline(secondSS, temp_2, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                    if (!temp_2.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                        wordList.push_back(temp_2); // push_back(): push current string value into vector (list of words)
                    }
                }

                for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attribute metadata: attributeName/attributeType (every other element -> i += 2)
                    attributeList_2.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
                    attributeType_2.push_back(wordList.at(i + 1)); // push_back(): push attributeType value into vector (list of attributeTypes)
                }

                // QUERY ANALYSIS //

                for (size_t i = 0; i < query.size(); i++) { // for loop: for each query option, conduct a filename search to indicate valid query join
                    failCondition = true; // default = true = failedCondition (look for file names, if found, then no fail)

                    if (query.at(i).compare("*") != 0) { // if statement: if any of the query options does not contain a "*" (fetch all) character, conduct a conditional query check for each entry
                        for (size_t j = 0; j < tableNames.size(); j++) { // for loop: for each file/table name arugment passed in the command, conduct a search for pre-determined file/table instances
                            currentFile = tableNames.at(j); // temp string: holds the string value of each file/table 

                            currentFile[0] = toupper(currentFile[0]); // toupper(): capitalize first letter currentFile string to prevent case sensitivity issues

                            for (size_t k = 1; k < currentFile.size(); k++) { // for loop: for each letter in firstFile string (except the first), tolower() to prevent case-sensitivity issues
                                currentFile[k] = tolower(currentFile[k]); // tolower(): change character case to lower-case for all characters except the first letter 
                            }

                            query.at(i)[0] = toupper(query.at(i)[0]); // toupper: captialize first letter of query.at(i) to prevent case sensitivity issues (first part of the query should be the file name)

                            found = query.at(i).find("."); /// find(): search for a "." character in the query string (representing boundary between reference and attribute)

                            for (int k = 1; k < found; k++) { // for loop: for each character between the first letter and the "." character boundary, tolower() to prevent case sensitivity issues for file names
                                query.at(i)[k] = tolower(query.at(i)[k]); // tolower(): change character case to lower-case for all characters except the first letter 
                            }

                            found = query.at(i).find(currentFile); // find(): search for parameterized file name within the query string

                            if (found != -1) { // if statement: if fileName found within query string, then query option is valid (failCondition = false = pass)
                                failCondition = false; // failCondition = false = pass
                            }
                        }

                        if (failCondition == true) { // if statement: if failCondiition == true such that the query option did not contain a file/table reference, prompt an error message
                            cerr << "ERROR: Invalid Query Attribute. " << query.at(i) << " is unavailable!" << endl; // error message (invalid query option)
                            error = true; // error = true
                        }
                    }
                    else { // else statement: else if any of the query options contain a "*" (fetch all) character, ignore conditional check
                        failCondition = false; // failCondition = false = pass
                    }
                }

                for (size_t i = 0; i < query.size(); i++) { // for loop: for each query target, check to see if instruction = * (all data)
                    if (query.at(i).compare("*") == 0) { // if statement: query.element == "*" (all info) -> print all relevant table information to the user
                        all = true; // all -> true
                    }
                    else {
                        found = query.at(i).find("."); // find(): search for "." characters in first conditional parameter (attributeSearch)

                        if (found != -1) { // if statement: if attributeSearch contains a "." character, extract attributeName (string after the ".")
                            query.at(i) = query.at(i).substr(found + 1); // fetch attributeName for File 1
                        }
                        else { // else statement: else if attributeSearch does not contain a "." character, prompt an error message (invalid table reference)
                            cerr << "ERROR: " << query.at(i) << " is an ambiguious attribute. Please try again!" << endl; // error message (reference ambiguity)
                            error = true; // error = true
                        }
                    }
                }

                if (error != true) { // if statement: if no error occured (query options are not ambigious), continue conducting inner join algorithm
                    if (all == true) { // if statement: all == true : instruction == * ; therefore, query all attributes
                        query.clear(); // reset vector instance for new parsing retrieval

                        // FILE 1 QUERY (*) //

                        for (size_t i = 0; i < attributeList_1.size(); i++) { // for loop: for each available attribute, push attribute label into query (list of relevant attributes)
                            query.push_back(attributeList_1.at(i)); // push_back(): push current string value into vector (table attribute)
                        }

                        // FILE 2 QUERY (*) //

                        for (size_t i = 0; i < attributeList_2.size(); i++) { // for loop: for each available attribute, push attribute label into query (list of relevant attributes)
                            query.push_back(attributeList_2.at(i)); // push_back(): push current string value into vector (table attribute)
                        }
                    }

                    // ATTRIBUTE VALIDITY CHECK //

                    for (size_t i = 0; i < query.size(); i++) { // for loop: for each given query option, determine attribute validity (is the query attribute in the table?) 
                    
                        // FILE 1 QUERY VALIDITY CHECK //

                        for (size_t j = 0; j < attributeList_1.size(); j++) { // for loop: for each attribute in the table, conduct a comparison to see if given attributes exists in the table (record pos. of attributes to identify attribute values in each record/sample)
                            if (query.at(i) == attributeList_1.at(j)) { // if statement: if target attribute found in list of attributes
                                attributeFound_1.push_back(j); // record pos. of target attribute (which dimension corresponds with target attribute)
                            }
                        }

                        // FILE 2 QUERY VALIDITY CHECK //

                        for (size_t j = 0; j < attributeList_2.size(); j++) { // for loop: for each attribute in the table, conduct a comparison to see if given attributes exists in the table (record pos. of attributes to identify attribute values in each record/sample)
                            if (query.at(i) == attributeList_2.at(j)) { // if statement: if target attribute found in list of attributes
                                attributeFound_2.push_back(j); // record pos. of target attribute (which dimension corresponds with target attribute)
                            }
                        }
                    }

                    // ATTRIBUTE VALIDITY ERROR PROMPT //

                    if ((attributeFound_1.size() + attributeFound_2.size()) != query.size()) { // if statement: if one or more target attribute do/does not exist in either table, prompt error message 
                        if (query.size() == 1) { // if statement: if query contains a single target attribute (print invalid error (single))
                            cerr << "ERROR: Invalid Attribute Selection. Please try again!" << endl; // cout confirmation: target attribute(s) do/does not exist in either table (error message)

                            attributeFound_1.clear(); // reset vector instance (error: do not fetch tuple data)
                        }
                        else if (query.size() > 1) { // else if statement: if query contains multiple target attributes (print invalid error (multiple))
                            cerr << "ERROR: One or More Invalid Attribute Selection(s). Please try again!" << endl; // cout confirmation: target attribute(s) do/does not exist in either table (error message)

                            attributeFound_1.clear(); // reset vector instance (error: do not fetch tuple data)
                        }
                    }

                    // FILE 1 ATTRIBUTE LIST + CHECK //

                    for (size_t i = 0; i < attributeList_1.size(); i++) { // for loop: for each attribute in the table, conduct a comparison to see if comparison attribute exists in the table (record pos. of attribute to identify attribute values in each record/sample)
                        if (attributeList_1.at(i) == attributeSearch) { // if statement: if target attribute found in list of attributes
                            comparisonFound_1 = i; // record pos. of target attribute (which dimension corresponds with target attribute)
                        }
                    }

                    if (comparisonFound_1 == -1) { // if statement: if target attribute does not exist in the table, prompt error message 
                        cerr << "ERROR: Invalid Attribute Operand. " << firstFile << " does not include: " << attributeSearch << "." << endl; // error message: target attribute does not exist in the table
                        error = true; // error = true
                    }

                    // FILE 2 ATTRIBUTE LIST + CHECK //

                    for (size_t i = 0; i < attributeList_2.size(); i++) { // for loop: for each attribute in the table, conduct a comparison to see if comparison attribute exists in the table (record pos. of attribute to identify attribute values in each record/sample)
                        if (attributeList_2.at(i) == targetOperand) { // if statement: if target attribute found in list of attributes
                            comparisonFound_2 = i; // record pos. of target attribute (which dimension corresponds with target attribute)
                        }
                    }

                    if (comparisonFound_2 == -1) { // if statement: if target attribute does not exist in the table, prompt error message 
                        cerr << "ERROR: Invalid Attribute Operand. " << secondFile << " does not include: " << targetOperand << "." << endl; // error message: target attribute does not exist in the table
                        error = true; // error = true
                    }

                    // ATTRIBUTE VALUES -> COMPARISON FILTERING //

                    if (error != true) { // if statement: if no error occured (valid attribute comparison statement), continue inner join algorithm
                        // FILE 1: Fetch attribute values, and choose which values to use in comparison statement //

                        while (!file.eof()) { // while loop: while getline() has not reach the end of the file (EOF), parse each record's/sample's attribute data for filtered comparison
                            firstSS.str(string()); // reset stream for new parsing retrieval
                            attributeLine_1.clear(); // reset variable value for new parsing retrieval
                            wordList.clear(); // reset vector instance for new parsing retrieval

                            getline(file, attributeLine_1); // read line from file input and store value into attributeLine

                            attributeLine_1.erase(remove(attributeLine_1.begin(), attributeLine_1.end(), '|'), attributeLine_1.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
                        
                            stringstream firstFS(attributeLine_1); // stringstream class firstFS: string command treated as stream (parsing -> extraction)
                            string temp_1; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

                            while (getline(firstFS, temp_1, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                                if (!temp_1.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                                    wordList.push_back(temp_1); // push_back(): push current string value into vector (list of words)
                                }
                            }

                            for (size_t i = 0; i < wordList.size(); i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute's value(s) of the given record/sample 
                                attributeValue_1.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of attribute's data)
                            }

                            int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)
                            
                            for (int i = 0; i < listSize; i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute value of the given record/sample 
                                if (i == comparisonFound_1) { // if statement: if the current index matches the target attribute index, push record's/sample's attribute data into a list of comparison attribute data. 
                                    comparisonValue_1.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of comparison attribute's data)                            
                                }
                            }               
                        }

                        // FILE 2: Fetch attribute values, and choose which values to use in comparison statement //

                        while (!file2.eof()) { // while loop: while getline() has not reach the end of the file (EOF), parse each record's/sample's attribute data for filtered comparison
                            secondSS.str(string()); // reset stream for new parsing retrieval
                            attributeLine_2.clear(); // reset variable value for new parsing retrieval
                            wordList.clear(); // reset vector instance for new parsing retrieval

                            getline(file2, attributeLine_2); // read line from file input and store value into attributeLine

                            attributeLine_2.erase(remove(attributeLine_2.begin(), attributeLine_2.end(), '|'), attributeLine_2.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
                        
                            stringstream secondFS(attributeLine_2); // stringstream class fs: string command treated as stream (parsing -> extraction)
                            string temp_2; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

                            while (getline(secondFS, temp_2, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                                if (!temp_2.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                                    wordList.push_back(temp_2); // push_back(): push current string value into vector (list of words)
                                }
                            }
                                            
                            for (size_t j = 0; j < wordList.size(); j++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute's value(s) of the given record/sample 
                                    attributeValue_2.push_back(wordList.at(j)); // push_back(): push current string value into vector (list of attribute's data)
                            }

                            int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)
                            
                            for (int i = 0; i < listSize; i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute value of the given record/sample 
                                if (i == comparisonFound_2) { // if statement: if the current index matches the target attribute index, push record's/sample's attribute data into a list of comparison attribute data. 
                                    comparisonValue_2.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of comparison attribute's data)
                                }
                            }                  
                        }

                        // ATTRIBUTE COMPARISON (MULTIPLE FILES/TABLES) //

                        for (size_t i = 0; i < comparisonValue_1.size(); i++) { // for loop: FILE 1 :for each record's/sample's attribute data, conduct 'on' comparison. If attribute value passes comparison, record its corresponding row for selection (filter).
                            for (size_t j = 0; j < comparisonValue_2.size(); j++) { // for loop: FILE 2 :for each record's/sample's attribute data, conduct 'on' comparison. If attribute value passes comparison, record its corresponding row for selection (filter).
                                if (isNumber(comparisonValue_1.at(i)) && isNumber(comparisonValue_2.at(j))) { // if statement: if the values being compared (i.e., attributeSearch[x], targetOperand[x], etc.) are numbers [isNumber() helper function], conduct numerical arithmetic (e.g., >, <, >=, <=, and =)
                                    float operand_1 = 0; // variable: left-side operand (float -> scenarios that involve decimal values (i.e., pricing, income, etc.))
                                    float operand_2 = 0; // variable: right-side operand (float -> scenarios that involve decimal values ...)
                                    
                                    stringstream op1(comparisonValue_1.at(i)); // stringstream class op1: string command treated as stream (extraction) -> string to float conversion
                                    stringstream op2(comparisonValue_2.at(j)); // stringstream class op2: string command treated as stream (extraction) -> string to float conversion

                                    op1 >> operand_1; // string to float conversion (left-side)
                                    op2 >> operand_2; // string to float conversion (right-side)

                                    if (condition == ">") { // if statement: if the condition operation == >, conduct a greater than comparison
                                        if (operand_1 > operand_2) { // if statement: if operand_1 > operand_2
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                            targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                        }
                                    }
                                    else if (condition == "<") {  // else if statement: if the condition operation == <, conduct a less than comparison
                                        if (operand_1 < operand_2) { // if statement: if operand_1 < operand_2
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                            targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                        }
                                    }
                                    else if (condition == "<=") { // else if statement: if the condition operation == <=, conduct a less than or equal to comparison
                                        if (operand_1 <= operand_2) { // if statement: if operand_1 <= operand_2
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                            targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                        }
                                    }
                                    else if (condition == ">=") { // else if statement: if the condition operation == <=, conduct a greater than or equal to comparison
                                        if (operand_1 >= operand_2) { // if statement: if operand_1 >= operand_2
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                            targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                        }
                                    }
                                    else if (condition == "=") { // else if statement: if the condition operation == =, conduct an equal comparison
                                        if (operand_1 == operand_2) { // if statement: if operand_1 == operand_2
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                            targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                        }                    
                                    }
                                    else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a not equal comparison
                                        if (operand_1 != operand_2) { // if statement: if operand_1 != operand_2
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                            targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                        }
                                    }
                                    else { // else statement: if the condition operation is invalid (none of the aforementioned cases), prompt error message
                                        cerr << "ERROR: Invalid Numerical Condition. Please try one of the following: >, <, >=, <=, =" << endl; // error message: Invalid Numerical Condition (not a comparison between numbers)
                                        break;
                                    }   

                                    op1.str(string()); // reset stream for new parsing retrieval [operand_1 is the only operand changing with each comparison (different record/sample data)]
                                }
                                else if (!isNumber(comparisonValue_1.at(i)) && !isNumber(comparisonValue_2.at(j))) { // else statement: else if values being compared (i.e., attributeSearch[x], targetOperand[x], etc.) are not numbers, then they classify as string(s)/object(s) and can not support Numerical Comparions (only ==)
                                    if ((condition == "=") || (condition == "==")) { // if statement: if the condition operation == = || ==, conduct a string comparison [are the strings the same (i.e., filter by name, tile, etc.)]
                                        if (comparisonValue_1.at(i) == comparisonValue_2.at(j)) { // if statement: if the record's/sample's current string value matches the value of the target operand, fetch record/sample data 
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                                            targetTuple_2.push_back(j + 1);
                                        }
                                    }
                                    else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a string NOT comparison 
                                        if (comparisonValue_1.at(i) != comparisonValue_2.at(j))  { // if statement: if the record's/sample's current string value does not match the value of the target operand, fetch record/sample data
                                            targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                                            targetTuple_2.push_back(j + 1);
                                        }
                                    }
                                    else { // else statement: if the condition is not a string comparison, prompt error message 
                                        cerr << "ERROR: Invalid String Condition. Please try one of the following: =, ==" << endl; // error message: Invalid String Condition (not a comparison between strings)
                                        break;
                                    }    
                                }
                                else { // else statement: else if one of the values is a number, but the other value is a non-number, then prompt an error message (Invalid DataType Operation)
                                    cerr << "ERROR: Invalid Conditional Statement. DataType Violation!" << endl; // error message: Invalid DataType Operation
                                    break;
                                }
                            }
                        }

                        if (targetTuple_1.empty() || targetTuple_2.empty()) { // if statement: if either targetTuple (list of tuples to query) is empty, prompt an error message (cannot conduct a JOIN without successful filtering/conditions)
                            cerr << "ERROR: Invalid Query. Cannnot find tuples that pass the following condition: " << attributeSearch << " " << condition << " " << targetOperand << endl; // error message (failed conditional statement)
                            error = true; // boolean: error = true -> failsafe
                        }

                        if (error != true) { // if statement: if no error occured (successful filtered query), fetch relevant tuple data by creating/reading a temp copy
                            
                            // EXPORT INNER JOIN HEADER (COMBINE FILE HEADER(S)) //
                            
                            for (size_t i = 0; i < query.size(); i++) { // for loop: for each query option, determine which file the indicated attribute belongs to and export complete inner join header (ORDER MATTERS BASED ON QUERY ENTRY)
                                for (size_t j = 0; j < attributeList_1.size(); j++) { // for loop: for each attribute in FILE 1, conduct query option comparison (does it exist in FILE 1?)
                                    if (query.at(i) == attributeList_1.at(j)) { // if statement: if query option exists in FILE 1 attribute list, export using proper formatting (single | multiple)
                                        if (i == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                            fileCopy << attributeList_1[j] << " " << attributeType_1[j]; // export table attribute (single)
                                        }
                                        else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                            fileCopy << attributeList_1[j] << " " << attributeType_1[j] << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                        }
                                    }
                                }

                                for (size_t j = 0; j < attributeList_2.size(); j++) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                    if (query.at(i) == attributeList_2.at(j)) { // if statement: if query option exists in FILE 2 attribute list, export using proper formatting (single | multiple)
                                        if (i == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                            fileCopy << attributeList_2[j] << " " << attributeType_2[j]; // export table attribute (single)
                                        }
                                        else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                            fileCopy << attributeList_2[j] << " " << attributeType_2[j] << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                        }
                                    }
                                }
                            }

                            // EXPORT INNER JOIN TUPLE DATA (INTERSECTION) // 
                            
                            for (size_t i = 0; i < attributeValue_1.size(); i += attributeFound_1.size()) { // for loop: for each record's/sample's (valid) attribute value, export relevant table data to a temp target file for proper getline() reading [attributeValue format: queryNum -> 3 attr. : attributeValue[0] = attr.1, attributeValue[1] = attr.2, attributeValue[2] = attr. 3, attributeValue[4] = attr. 1, attributeValue[5] = attr.2, etc.]                    
                                numIterations = 0; // reset numIterations for each record/sample 
                                match = false; // boolean: match = false -> default (conditions)

                                matchingTuple.clear(); // reset vector instance for new matchingTuple index retrieval

                                for (size_t j = 0; j < targetTuple_1.size(); j++) { // for loop: for each recorded tuple index (pending selection), compare with current lineIndex to identify if data is exported to target file (updated table)                                    
                                    if (lineIndex == targetTuple_1.at(j)) { // if statement: if current lineIndex == any(targetTuple.list)
                                        match = true; // change boolean status (true) -> export record's/sample's attribute data for reading
                                        
                                        matchingTuple.push_back(targetTuple_2.at(j)); // fetch matchingTuple index based on FILE 1 condition

                                        numIterations++; // numIterations increment (total number of matching record(s)/sample(s))
                                    }
                                }

                                for (int j = 0; j < numIterations; j++) { // for loop: for each matching record/sample (multiple iterations = repeated record(s)/sample(s))
                                    if (match == true) { // if statement: if match == true -> if current tuple data is relevant, export to temp copy
                                        fileCopy << endl; // export '\n' (new line) character : seperation between rows of data

                                        for (size_t k = 0; k < query.size(); k++) { // for loop: for each query option, determine which file the indicated attribute belongs to, then export the appropriate data (attributeName -> attributeValue)
                                            for (size_t l = 0; l < attributeList_1.size(); l++) { // for loop: for each attribute in FILE 1, conduct query option comparison (does it exist in FILE 1?)
                                                if (query.at(k) == attributeList_1.at(l)) { // if statement: if query option exists in FILE 1 attribute list, export the corresponding attribute value w/ proper formatting (single | multiple)
                                                    if (k == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                        fileCopy << attributeValue_1.at(l + ((lineIndex - 1) * attributeList_1.size())); // export table attribute (single)
                                                    }
                                                    else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                        fileCopy << attributeValue_1.at(l + ((lineIndex - 1) * attributeList_1.size())) << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                                    }
                                                }
                                            }

                                            for (size_t l = 0; l < attributeList_2.size(); l++) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                                if (query.at(k) == attributeList_2.at(l)) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                                    if (k == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                        fileCopy << attributeValue_2.at(l + ((matchingTuple.at(j) - 1) * attributeList_2.size())); // export table attribute (single)
                                                    }
                                                    else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                        fileCopy << attributeValue_2.at(l + ((matchingTuple.at(j) - 1) * attributeList_2.size())) << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                lineIndex++; // lineIndex increment (represents next tuple data index/row)
                            }

                            fileCopy.close(); // fileCopy.close(): close file from write-only

                            fileCopy.open("copyTemp.txt", ios::in); // fileCopy.open(): attempt to open file in read-only (getline() fetch)

                            while (getline(fileCopy, line)) { // while loop: while getline() continues to extract file data line-by-line, print to user
                                cout << line << endl; // cout table data
                            }
                        }
                    }
                }

                file.close(); // file.close(): close file from read-only
                file2.close(); // file.close(): close file from read-only
                fileCopy.close(); // fileCopy.close(): close file from read-only

                remove("copyTemp.txt"); // after displaying relevant data, remove/delete temp target
            }
            else { // else statement: else if FILE 2 / TABLE 2 does not exist, do not fetch a query (prompt error message)
                cerr << "!Failed to query table " << secondFile << " because it does not exist." << endl; // error message (FILE 2 does not exist)
            }
        }
        else { // else statement: else if FILE 1 / TABLE 1 does not exist, do not fetch a query (prompt error message)
            cerr << "!Failed to query table " << firstFile << " because it does not exist." << endl; // error message (FILE 1 does not exist)
        }    
    }
}

void outerJoin(vector<string> query, vector<string> tableNames, vector<string> tableAcronyms, string attributeSearch, string condition, string targetOperand, string direction) { // outerJon(): fetch intersecting information from existing table(s)/file(s), and return all unmatched rows in a given (directional) table
    int comparisonFound_1 = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label); used for 'on' comparison : FILE 1
    int comparisonFound_2 = -1; // variable: stores the position of an existing attributeName (pos of each attribute value for each record/label); used for 'on' comparison : FILE 2
    
    int found = -1; // helper-variable: used to record the position of a substring from str.find(); solution to prevent different signedness comparison issues (better than type-casting)

    int numIterations = 0; // variable: used to store the number of occurences in which FILE 1 tuple data must be repeated (multiple intersections with FILE 2)
    int lineIndex = 1; // variable: used to track/store the current lineIndex of the exported file 

    bool all = false; // boolean variable: used to store query instruction state -> Does the command use a * to fetch all relevant tuple data?
    bool error = false; // boolean variable: fail-safe helper (record error status)
    bool failCondition = true; // boolean variable: used to indicate if query options contain valid file/table references : default = true = failedCondition
    bool match = false; // boolean variable: used to determine which tuples are filtered for selection


    string firstFile = tableNames.at(0) + (".txt"); // variable: used to record full name of FIRST file/table for query algorithm
    string secondFile = tableNames.at(1) + (".txt"); // variable: used to record full name of SECOND file/table for query algorithm

    firstFile[0] = toupper(firstFile[0]); // toupper(): capitalize first letter firstFile string to prevent case sensitivity issues
    secondFile[0] = toupper(secondFile[0]); // toupper(): capitalize first letter secondFile string to prevent case sensitivity issues
    
    for (size_t i = 1; i < firstFile.length(); i++) { // for loop: for each letter in firstFile string (except the first), tolower() to prevent case-sensitivity issues
        firstFile[i] = tolower(firstFile[i]); // tolower(): change character case to lower-case
    }

    for (size_t i = 1; i < secondFile.length(); i++) { // for loop: for each letter in secondFile string (except the first), tolower() to prevent case-sensitivity issues
        secondFile[i] = tolower(secondFile[i]); // tolower(): change character case to lower-case
    }

    string swap; // variable: used to store temporary strings for the sake of swapping values
    string line; // variable: used to store current state of getline() from file data
    string currLine; // variable: used to store current state of getline() from file data
    string currentFile; // variable: temporary string used to store file names and conduct toupper() and tolower() for case-sensitive comparisons
    string attributeLine_1; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType : FILE 1
    string attributeLine_2; // variable: used to store inital state of getline() [line 0], representing the table header, outlining attributeName and attributeType : FILE 2

    vector<size_t> attributeFound_1; // vector instance: used to store relevant indexes from attributeList (list of attributes) : FILE 1
    vector<size_t> attributeFound_2; // vector instance: used to store relevant indexes from attributeList (list of attributes) : FILE 2
    
    vector<int> targetTuple_1; // vector instance: used to store designated tuple indexes (integers) for fetching (which lines are to be read) : FILE 1
    vector<int> targetTuple_2; // vector instance: used to store designated tuple indexes (integers) for fetching (which lines are to be read) : FILE 2
    vector<int> matchingTuple; // vector instance (int): used to store corresponding indexes that FILE 1 tuples have with FILE 2 (indexes where interesections exist)

    vector<string> wordList; // vector instance: used to store parsed string to identify attributeName and attributeType
    vector<string> attributeList_1; // vector instance: used to store parsed string to identify attributeName : FILE 1
    vector<string> attributeType_1; // vector instance: used to stored parsed string to identify attributeType : FILE 1
    vector<string> attributeValue_1; // vector instance: used to store attribute values for each record/sample for fetching : FILE 1
    vector<string> comparisonValue_1; // vector instace: used to store attribute valuess for each record/sample for comparison : FILE 1
    vector<string> attributeList_2; // vector instance: used to store parsed string to identify attributeName : FILE 2
    vector<string> attributeType_2; // vector instance: used to stored parsed string to identify attributeType  : FILE 2
    vector<string> attributeValue_2; // vector instance: used to store attribute values for each record/sample for fetching : FILE 2
    vector<string> comparisonValue_2; // vector instance: used to store attribute values for each record/sample for comparison : FILE 2
    vector<string> joinAttributesName; // vector instance: used to store joined attribute field names for multiple file(s)/table(s)
    vector<string> joinAttributesType; // vector instance: used to store join atttribute field types for multiple file(s)/table(s)

    ifstream file; // fstream initialization : FILE 1
    ifstream file2; // fstream initalization : FILE 2
    
    fstream fileCopy; // fstream initialization : TEMP READ

    file.open(firstFile, ios::in); // file.open(): attempt to open target File 1 in read-only
    file2.open(secondFile, ios::in); // file.open(): attempt to open target File 2 in read-only

    found = attributeSearch.find(tableAcronyms.at(0) + '.'); // find(): search for File 1 Acronym in first conditional parameter (attributeSearch)

    if (found == -1) { // if statement: if File 1 Acronym is not found in first conditional parameter, look in second conditional parameter
        found = targetOperand.find(tableAcronyms.at(0) + '.'); // find(): search for File 2 Acronym in second conditional parameter (targerOperand)

        if (found == -1) { // if statement: if File 1 Acronym is not found in second conditional parameter, prompt an error message (no valid attribute reference)
            cerr << "ERROR: " << firstFile << " acronym (" << tableAcronyms.at(0) << ") does not exist within query instructions. Please try again!" << endl; // error message (invalid attribute reference)
            error = true;
        }
        else { // else statement: else if File 1 Acronym is in the second conditional parameter, swap first and second conditional parameter -> [FORMAT: File 1, File 2 ... Acronym1 [condition] Acronym2] (ORDER MATTERS)    
            swap = attributeSearch; // temp placeholder
            attributeSearch = targetOperand; // move second parameter to first parameter
            targetOperand = swap; // move temp placeholder to second parameter
        }
    }

    found = attributeSearch.find("."); // find(): search for "." characters in first conditional parameter (attributeSearch)

    if (found != -1) { // if statement: if attributeSearch contains a "." character, extract attributeName (string after the ".")
        attributeSearch = attributeSearch.substr(found + 1); // fetch attributeName for File 1
    }
    else { // else statement: else if attributeSearch does not contain a "." character, prompt an error message (invalid table reference)
        cerr << "ERROR: " << attributeSearch << " is an ambiguious parameter. Please try again!" << endl; // error message (reference ambiguity)
        error = true; // error = true
    }

    found = targetOperand.find(tableAcronyms.at(1) + '.'); // find(): search for File 2 Acronym in second conditional parameter (targetOperand)

    if (found == -1) { // if statement: if File 2 Acronym is not found in second conditional parameter, look in first conditional parameter
        found = attributeSearch.find(tableAcronyms.at(1) + '.'); // find(): search for File 2 Acronym in first conditional parameter (attributeSearch)

        if (found == -1) { // if statement: if File 2 Acronym is not found in first conditional parameter, prompt an error message (no valid attribute reference)
            cerr << "ERROR: " << secondFile << " acronym (" << tableAcronyms.at(1) << ") does not exist within query instructions. Please try again!" << endl; // error message (invalid attribute reference)
            error = true; // error = true
        }
        else { // else statement: else if File 2 Acronym is in the first conditional parameter, swap first and second conditional parameter -> [FORMAT: File 1, File 2 ... Acronym1 [condition] Acronym2] (ORDER MATTERS)    
            swap = attributeSearch; // temp placeholder
            attributeSearch = targetOperand; // move second parameter to first parameter
            targetOperand = swap; // move temp placeholder to second parameter
        }
    }

    found = targetOperand.find("."); // find(): search for "." characters in second conditional parameter (targetOperand)

    if (found != -1) { // if statement: if targetOperand contains a "." character, extract attributeName (string after the ".")
        targetOperand = targetOperand.substr(found + 1); // fetch attributeName for File 2
    }
    else { // else statement: else if targetOperand does not contain a "." character, prompt an error message (invalid table reference)
        cerr << "ERROR: " << targetOperand << " is an ambiguious parameter. Please try again!" << endl; // error message (reference ambiguity)
        error = true; // error = true
    }

    if (direction == "LEFT") {
        if (error != true) { // if statement: if no error occured (user input contains valid parameters), conduct left outer join algorithm
            if (file) { // if statement: if File 1 / Table 1 exists, attempt to conduct an left outer joined query
                if (file2) { // if statement: if File 2 / Table 2 exists, attempt to conduct an left outer joined query
                    fileCopy.open("copyTemp.txt", ios::out); // fileCopy.open(): create a temp target file that will store updated table data (post-filter)

                    attributeLine_1.clear(); // reset variable value for new parsing retrieval : FILE 1
                    getline(file, attributeLine_1); // read line from file input and store value into attributeLine : FILE 1
                    attributeLine_1.erase(remove(attributeLine_1.begin(), attributeLine_1.end(), '|'), attributeLine_1.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries) : FILE 1

                    attributeLine_2.clear(); // reset variable value for new parsing retrieval : FILE 2
                    getline(file2, attributeLine_2); // read line from file input and store value into attributeLine : FILE 22
                    attributeLine_2.erase(remove(attributeLine_2.begin(), attributeLine_2.end(), '|'), attributeLine_2.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries) : FILE 2

                    stringstream firstSS(attributeLine_1); // stringstream class firstSS: string command treated as stream (parsing -> extraction) : FILE 1
                    stringstream secondSS(attributeLine_2); // stringstream class secondSS: string command treated as stream (parsing -> extraction) : FILE 2

                    string temp_1; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList) : FILE 1
                    string temp_2; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList) : FILE 2
                
                    // FILE 1 PARSING //

                    while (getline(firstSS, temp_1, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                        if (!temp_1.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                            wordList.push_back(temp_1); // push_back(): push current string value into vector (list of words)
                        }
                    }

                    for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attribute metadata: attributeName/attributeType (every other element -> i += 2)
                        attributeList_1.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
                        attributeType_1.push_back(wordList.at(i + 1)); // push_back(): push attributeType value into vector (list of attributeTypes)
                    }

                    // FILE 2 PARSING //

                    wordList.clear(); // wordList.clear(): reset vector instance for File 2 parsing (new retrieval)

                    while (getline(secondSS, temp_2, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                        if (!temp_2.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                            wordList.push_back(temp_2); // push_back(): push current string value into vector (list of words)
                        }
                    }

                    for (size_t i = 0; i < wordList.size(); i += 2) { // for loop: for each parsed attribute data, filter out attribute metadata: attributeName/attributeType (every other element -> i += 2)
                        attributeList_2.push_back(wordList.at(i)); // push_back(): push attributeName value into vector (list of attributes)
                        attributeType_2.push_back(wordList.at(i + 1)); // push_back(): push attributeType value into vector (list of attributeTypes)
                    }

                    // QUERY ANALYSIS //

                    for (size_t i = 0; i < query.size(); i++) { // for loop: for each query option, conduct a filename search to indicate valid query join
                        failCondition = true; // default = true = failedCondition (look for file names, if found, then no fail)

                        if (query.at(i).compare("*") != 0) { // if statement: if any of the query options does not contain a "*" (fetch all) character, conduct a conditional query check for each entry
                            for (size_t j = 0; j < tableNames.size(); j++) { // for loop: for each file/table name arugment passed in the command, conduct a search for pre-determined file/table instances
                                currentFile = tableNames.at(j); // temp string: holds the string value of each file/table 

                                currentFile[0] = toupper(currentFile[0]); // toupper(): capitalize first letter currentFile string to prevent case sensitivity issues

                                for (size_t k = 1; k < currentFile.size(); k++) { // for loop: for each letter in firstFile string (except the first), tolower() to prevent case-sensitivity issues
                                    currentFile[k] = tolower(currentFile[k]); // tolower(): change character case to lower-case for all characters except the first letter 
                                }

                                query.at(i)[0] = toupper(query.at(i)[0]); // toupper: captialize first letter of query.at(i) to prevent case sensitivity issues (first part of the query should be the file name)

                                found = query.at(i).find("."); /// find(): search for a "." character in the query string (representing boundary between reference and attribute)

                                for (int k = 1; k < found; k++) { // for loop: for each character between the first letter and the "." character boundary, tolower() to prevent case sensitivity issues for file names
                                    query.at(i)[k] = tolower(query.at(i)[k]); // tolower(): change character case to lower-case for all characters except the first letter 
                                }

                                found = query.at(i).find(currentFile); // find(): search for parameterized file name within the query string

                                if (found != -1) { // if statement: if fileName found within query string, then query option is valid (failCondition = false = pass)
                                    failCondition = false; // failCondition = false = pass
                                }
                            }

                            if (failCondition == true) { // if statement: if failCondiition == true such that the query option did not contain a file/table reference, prompt an error message
                                cerr << "ERROR: Invalid Query Attribute. " << query.at(i) << " is unavailable!" << endl; // error message (invalid query option)
                                error = true; // error = true
                            }
                        }
                        else { // else statement: else if any of the query options contain a "*" (fetch all) character, ignore conditional check
                            failCondition = false; // failCondition = false = pass
                        }
                    }

                    for (size_t i = 0; i < query.size(); i++) { // for loop: for each query target, check to see if instruction = * (all data)
                        if (query.at(i).compare("*") == 0) { // if statement: query.element == "*" (all info) -> print all relevant table information to the user
                            all = true; // all -> true
                        }
                        else {
                            found = query.at(i).find("."); // find(): search for "." characters in first conditional parameter (attributeSearch)

                            if (found != -1) { // if statement: if attributeSearch contains a "." character, extract attributeName (string after the ".")
                                query.at(i) = query.at(i).substr(found + 1); // fetch attributeName for File 1
                            }
                            else { // else statement: else if attributeSearch does not contain a "." character, prompt an error message (invalid table reference)
                                cerr << "ERROR: " << query.at(i) << " is an ambiguious attribute. Please try again!" << endl; // error message (reference ambiguity)
                                error = true; // error = true
                            }
                        }
                    }

                    if (error != true) { // if statement: if no error occured (query options are not ambigious), continue conducting left outer join algorithm
                        if (all == true) { // if statement: all == true : instruction == * ; therefore, query all attributes
                            query.clear(); // reset vector instance for new parsing retrieval

                            // FILE 1 QUERY (*) //

                            for (size_t i = 0; i < attributeList_1.size(); i++) { // for loop: for each available attribute, push attribute label into query (list of relevant attributes)
                                query.push_back(attributeList_1.at(i)); // push_back(): push current string value into vector (table attribute)
                            }

                            // FILE 2 QUERY (*) //

                            for (size_t i = 0; i < attributeList_2.size(); i++) { // for loop: for each available attribute, push attribute label into query (list of relevant attributes)
                                query.push_back(attributeList_2.at(i)); // push_back(): push current string value into vector (table attribute)
                            }
                        }

                        // ATTRIBUTE VALIDITY CHECK //

                        for (size_t i = 0; i < query.size(); i++) { // for loop: for each given query option, determine attribute validity (is the query attribute in the table?) 
                        
                            // FILE 1 QUERY VALIDITY CHECK //

                            for (size_t j = 0; j < attributeList_1.size(); j++) { // for loop: for each attribute in the table, conduct a comparison to see if given attributes exists in the table (record pos. of attributes to identify attribute values in each record/sample)
                                if (query.at(i) == attributeList_1.at(j)) { // if statement: if target attribute found in list of attributes
                                    attributeFound_1.push_back(j); // record pos. of target attribute (which dimension corresponds with target attribute)
                                }
                            }

                            // FILE 2 QUERY VALIDITY CHECK //

                            for (size_t j = 0; j < attributeList_2.size(); j++) { // for loop: for each attribute in the table, conduct a comparison to see if given attributes exists in the table (record pos. of attributes to identify attribute values in each record/sample)
                                if (query.at(i) == attributeList_2.at(j)) { // if statement: if target attribute found in list of attributes
                                    attributeFound_2.push_back(j); // record pos. of target attribute (which dimension corresponds with target attribute)
                                }
                            }
                        }

                        // ATTRIBUTE VALIDITY ERROR PROMPT //

                        if ((attributeFound_1.size() + attributeFound_2.size()) != query.size()) { // if statement: if one or more target attribute do/does not exist in either table, prompt error message 
                            if (query.size() == 1) { // if statement: if query contains a single target attribute (print invalid error (single))
                                cerr << "ERROR: Invalid Attribute Selection. Please try again!" << endl; // cout confirmation: target attribute(s) do/does not exist in either table (error message)

                                attributeFound_1.clear(); // reset vector instance (error: do not fetch tuple data)
                            }
                            else if (query.size() > 1) { // else if statement: if query contains multiple target attributes (print invalid error (multiple))
                                cerr << "ERROR: One or More Invalid Attribute Selection(s). Please try again!" << endl; // cout confirmation: target attribute(s) do/does not exist in either table (error message)

                                attributeFound_1.clear(); // reset vector instance (error: do not fetch tuple data)
                            }
                        }

                        // FILE 1 ATTRIBUTE LIST + CHECK //

                        for (size_t i = 0; i < attributeList_1.size(); i++) { // for loop: for each attribute in the table, conduct a comparison to see if comparison attribute exists in the table (record pos. of attribute to identify attribute values in each record/sample)
                            if (attributeList_1.at(i) == attributeSearch) { // if statement: if target attribute found in list of attributes
                                comparisonFound_1 = i; // record pos. of target attribute (which dimension corresponds with target attribute)
                            }
                        }

                        if (comparisonFound_1 == -1) { // if statement: if target attribute does not exist in the table, prompt error message 
                            cerr << "ERROR: Invalid Attribute Operand. " << firstFile << " does not include: " << attributeSearch << "." << endl; // error message: target attribute does not exist in the table
                            error = true; // error = true
                        }

                        // FILE 2 ATTRIBUTE LIST + CHECK //

                        for (size_t i = 0; i < attributeList_2.size(); i++) { // for loop: for each attribute in the table, conduct a comparison to see if comparison attribute exists in the table (record pos. of attribute to identify attribute values in each record/sample)
                            if (attributeList_2.at(i) == targetOperand) { // if statement: if target attribute found in list of attributes
                                comparisonFound_2 = i; // record pos. of target attribute (which dimension corresponds with target attribute)
                            }
                        }

                        if (comparisonFound_2 == -1) { // if statement: if target attribute does not exist in the table, prompt error message 
                            cerr << "ERROR: Invalid Attribute Operand. " << secondFile << " does not include: " << targetOperand << "." << endl; // error message: target attribute does not exist in the table
                            error = true; // error = true
                        }

                        // ATTRIBUTE VALUES -> COMPARISON FILTERING //

                        if (error != true) { // if statement: if no error occured (valid attribute comparison statement), continue left outer join algorithm
                            // FILE 1: Fetch attribute values, and choose which values to use in comparison statement //

                            while (!file.eof()) { // while loop: while getline() has not reach the end of the file (EOF), parse each record's/sample's attribute data for filtered comparison
                                firstSS.str(string()); // reset stream for new parsing retrieval
                                attributeLine_1.clear(); // reset variable value for new parsing retrieval
                                wordList.clear(); // reset vector instance for new parsing retrieval

                                getline(file, attributeLine_1); // read line from file input and store value into attributeLine

                                attributeLine_1.erase(remove(attributeLine_1.begin(), attributeLine_1.end(), '|'), attributeLine_1.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
                            
                                stringstream firstFS(attributeLine_1); // stringstream class firstFS: string command treated as stream (parsing -> extraction)
                                string temp_1; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

                                while (getline(firstFS, temp_1, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                                    if (!temp_1.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                                        wordList.push_back(temp_1); // push_back(): push current string value into vector (list of words)
                                    }
                                }

                                for (size_t i = 0; i < wordList.size(); i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute's value(s) of the given record/sample 
                                    attributeValue_1.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of attribute's data)
                                }

                                int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)
                                
                                for (int i = 0; i < listSize; i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute value of the given record/sample 
                                    if (i == comparisonFound_1) { // if statement: if the current index matches the target attribute index, push record's/sample's attribute data into a list of comparison attribute data. 
                                        comparisonValue_1.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of comparison attribute's data)                            
                                    }
                                }               
                            }

                            // FILE 2: Fetch attribute values, and choose which values to use in comparison statement //

                            while (!file2.eof()) { // while loop: while getline() has not reach the end of the file (EOF), parse each record's/sample's attribute data for filtered comparison
                                secondSS.str(string()); // reset stream for new parsing retrieval
                                attributeLine_2.clear(); // reset variable value for new parsing retrieval
                                wordList.clear(); // reset vector instance for new parsing retrieval

                                getline(file2, attributeLine_2); // read line from file input and store value into attributeLine

                                attributeLine_2.erase(remove(attributeLine_2.begin(), attributeLine_2.end(), '|'), attributeLine_2.end()); // str.erase(): remove all '|' characters for clean parsing (no boundaries)
                            
                                stringstream secondFS(attributeLine_2); // stringstream class fs: string command treated as stream (parsing -> extraction)
                                string temp_2; // variable: placeholder string used to extract/parse words from the user command, then used to push it string vector (wordList)

                                while (getline(secondFS, temp_2, ' ')) { // while loop: retrieve's command stream, parses words using " " as a delimiter, then pushes individual words into vector
                                    if (!temp_2.empty()) { // if statement: if the resulting string is not empty (temp != ""), push current string value into word (list) vector
                                        wordList.push_back(temp_2); // push_back(): push current string value into vector (list of words)
                                    }
                                }
                                                
                                for (size_t j = 0; j < wordList.size(); j++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute's value(s) of the given record/sample 
                                        attributeValue_2.push_back(wordList.at(j)); // push_back(): push current string value into vector (list of attribute's data)
                                }

                                int listSize = wordList.size(); // variable: size_t (long unsigned int) -> int (int)
                                
                                for (int i = 0; i < listSize; i++) { // for loop: for each attribute in the table, conduct a pos. comparison (e.g., indexes) to identify the target attribute value of the given record/sample 
                                    if (i == comparisonFound_2) { // if statement: if the current index matches the target attribute index, push record's/sample's attribute data into a list of comparison attribute data. 
                                        comparisonValue_2.push_back(wordList.at(i)); // push_back(): push current string value into vector (list of comparison attribute's data)
                                    }
                                }                  
                            }

                            // ATTRIBUTE COMPARISON (MULTIPLE FILES/TABLES) //

                            for (size_t i = 0; i < comparisonValue_1.size(); i++) { // for loop: FILE 1 :for each record's/sample's attribute data, conduct 'on' comparison. If attribute value passes comparison, record its corresponding row for selection (filter).
                                for (size_t j = 0; j < comparisonValue_2.size(); j++) { // for loop: FILE 2 :for each record's/sample's attribute data, conduct 'on' comparison. If attribute value passes comparison, record its corresponding row for selection (filter).
                                    if (isNumber(comparisonValue_1.at(i)) && isNumber(comparisonValue_2.at(j))) { // if statement: if the values being compared (i.e., attributeSearch[x], targetOperand[x], etc.) are numbers [isNumber() helper function], conduct numerical arithmetic (e.g., >, <, >=, <=, and =)
                                        float operand_1 = 0; // variable: left-side operand (float -> scenarios that involve decimal values (i.e., pricing, income, etc.))
                                        float operand_2 = 0; // variable: right-side operand (float -> scenarios that involve decimal values ...)
                                        
                                        stringstream op1(comparisonValue_1.at(i)); // stringstream class op1: string command treated as stream (extraction) -> string to float conversion
                                        stringstream op2(comparisonValue_2.at(j)); // stringstream class op2: string command treated as stream (extraction) -> string to float conversion

                                        op1 >> operand_1; // string to float conversion (left-side)
                                        op2 >> operand_2; // string to float conversion (right-side)

                                        if (condition == ">") { // if statement: if the condition operation == >, conduct a greater than comparison
                                            if (operand_1 > operand_2) { // if statement: if operand_1 > operand_2
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                                targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                            }
                                        }
                                        else if (condition == "<") {  // else if statement: if the condition operation == <, conduct a less than comparison
                                            if (operand_1 < operand_2) { // if statement: if operand_1 < operand_2
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                                targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                            }
                                        }
                                        else if (condition == "<=") { // else if statement: if the condition operation == <=, conduct a less than or equal to comparison
                                            if (operand_1 <= operand_2) { // if statement: if operand_1 <= operand_2
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                                targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                            }
                                        }
                                        else if (condition == ">=") { // else if statement: if the condition operation == <=, conduct a greater than or equal to comparison
                                            if (operand_1 >= operand_2) { // if statement: if operand_1 >= operand_2
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                                targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                            }
                                        }
                                        else if (condition == "=") { // else if statement: if the condition operation == =, conduct an equal comparison
                                            if (operand_1 == operand_2) { // if statement: if operand_1 == operand_2
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                                targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                            }                    
                                        }
                                        else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a not equal comparison
                                            if (operand_1 != operand_2) { // if statement: if operand_1 != operand_2
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 1
                                                targetTuple_2.push_back(j + 1); // push.back(): push current row pos. into vector (list of tuples to query) : FILE 2
                                            }
                                        }
                                        else { // else statement: if the condition operation is invalid (none of the aforementioned cases), prompt error message
                                            cerr << "ERROR: Invalid Numerical Condition. Please try one of the following: >, <, >=, <=, =" << endl; // error message: Invalid Numerical Condition (not a comparison between numbers)
                                            break;
                                        }   

                                        op1.str(string()); // reset stream for new parsing retrieval [operand_1 is the only operand changing with each comparison (different record/sample data)]
                                    }
                                    else if (!isNumber(comparisonValue_1.at(i)) && !isNumber(comparisonValue_2.at(j))) { // else statement: else if values being compared (i.e., attributeSearch[x], targetOperand[x], etc.) are not numbers, then they classify as string(s)/object(s) and can not support Numerical Comparions (only ==)
                                        if ((condition == "=") || (condition == "==")) { // if statement: if the condition operation == = || ==, conduct a string comparison [are the strings the same (i.e., filter by name, tile, etc.)]
                                            if (comparisonValue_1.at(i) == comparisonValue_2.at(j)) { // if statement: if the record's/sample's current string value matches the value of the target operand, fetch record/sample data 
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                                                targetTuple_2.push_back(j + 1);
                                            }
                                        }
                                        else if (condition == "!=") { // else if statement: if the condition operation == !=, conduct a string NOT comparison 
                                            if (comparisonValue_1.at(i) != comparisonValue_2.at(j))  { // if statement: if the record's/sample's current string value does not match the value of the target operand, fetch record/sample data
                                                targetTuple_1.push_back(i + 1); // push.back(): push current row pos. into vector (list of tuples to query)
                                                targetTuple_2.push_back(j + 1);
                                            }
                                        }
                                        else { // else statement: if the condition is not a string comparison, prompt error message 
                                            cerr << "ERROR: Invalid String Condition. Please try one of the following: =, ==" << endl; // error message: Invalid String Condition (not a comparison between strings)
                                            break;
                                        }    
                                    }
                                    else { // else statement: else if one of the values is a number, but the other value is a non-number, then prompt an error message (Invalid DataType Operation)
                                        cerr << "ERROR: Invalid Conditional Statement. DataType Violation!" << endl; // error message: Invalid DataType Operation
                                        break;
                                    }
                                }
                            }

                            if (targetTuple_1.empty() || targetTuple_2.empty()) { // if statement: if either targetTuple (list of tuples to query) is empty, prompt an error message (cannot conduct a JOIN without successful filtering/conditions)
                                cerr << "ERROR: Invalid Query. Cannnot find tuples that pass the following condition: " << attributeSearch << " " << condition << " " << targetOperand << endl; // error message (failed conditional statement)
                                error = true; // boolean: error = true -> failsafe
                            }

                            if (error != true) { // if statement: if no error occured (successful filtered query), fetch relevant tuple data by creating/reading a temp copy
                                
                                // EXPORT OUTER JOIN HEADER (COMBINE FILE HEADER(S)) //
                                
                                for (size_t i = 0; i < query.size(); i++) { // for loop: for each query option, determine which file the indicated attribute belongs to and export complete left outer join header (ORDER MATTERS BASED ON QUERY ENTRY)
                                    for (size_t j = 0; j < attributeList_1.size(); j++) { // for loop: for each attribute in FILE 1, conduct query option comparison (does it exist in FILE 1?)
                                        if (query.at(i) == attributeList_1.at(j)) { // if statement: if query option exists in FILE 1 attribute list, export using proper formatting (single | multiple)
                                            if (i == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                fileCopy << attributeList_1[j] << " " << attributeType_1[j]; // export table attribute (single)
                                            }
                                            else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                fileCopy << attributeList_1[j] << " " << attributeType_1[j] << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                            }
                                        }
                                    }

                                    for (size_t j = 0; j < attributeList_2.size(); j++) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                        if (query.at(i) == attributeList_2.at(j)) { // if statement: if query option exists in FILE 2 attribute list, export using proper formatting (single | multiple)
                                            if (i == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                fileCopy << attributeList_2[j] << " " << attributeType_2[j]; // export table attribute (single)
                                            }
                                            else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                fileCopy << attributeList_2[j] << " " << attributeType_2[j] << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                            }
                                        }
                                    }
                                }

                                // EXPORT LEFT OUTER JOIN TUPLE DATA (INTERSECTION + LEFT SIDE ) // 
                                
                                for (size_t i = 0; i < attributeValue_1.size(); i += attributeFound_1.size()) { // for loop: for each record's/sample's (valid) attribute value, export relevant table data to a temp target file for proper getline() reading [attributeValue format: queryNum -> 3 attr. : attributeValue[0] = attr.1, attributeValue[1] = attr.2, attributeValue[2] = attr. 3, attributeValue[4] = attr. 1, attributeValue[5] = attr.2, etc.]                    
                                    numIterations = 0; // reset numIterations for each record/sample 
                                    match = false; // boolean: match = false -> default (conditions)

                                    matchingTuple.clear(); // reset vector instance for new matchingTuple index retrieval

                                    for (size_t j = 0; j < targetTuple_1.size(); j++) { // for loop: for each recorded tuple index (pending selection), compare with current lineIndex to identify if data is exported to target file (updated table)                                    
                                        if (lineIndex == targetTuple_1.at(j)) { // if statement: if current lineIndex == any(targetTuple.list)
                                            match = true; // change boolean status (true) -> export record's/sample's attribute data for reading
                                            
                                            matchingTuple.push_back(targetTuple_2.at(j)); // fetch matchingTuple index based on FILE 1 condition

                                            numIterations++; // numIterations increment (total number of matching record(s)/sample(s))
                                        }
                                    }

                                    if (match == true) { // if statement: if match == true -> if current tuple data contains intersections, export intersection data to temp copy
                                        for (int j = 0; j < numIterations; j++) { // for loop: for each matching record/sample (multiple iterations = repeated record(s)/sample(s))
                                            fileCopy << endl; // export '\n' (new line) character : seperation between rows of data

                                            for (size_t k = 0; k < query.size(); k++) { // for loop: for each query option, determine which file the indicated attribute belongs to, then export the appropriate data (attributeName -> attributeValue)
                                                for (size_t l = 0; l < attributeList_1.size(); l++) { // for loop: for each attribute in FILE 1, conduct query option comparison (does it exist in FILE 1?)
                                                    if (query.at(k) == attributeList_1.at(l)) { // if statement: if query option exists in FILE 1 attribute list, export the corresponding attribute value w/ proper formatting (single | multiple)
                                                        if (k == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                            fileCopy << attributeValue_1.at(l + ((lineIndex - 1) * attributeList_1.size())); // export table attribute (single)
                                                        }
                                                        else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                            fileCopy << attributeValue_1.at(l + ((lineIndex - 1) * attributeList_1.size())) << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                                        }
                                                    }
                                                }

                                                for (size_t l = 0; l < attributeList_2.size(); l++) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                                    if (query.at(k) == attributeList_2.at(l)) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                                        if (k == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                            fileCopy << attributeValue_2.at(l + ((matchingTuple.at(j) - 1) * attributeList_2.size())); // export table attribute (single)
                                                        }
                                                        else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                            fileCopy << attributeValue_2.at(l + ((matchingTuple.at(j) - 1) * attributeList_2.size())) << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else { // else statement: else if match === false -> if current tuple data does not contain intersections, export left-side data to temp copy
                                        fileCopy << endl; // export '\n' (new line) character : seperation between rows of data

                                        for (size_t k = 0; k < query.size(); k++) { // for loop: for each query option, determine which file the indicated attribute belongs to, then export the appropriate data (attributeName -> attributeValue)
                                            for (size_t l = 0; l < attributeList_1.size(); l++) { // for loop: for each attribute in FILE 1, conduct query option comparison (does it exist in FILE 1?)
                                                if (query.at(k) == attributeList_1.at(l)) { // if statement: if query option exists in FILE 1 attribute list, export the corresponding attribute value w/ proper formatting (single | multiple)
                                                    if (k == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                        fileCopy << attributeValue_1.at(l + ((lineIndex - 1) * attributeList_1.size())); // export table attribute (single)
                                                    }
                                                    else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                        fileCopy << attributeValue_1.at(l + ((lineIndex - 1) * attributeList_1.size())) << setw(5) << "|" << setw(6); // export table attributes (multiple)
                                                    }
                                                }
                                            }

                                            for (size_t l = 0; l < attributeList_2.size(); l++) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                                if (query.at(k) == attributeList_2.at(l)) { // for loop: for each attribute in FILE 2, conduct query option comparison (does it exist in FILE 2?)
                                                    if (k == query.size() - 1) { // if statement: if there is only 1 attributeValue, export the single attribute data (no boundaries)
                                                        fileCopy << "NULL"; // export table attribute (single) : INITIALIZE TO NULL
                                                    }
                                                    else { // else if statement: else if there is only multiple attributeValue(s), export the multiple instances of attribute data (seperated by boundaries)
                                                        fileCopy << "NULL" << setw(5) << "|" << setw(6); // export table attributes (multiple) : initialize TO NULL
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    lineIndex++; // lineIndex increment (represents next tuple data index/row)
                                }

                                fileCopy.close(); // fileCopy.close(): close file from write-only

                                fileCopy.open("copyTemp.txt", ios::in); // fileCopy.open(): attempt to open file in read-only (getline() fetch)

                                while (getline(fileCopy, line)) { // while loop: while getline() continues to extract file data line-by-line, print to user
                                    cout << line << endl; // cout table data
                                }
                            }
                        }
                    }

                    file.close(); // file.close(): close file from read-only
                    file2.close(); // file.close(): close file from read-only
                    fileCopy.close(); // fileCopy.close(): close file from read-only

                    remove("copyTemp.txt"); // after displaying relevant data, remove/delete temp target
                }
                else { // else statement: else if FILE 2 / TABLE 2 does not exist, do not fetch a query (prompt error message)
                    cerr << "!Failed to query table " << secondFile << " because it does not exist." << endl; // error message (FILE 2 does not exist)
                }
            }
            else { // else statement: else if FILE 1 / TABLE 1 does not exist, do not fetch a query (prompt error message)
                cerr << "!Failed to query table " << firstFile << " because it does not exist." << endl; // error message (FILE 1 does not exist)
            }    
        }        
    }
    else if (direction == "RIGHT") {
        cerr << "ERROR: RIGHT OUTER JOIN has not been implemented. Please try again later!" << endl;
    }
    else if (direction == "FULL") {
        cerr << "ERROR: FULL OUTER JOIN has not been implemented. Please try again later!" << endl;
    }
    else {
        cerr << "ERROR: Invalid Outer Join Direction. Please try one of the following: LEFT | RIGHT | FULL" << endl; 
    }
}

bool isNumber(const string& s) { // isNumber(): searches for non-numerical characters in the given string to determine if value classifies as a real number
    return s.find_first_not_of("(0123456789.)") == string::npos; // return boolean value (true | false) -> does string contain more than numerical characters [i.e., 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, . (decimal point), (, )]
}
