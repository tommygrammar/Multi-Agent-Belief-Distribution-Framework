#include <iostream>
#include "headers/System.h"

int main()
{

    System sys;

    Section* A = sys.createSection('A', 0.9, 0.1, 2);
    Section* B = sys.createSection('B', 0.5, 0.5, 3);
    Section* C = sys.createSection('C', 0.7, 0.3, 1);
    Section* D = sys.createSection('D', 0.2, 0.8, 5);

    A->addNeighbor(B);
    A->addNeighbor(C);

    B->addNeighbor(D);

    C->addNeighbor(D);

    Agent* alice = sys.createAgent('Alice');
    Agent* bob = sys.createAgent('Bob');

    alice->updateSection('A', 0.9, 0.1, 2);
    alice->updateSection('B', 0.7, 0.3, 3);

    bob->updateSection('C', 0.8, 0.2, 1);
    bob->updateSection('D', 0.6, 0.4, 4);

    alice->shareBelief(bob, 'B');

    auto path = alice->shortestCertainPath(A, D);

    std::cout << "\nChosen path:\n";

    for (auto* s : path)
        std::cout << s->id << " "<<std::endl;

    return 0;
}
