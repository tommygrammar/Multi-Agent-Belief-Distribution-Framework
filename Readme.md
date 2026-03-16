# Multi Agent Belief Distribution framework
- This repo implements a multi agent belief distribution framework designed to enable decentralized coordination in multi agent environments.

## Repo Contents
1. Headers
    -  `Agent.h`
    - `SectionPool.h`
    - `System.h`
2. Example
    - `example.cpp`
3. Makefile

## Core Idea
1. Agents share beliefs about sections
2. Beliefs include certaint and uncertaint
3. This information influences navigational decisions
4. Coordination is decertralized.

## How to run
```bash
git clone Multi-Agent-Belief-Distribution-Framework

cd Multi-Agent-Belief-Distribution-Framework

make

./run

```

## Algorithm Design
### Imports
1. `SectionPool.h`
```cpp
#include <array> //an array container whose size is known at compile time as we know the maximum number of sections, this enables compiler optimizations
#include <string> //for identifiers, specifically id which is a section identifier
#include <vector> //for neighbours which is an adjacent list of neighbours
```

2. `Agent.h`
```cpp
#include <string> //Agent identifiers
#include <vector> //vectors
#include "SectionPool.h" //imports section memory pool and sections itself
#include <iostream> //debug prints
```

3. `System.h`
```cpp
#include <vector> //containers
#include "SectionPool.h" //pools
#include "Agent.h" //agent class
#include <string> //identifiers
#include <memory> //smart pointers -> unique_ptr, make unique
#include <unordered_map> //has table dictionary for faster lookups
#include <algorithm> //reverse
#include <queue> //priority queue
```

4. `example.cpp`
```cpp
#include <iostream> //print outputs
#include "headers/System.h" // imports the whole system class
```
## Implementation Details
1. `SectionPool.h`
- This contains the section structure and our memory pool that allows flexible access to global sections.
- The first part is:
```cpp
constexpr size_t MAX_SECTIONS = 32;
```
- Since we already know the maximum number of sections that are going to be in the program, we set a number which in this case is 32. This allows us to create an array container instead of a vector container, allowing us to  avoid reallocation.

- Next, we have the Struct which is:
```cpp
struct Section {
    std::string id;
    double certainty;
    double uncertainty;
    int gaps;

    std::vector<Section*> neighbors; 
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
}
```
- This defines the contents of sections which are id, certainty, uncertainty and gaps.
- We also define neighbours which is a vector of pointers pointing specifically to sections.
- `addNeighbor(Section* other)` adds new neighbours to the end of neighbours, it specifically adds a raw pointer of a neighbour.

- Next, we have the `SectionPool` which is a class.
- Initially, we directly had a struct and then agents would deal directly with sections, but we wanted an access pattern that promoted flexibility of access while still tying the Section pool to the systems lifetime.
- We went for an array pool that would store the sections.
- `size_t count` counts specifically the number of sections in use currently.
- The public contains:
- ` Section* createSection(const std::string& id, double c = 0.0,double u = 1.0,int g = 0)` 
    - This creates a new section. We first check if the count of sections is less than the capacity of our memory pool, if it is less than that, we insert an entry of a new section and return the reference of the created one.
- `Section* getSection(const std::string& id) `
    - This gets a specific section by using the argument id.
    - Then we iterate a loop and if we find the instance of id, return the reference of that specific instance.
    - If it fails we return a null pointer.
- `size_t size() const`
    - This one specifically returns the count
- `Section* operator[](size_t i)`
    - keeps the access in an array, a contigous array.

2. `Agent.h`
- This is the agent class. 
- We first declare a forward declaration so that agent can point to the system: `class System;`
- Next we define a struct that will be used later: `NodeCost`.
- Nodecost is made up of pointers to the section, as well as cost of reaching the node from the start. It also has a bool operator function that takes the nodecost of another and returns a bool of whether the current cost is greater than the cost of the other.

- Next, we have the agent class:
    - The agent class contains name which is a string
    - `std::vector<Section*> mySections` - which points to sections owned by the agent. Their raw pointers are used.
    - `System* system` - can borrow from system class but does not own system.

- Its public contains:
    - `Agent(const std::string& n, System* sys)`  which takes a name and raw pointer to system in order to access system features.
    - `void addSection(Section* s)` which adds section my a pointer
    - `Section* getSection(const std::string& id)` which gets section by id iteration and returns the pointer, if not it returns a null pointer
    - `void updateSection(const std::string& id,double certainty, double uncertainty,int gaps);` - this updates global beliefs
    -`void shareBelief(Agent* other, const std::string& sectionId)` - this one shares beliefs, for example, if the current agent shares a section with another agent, then if they have more certainty than the current agent, then what happens is it uses a very simple heuristic that updates the current agent with the section knowledge of the other agent.
    - `std::vector<Section*> shortestCertainPath(Section* start,Section* goal);` - this one finds the shortest most certain path, we will explore it in systems.
    - `std::string getName() const` - this gets the name of an agent

3. `System.h`
- This is the system responsible for managing both the pool and the agent. 
- The system owns both the agents and the memory pool.
- If the system dies, it ensures the pool and the agent die with it.
- The public contains:
    - `Agent* createAgent(const std::string& name)` which creates an agent, after creating an agent, it transfers ownership to the system and returns the pointer of the newly created agent.
    - `Section* createSection(const std::string& id,double c = 0.0,double u = 1.0,int g = 0)` - creates a section.
    - `Section* getSection(const std::string& id)` - calles the section function and gets the id.
    - `SectionPool& getPool()` - this gets the whole memory pool.

- Then, still in system.h, we have the `void Agent::updateSection(const std::string& id,double certainty,double uncertainty,int gaps)`. The goal of this is to update the global with new sections.
- Its process is:
    - The pointer points to a sstem instance of a certain id and gets section
    - If certainty available by the current Agent for that section is greater than the one that has just been pooled from the global pool, then it updates the global one.
    - It also adds a section, sec which is the new one.

- Next we have: 
`std::vector<Section*> Agent::shortestCertainPath(Section* start,Section* goal) `
- This computes the certainty aware shortest path and returns a vector representing the path. The selection is that the path has to be short and more certain in order to be chosen.
- The algorithm specifically finds a path that balances between distance and certainty
- `std::priority_queue<NodeCost, std::vector<NodeCost>,std::greater<NodeCost>> pq; `. This stores nodes and accumulated cost. Its a min-heap of nodes that ensures the node with the lowest cost comes first.
- `std::unordered_map<Section*, double> dist;`. This keeps track of the shortest cost found so far
- `std::unordered_map<Section*, Section*> prev;` - keeps track of previous section on the best path
- ` pq.push({start, 0.0})` - starts from cost 0.0 and adds it to queue for exploration
- while pq is not empty, the current is the first element in the queue, it then pops the next node with the smallest cost
- if current node is the goal, then we are done and the loop breaks
- we then assess uncertainty, which is lower cost with higher certainty
- we used a simple heuristic weight calculation that ensured that if certainty is close to 1 then weight is closer to 1 and thus it is more desirable, if close to 0, then weight is close to 100 which means less desirable
- this encourages agents to prefer paths through more certain sections.
- the new cost is a combo of distance and certaint weight to find the cost to reach current node and cost to move to neighbour.
- if the new path to neighbour is cheaper than the previous one, it updates the dist[neighbour] with new cost and previus neighbour with current node, then pq.push neighbour and  newcost adds them to priority queue to explore its neighbours,

- once done, we reconstruct the path, from the start of the goal  then follows previous map back to start
- we then reverse to get path from start to goal
- we return that path.

4. `example.cpp`
- Creates an instance of System called sys
- Creates 4 sections
- Adds neighbours
- Creates two agents alice and bob
- alice updates global sections with what she knows, bob also updates it with what he knows
- alice shares belief with bob
- we find the best path alice should take from a start of A to a goal of D



