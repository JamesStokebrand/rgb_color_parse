/* This file defines tests for testing the rgb_node.h file.
*/

#include "include/rgb_node.h"

ostream& operator<<(ostream &out, const rgb_node &A)
{
    out << A.name << " "
        << A.red << " "
        << A.green << " "
        << A.blue;
    return out;
}

#if 0
// Test code
int main(int argc, char *argv[])
{

    rgb_node aNode, bNode;

    cout << "A" << aNode << endl;
    cout << "B" << aNode << endl;

    aNode.set_red(2.0);
    cout << "A" << aNode << endl;
    aNode.set_red(-1.0);
    cout << "A" << aNode << endl;
    aNode.set_red(1.0);
    cout << "A" << aNode << endl;

    bNode.set_blue(1.0);
    cout << "B" << bNode << endl;

    if (aNode == bNode)
    {
        cout << "Nodes are equal" << endl;
    }
    else 
    {
        cout << "Nodes are NOT equal" << endl;
    }

    bNode = aNode;
    if (aNode == bNode)
    {
        cout << "Nodes are equal" << endl;
    }
    else
    {
        cout << "Nodes are NOT equal" << endl;
    }


    return 0; // Success
}

#endif

