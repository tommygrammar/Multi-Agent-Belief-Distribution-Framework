//Contains the class Agent and the Stuff associated with navigation etc
#pragma once
#include <string>
#include <vector>
#include "SectionPool.h"
#include <iostream>


// forward declaration so that agent can use system
class System;

//certainty aware djisktra
struct NodeCost {

    Section* node; //pointers to the section

    double cost; //cost to reach this node from start

    bool operator>(const NodeCost& other) const //returns 
    {
        return cost > other.cost; //returns a bool of whether this cost is greater than the other cost
    }
};



// agent class 
class Agent {
    std::string name;
    std::vector<Section*> mySections; //sections owned by the agent, their praw pointers to sections
    System* system; //can use system features by borrowing

public:

    Agent(const std::string& n, System* sys)
        : name(n), system(sys) {}



    void addSection(Section* s) //adds section by pushback, to the back
    {
        mySections.push_back(s);
    }



    Section* getSection(const std::string& id) // get section by id
    {
        for (auto* s : mySections) //iterates thru all of them and if it gets that id, it returns the raw pointer to that id
            if (s->id == id)
                return s;

        return nullptr; //else if nothing returns a nulptr
    }




    //this discovery updates global beliefs
    void updateSection(const std::string& id,
                         double certainty,
                         double uncertainty,
                         int gaps);



    void shareBelief(Agent* other, const std::string& sectionId) //this one shares beliefs and if they are beaten, they exchange information
    {
        Section* mySec = getSection(sectionId);
        Section* otherSec = other->getSection(sectionId);

        if (!mySec || !otherSec)
            return;

        if (mySec->certainty > otherSec->certainty)
        {
            otherSec->certainty = mySec->certainty;
            otherSec->uncertainty = mySec->uncertainty;
            otherSec->gaps = mySec->gaps;

            std::cout << name
                      << " shared belief about "
                      << sectionId
                      << " with "
                      << other->name
                      << "\n";
        }
    }



    // certainty aware Dijkstra - finds the shortest path
    std::vector<Section*> shortestCertainPath(Section* start,
                                              Section* goal);



    std::string getName() const //gets name of agent
    {
        return name;
    }
};