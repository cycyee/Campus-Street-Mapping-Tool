# Project 4 README

Student 1: calvin yee (919768747)

Student 2: Qian Shan (919260012)

## Project Status
Project Complete, all written tests pass. 

## Known Issues
Have tested everything we think the project will fail for.

## Code References
https://stackoverflow.com/questions/32685540/why-cant-i-compile-an-unordered-map-with-a-pair-as-key --help figure out can't compile an unordered_map with a pair as key
https://www.freecodecamp.org/news/dijkstras-shortest-path-algorithm-visual-introduction/#:~:text=Dijkstra's%20Algorithm%20finds%20the%20shortest,node%20and%20all%20other%20nodes. -- information about dijkstra algorithm
https://www.freecodecamp.org/news/dijkstras-shortest-path-algorithm-visual-introduction/#:~:text=Dijkstra's%20Algorithm%20finds%20the%20shortest,node%20and%20all%20other%20nodes. -- basic information about dijkstra algorithm in cpp

## Generative AI Use

### Prompt 1
what is dijkstra algorithm, and give me some examples
### Response 1
Dijkstra's algorithm is a popular and efficient algorithm used to find the shortest paths from a single source vertex to all other vertices in a graph with non-negative edge weights. It's widely used in various applications, from networking routing protocols to mapping and navigation services. Here's how it fundamentally operates:

Initialization: Mark the distance to the source vertex as zero and to all other vertices as infinity. Set the initial node as current.

Visiting Neighbors: Update the tentative distance to every neighbor of the current node by summing the distance from the source to the current node and from the current node to the neighbor. If this distance is less than the previously recorded distance, update the neighbor's distance.

Mark Visited: Once all neighbors have been visited, mark the current node as visited. A visited node will not be checked again.

Select Unvisited Node with the Smallest Distance: Choose the unvisited node that is marked with the smallest tentative distance, set it as the new "current node," and go back to step 2.

Repeat: Continue the process until all nodes have been visited. The algorithm ends when the shortest paths from the source to all other nodes have been established.

Examples
Here are some practical examples of where Dijkstra's algorithm is used:

Network Routing: In computer networks, Dijkstra's algorithm is used to find the shortest path for data packet routing from the source to the destination node, minimizing the time taken for data transmission.

Mapping Services: Applications like Google Maps and Waze use Dijkstra's algorithm (or variants of it) to find the shortest route from one location to another over a map, taking into account various factors such as distance, traffic conditions, and road quality.

Video Game Pathfinding: In video games, Dijkstra's algorithm is often used for non-player character (NPC) movement, helping them navigate the game world efficiently from point A to B, avoiding obstacles.

Robotics: Robots use Dijkstra's algorithm for navigation in an environment, finding the shortest, safest, or most efficient path to move from their current location to the target location.

Telecommunication Networks: In managing and optimizing the flow of signals through the telecommunication networks, Dijkstra's algorithm helps in finding the shortest path for signal transmission to ensure efficient communication.

These examples highlight the versatility and utility of Dijkstra's algorithm in solving various real-world problems involving the optimization of paths or routes.
### Changes 1
N/A get the basic ideas about dijkstra

### Prompt 2
explain Dijkstra's algorithm using a minimum heap
### Response 2
Dijkstra's algorithm is a popular algorithm for finding the shortest path between nodes in a graph, particularly when all edge weights are non-negative. The algorithm iteratively explores nodes in the graph, starting from the source node and gradually expanding to neighboring nodes with the smallest known distance.

Here's how Dijkstra's algorithm works using a minimum heap:

Initialization: Start with a priority queue (min heap) to keep track of the nodes to be explored, initialized with the source node and its distance (set to 0). Also, initialize a distance array to keep track of the shortest distance from the source node to each node in the graph.

Main Loop: While the priority queue is not empty, do the following:

Extract the node with the smallest known distance from the priority queue. Let's call this node current.
For each neighboring node neighbor of current, do the following:
Calculate the distance from the source node to neighbor through current.
If this calculated distance is less than the previously known distance to neighbor, update the distance to neighbor and update neighbor's entry in the priority queue with the new distance.
Termination: Repeat step 2 until the priority queue is empty, which means all reachable nodes have been visited and their shortest distances from the source have been computed.

Result: After the algorithm terminates, the distance array contains the shortest distance from the source node to every other node in the graph.

Using a minimum heap (priority queue) speeds up the process of finding the node with the smallest known distance efficiently. The priority queue ensures that the node with the smallest distance is always selected for further exploration, reducing the overall time complexity of the algorithm.

The time complexity of Dijkstra's algorithm with a minimum heap implementation is O((V + E) * log(V)), where V is the number of vertices and E is the number of edges in the graph

### Changes 2
N/A get the basic ideas about dijkstra in cpp

### Prompt 3
this is my error code: which part I should focus on to fix my code?
[ RUN      ] XMLReaderTest.SpecialCharacterTest
./testsrc/XMLTest.cpp:186: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "&"
  "&\"'<>"
./testsrc/XMLTest.cpp:190: Failure
Expected equality of these values:
  Entity.DType
    Which is: 4-byte object <02-00 00-00>
  SXMLEntity::EType::EndElement
    Which is: 4-byte object <01-00 00-00>
./testsrc/XMLTest.cpp:191: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "\""
  "elem"
./testsrc/XMLTest.cpp:194: Failure
Value of: Reader.End()
  Actual: false
Expected: true
[  FAILED  ] XMLReaderTest.SpecialCharacterTest (0 ms)
### Response 3
The error messages you're encountering in your test output suggest there are a few specific areas within your XML parsing code that you should focus on for debugging:

Special Character Handling (./testsrc/XMLTest.cpp:186):

The test expects Entity.DNameData to equal "&\"'<>", which is a string containing special characters often used in XML for encoding. However, Entity.DNameData actually contains only "&". This suggests there might be an issue with how your XML parser handles or decodes special characters.
Entity Type Mismatch (./testsrc/XMLTest.cpp:190):

The test expects Entity.DType to be SXMLEntity::EType::EndElement, represented by the byte object <01-00 00-00>, but it finds a different type, represented by <02-00 00-00>. This indicates a problem with how the entity type is being identified or assigned in your code. It seems your parser might be misclassifying XML elements or perhaps mishandling the transitions between start elements, end elements, or other entity types.
Incorrect Name Data for an Entity (./testsrc/XMLTest.cpp:191):

Here, the test expects Entity.DNameData to be "elem", indicating the name of an element, but it instead finds Entity.DNameData to be "\"". This discrepancy suggests that there's an issue with how your parser is extracting or assigning names to entities, particularly around parsing element names or handling quotes.
Incorrect End of Document Handling (./testsrc/XMLTest.cpp:194):

The test expects that Reader.End() will return true, indicating the end of the XML document has been successfully reached. However, it returns false. This failure could be related to the above issues, indicating that the parser might not be correctly interpreting the structure of the XML, leading to an incorrect assessment of the document's end.
To fix these issues, you should:

Review Special Character Decoding: Ensure your parser correctly decodes special characters and entities within XML. It should properly translate entity references to their corresponding characters.
Check Entity Type Assignment: Verify that your code correctly identifies and assigns the correct entity types (StartElement, EndElement, etc.) based on the XML structure.
Examine Element Name Parsing: Ensure that your parser correctly handles the extraction and assignment of element names, paying particular attention to the handling of quotes and special characters.
Validate Document Structure Parsing: Ensure that your parser correctly identifies the start and end of the document, and that it can accurately traverse and interpret the XML structure.
Debugging each of these areas by isolating test cases and carefully reviewing the relevant portions of your code should help you address the failures and improve the accuracy of your XML parser.
### Changes 3
make change in XMLReader.cpp and pass the test
form:   if (!cdata.empty()) { 
            SXMLEntity entity;
            entity.DNameData = cdata; // give the character data
            entity.DType = SXMLEntity::EType::CharData; //Mark as character data
            DEntityQueue.push(entity);
        }
to:     if (!cdata.empty()) {
            if (!DEntityQueue.empty() && DEntityQueue.back().DType == SXMLEntity::EType::CharData) {
            // Concatenate CDATA with the last entity if it is also CharData
            DEntityQueue.back().DNameData += cdata;
            } else {
                SXMLEntity entity;
                entity.DNameData = cdata;
                entity.DType = SXMLEntity::EType::CharData;
                DEntityQueue.push(entity);
            }
        }

### Prompt 4
Why can't I compile an unordered_map with a pair as key?
### Response 4
When using std::pair (or any other custom type) as the key in an std::unordered_map, you encounter issues because the unordered map requires a hash function for its keys to organize them in the underlying hash table. By default, C++ standard library provides hash functions for basic types like int, double, string, etc., but it does not provide a hash function for user-defined types, including std::pair.

To use std::pair as a key in an std::unordered_map, you must define a custom hash function or specialize the std::hash template for your specific key type. Here's a basic example of how you can achieve this:

cpp
Copy code
#include <iostream>
#include <unordered_map>
#include <utility>

// Define a custom hash function for std::pair<int, int>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2; // Combine the two hash values. You might want to use a better combine function.
    }
};

int main() {
    // Use std::pair<int, int> as key for std::unordered_map
    std::unordered_map<std::pair<int, int>, std::string, pair_hash> map;
    map[std::make_pair(1, 2)] = "Example";
    
    std::cout << map[std::make_pair(1, 2)] << std::endl; // Outputs: Example
    
    return 0;
}
This example defines a pair_hash struct that provides a hash function for pairs. This hash function combines the hashes of the two elements of the pair. Note that the way of combining hash values (hash1 ^ hash2 in this example) is straightforward but may lead to hash collisions. You might want to use a more sophisticated method to combine hash values for better distribution, especially if your application is sensitive to hash collisions.

By defining a suitable hash function, you allow std::unordered_map to use std::pair as a key, thereby resolving the compilation error.
### Changes 4
fix the problem in bussystemindexer
    struct pair_hash{
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1,T2> &p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            // Mainly for demonstration purposes, i.e. works but is overly simple
            // In the real world, use sth. like boost.hash_combine
            return h1 ^ h2;  
        }
    };

### Prompt 5
What is a good way or is there any way to get all neighbors of a node?
### Response 5
To get all neighbors of a node in a graph, the approach you take can vary depending on how your graph is represented. The most common representations of graphs are adjacency matrices, adjacency lists, and edge lists. Below, I'll outline methods for each representation.

Adjacency List
An adjacency list represents a graph as an array of lists. The index of the array represents a vertex and each element in its list represents the other vertices that are its neighbors.

Advantages: Space-efficient for sparse graphs, easy to iterate over all neighbors of a vertex.

How to get neighbors: Access the list at the index of the node. This list directly contains all the neighbors.

cpp
Copy code
#include <vector>
#include <iostream>

int main() {
    // Graph represented as an adjacency list
    // (assuming an undirected graph for simplicity)
    std::vector<std::vector<int>> graph = {
        {1, 2},    // Neighbors of vertex 0
        {0, 3, 4}, // Neighbors of vertex 1
        {0, 4},    // Neighbors of vertex 2
        {1},       // Neighbors of vertex 3
        {1, 2}     // Neighbors of vertex 4
    };

    int node = 1; // Node to get neighbors for

    std::cout << "Neighbors of node " << node << ": ";
    for(int neighbor : graph[node]) {
        std::cout << neighbor << " ";
    }
    std::cout << std::endl;

    return 0;
}
Adjacency Matrix
An adjacency matrix is a 2D array where the element at [i][j] is 1 (or any other non-zero value to represent weight in weighted graphs) if there is an edge from vertex i to vertex j, and 0 otherwise.

Advantages: Easy to check if a particular edge exists between any two vertices, straightforward implementation.

How to get neighbors: Iterate through the row corresponding to the node. Any column with a non-zero value at that row indicates a neighbor.

cpp
Copy code
#include <iostream>
#include <vector>

int main() {
    // Graph represented as an adjacency matrix
    std::vector<std::vector<int>> matrix = {
        {0, 1, 1, 0, 0}, // Connections for vertex 0
        {1, 0, 0, 1, 1}, // Connections for vertex 1
        {1, 0, 0, 0, 1}, // Connections for vertex 2
        {0, 1, 0, 0, 0}, // Connections for vertex 3
        {0, 1, 1, 0, 0}  // Connections for vertex 4
    };

    int node = 1; // Node to get neighbors for

    std::cout << "Neighbors of node " << node << ": ";
    for(int j = 0; j < matrix[node].size(); ++j) {
        if(matrix[node][j] == 1) {
            std::cout << j << " ";
        }
    }
    std::cout << std::endl;

    return 0;
}
Edge List
An edge list represents a graph as a list where each element is a pair (or tuple for weighted graphs) representing an edge connecting two vertices.

Advantages: Space-efficient for very sparse graphs, simple representation.

How to get neighbors: Iterate through the entire list, checking if the current edge involves the node of interest.

cpp
Copy code
#include <vector>
#include <iostream>

int main() {
    // Graph represented as an edge list
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 4}
    };

    int node = 1; // Node to find neighbors for

    std::cout << "Neighbors of node " << node << ": ";
    for(auto& edge : edges) {
        if(edge.first == node) {
            std::cout << edge.second << " ";
        } else if(edge.second == node) {
            std::cout << edge.first << " ";
        }
    }
    std::cout << std::endl;

    return 0;
}
Each of these methods has its own use cases depending on the specific needs of your application, such as whether the graph is directed or undirected, sparse or dense, and the operations you need to perform most frequently.
### Changes 5
N/A get the idea about how to deal with getting neighbor.

### Prompt 6
How mock method  in google test work
### Response 6

Google Test is a widely used C++ testing framework, and mocking is an essential technique in unit testing for isolating the system under test from its dependencies. Mocking methods in Google Test is primarily facilitated by Google Mock, a library for writing and using C++ mock classes. Google Mock is now part of the Google Test project, so when you use Google Test, you also get Google Mock's capabilities.

Mocking methods with Google Mock involves a few key steps:

Define a Mock Class: You create a mock class that mimics the interface of the class whose methods you want to mock. This is done using Google Mock's MOCK_METHOD macro, where you specify the method name, its return type, its arguments, and (optionally) its call type (e.g., Calltype::TYPED, Calltype::OVERLOADED).

Example:

cpp
Copy code
#include <gmock/gmock.h>

class MyClass {
public:
    virtual ~MyClass() = default;
    virtual int SomeMethod(int value) = 0;
};

class MockMyClass : public MyClass {
public:
    MOCK_METHOD(int, SomeMethod, (int value), (override));
};
Specify Expectations and Behaviors: Once you have a mock class, you can specify what should happen when its methods are called. This involves setting expectations (e.g., how many times a method should be called) and defining the behavior (e.g., what it should return) using Google Mock's EXPECT_CALL macro and actions like Return, Throw, etc.

Example:

cpp
Copy code
MockMyClass mockObj;
EXPECT_CALL(mockObj, SomeMethod(testing::_))
    .WillOnce(testing::Return(42));
Use the Mock in Tests: Finally, you use the mock object in your tests instead of the real object. This allows you to test the system under test in isolation, controlling and verifying interactions with its dependencies.

Example:

cpp
Copy code
TEST(MyTest, TestSomething) {
    MockMyClass mockObj;
    EXPECT_CALL(mockObj, SomeMethod(testing::_))
        .WillOnce(testing::Return(42));

    // Your test code here, using mockObj
}
Compile and Link Your Test: To use Google Mock, ensure your build system compiles and links your test code with both Google Test and Google Mock libraries. This often involves adding the appropriate include paths and linking against the gtest and gmock libraries.

In summary, mocking methods in Google Test with Google Mock involves defining mock classes using the MOCK_METHOD macro, setting expectations and behaviors with EXPECT_CALL and action macros, and then using these mock objects in your unit tests to ensure your system under test behaves correctly in a controlled environment.
### Changes 6
N/A, get the idea how mock_method work

### Prompt 7
how to make sure my program handling the command line will not stop until user types exit
### Response 7

To create a program that continuously handles command line input without stopping until the user types "exit", you can implement a loop that keeps running until a specific exit condition is met. In C++, this could be achieved by repeatedly reading input from the user and then processing it, breaking the loop only when the input matches the exit condition. Here's a simple example to illustrate how you might implement this:

cpp
Copy code
#include <iostream>
#include <string>

int main() {
    std::string userInput;

    // Loop indefinitely until the user types "exit"
    while (true) {
        std::cout << "Enter command (type 'exit' to quit): ";
        
        // Get the entire line of input from the user
        std::getline(std::cin, userInput);
        
        // Check if the input is the exit command
        if (userInput == "exit") {
            break; // Exit the loop
        }

        // Process the input here
        // For demonstration, we'll just echo the input
        std::cout << "You entered: " << userInput << std::endl;
    }

    std::cout << "Program terminated." << std::endl;

    return 0;
}
This program works as follows:

It enters an infinite loop (while (true)), continuously asking the user for input.
It uses std::getline(std::cin, userInput) to read the entire line of input into a std::string named userInput. This allows for handling inputs with spaces.
It checks if userInput matches the string "exit". If so, it breaks out of the loop using break, effectively ending the program's main loop.
If the input is not "exit", the program proceeds to process the input. In this example, processing is simply echoing the input back to the user.
Once the loop is exited, the program prints "Program terminated." and ends.
This pattern allows your program to continuously accept and process user input until the user explicitly decides to exit the program by typing "exit". You can replace the input processing part with whatever functionality your program requires.
### Changes 7
N/A

### Prompt 8
normally how can I manipulate with std::move
### Response 8
In C++, std::move is a standard library function that converts its argument into an rvalue reference. This allows the object to be moved rather than copied, which can be more efficient, especially for large objects. Using std::move is a way to enable move semantics on objects that would otherwise be copied. Here's how you can use it:

Enabling Move Semantics: For a class to benefit from move semantics, it needs to have a move constructor and/or a move assignment operator. These special member functions take an rvalue reference to an object of the same type and move the resources from the given object to the current object, leaving the moved-from object in a valid but unspecified state.

Using std::move: You can use std::move to cast an lvalue (an object that has a name) to an rvalue reference, thus enabling the move semantics. This is particularly useful when you know an object won't be used again and you want to avoid the cost of copying it.

Basic Usage Example
Here is a simple example of using std::move:

cpp
Copy code
#include <utility>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> vec1 = {1, 2, 3, 4, 5};
    
    // Use std::move to enable moving vec1 into vec2. vec1 is empty after this operation.
    std::vector<int> vec2 = std::move(vec1);
    
    if(vec1.empty()) {
        std::cout << "vec1 is empty after move" << std::endl;
    }
    
    std::cout << "vec2 contains: ";
    for(int i : vec2) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
Points to Remember
Efficiency: Moving is generally more efficient than copying, especially for objects that manage resources like memory, because it transfers the ownership of those resources to a new object rather than duplicating them.

Object State After Move: The state of the moved-from object is valid but unspecified. This means you can use or assign a new value to it, but you shouldn't rely on its old value.

When to Use std::move: Use std::move when you want to transfer resources from one object to another and the source object will not be used again in a context where its value matters.

Remember, misuse of std::move can lead to performance degradation or bugs due to the moved-from object being in an unspecified state. Always ensure that a moved-from object either is not used again or is only used in a manner that does not depend on its previous state.
### Changes 8
CmdSrc(std::move(cmdsrc)), OutSink(std::move(outsink)), ErrSink(std::move(errsink)), Results(std::move(results)), Planner(std::move(planner))

### Prompt 9
explain differences between std::cin, std::cout, std::cerr
### Response 9
std::cin, std::cout, and std::cerr are all part of the C++ Standard Library's iostream library, which provides facilities for input and output (I/O) operations. Each serves a different purpose in the context of I/O operations:

std::cin
Usage: Used for input from the standard input device, which is usually the keyboard.
Type: It is an object of istream class.
Functionality: It is used to read data from the standard input stream.
Example Usage: std::cin >> variable; This reads input from the user and stores it in variable.
std::cout
Usage: Used for output to the standard output device, which is usually the console or terminal.
Type: It is an object of ostream class.
Functionality: It is used to write data to the standard output stream.
Example Usage: std::cout << "Hello, World!" << std::endl; This prints "Hello, World!" followed by a newline to the standard output.
std::cerr
Usage: Used for output to the standard error device, which is also usually the console or terminal but can be redirected separately from std::cout.
Type: It is an object of ostream class, similar to std::cout.
Functionality: It is used to write error messages or diagnostics to the standard error stream. The key characteristic of std::cerr is that it is unbuffered by default, which means it writes directly to the error device immediately, ensuring that error messages are displayed as soon as possible.
Example Usage: std::cerr << "Error occurred!" << std::endl; This prints an error message to the standard error.
Differences
Buffering:
std::cout is buffered, which means output might be delayed until the buffer is flushed (e.g., when std::endl is used).
std::cerr is unbuffered, meaning output appears immediately without waiting for the buffer to fill. This is critical for error messages that need to be shown promptly.
Purpose:
std::cin is solely for input.
std::cout is used for general output.
std::cerr is specifically for error messages and diagnostics.
Stream:
std::cin interacts with the standard input stream.
std::cout and std::cerr interact with the standard output and standard error streams, respectively, and can be redirected separately in most operating systems.
These distinctions help ensure that input, output, and error handling are managed effectively and efficiently in C++ programs.
### Change 9
get the idea about them, help for writing transplanner.cpp