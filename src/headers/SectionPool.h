//this is our section memory pool implementation
#pragma once
#include <array>
#include <string>
#include <vector>


//section size is always known at compile time
constexpr size_t MAX_SECTIONS = 32;



//section structure - id, certainty, uncertainty, gaps, adacet list neighbours of raw pointers
struct Section {
    std::string id;
    double certainty;
    double uncertainty;
    int gaps;

    std::vector<Section*> neighbors; //adjacency list, sections connect to sections establishing neighbours, the neigboours are raw pointers pointing to sections

    Section()
        : id(""), certainty(0.0), uncertainty(1.0), gaps(0) {}

    Section(const std::string& id_,
            double c = 0.0,
            double u = 1.0,
            int g = 0)
        : id(id_), certainty(c), uncertainty(u), gaps(g) {}

    void addNeighbor(Section* other) // this adds new neighbours to the end, specifically pointers
    {
        neighbors.push_back(other);
    }
};


//Memory pool
class SectionPool {

    std::array<Section, MAX_SECTIONS> pool; // max size is known at compile time

    size_t count = 0; // count of the sections in use

public:

    Section* createSection(const std::string& id,
                           double c = 0.0,
                           double u = 1.0,
                           int g = 0)
    {
        if (count >= MAX_SECTIONS) //if count is more or equal to max sections we cannot create more sections
            return nullptr;

        pool[count] = Section(id, c, u, g);// this one adds a section to the pool, the created one

        return &pool[count++]; //returns the reference of the created one
    }

    Section* getSection(const std::string& id) 
    {
        for (size_t i = 0; i < count; ++i) //we get a section by actively taking in id and iterating to get the one we want, basically if pool[i].id = id
        {
            if (pool[i].id == id)
                return &pool[i]; //returns the reference of that
        }

        return nullptr; //if failed returns nothing
    }

    size_t size() const //returns count
    {
        return count;
    }

    Section* operator[](size_t i) //keeps in an array all of them, contigous arra
    {
        return &pool[i];
    }
};

