// the system which owns both the agents and pool, lifetime of both is tied to this
#pragma once
#include <vector>
#include "SectionPool.h"
#include "Agent.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <queue>

//system class
class System {

    std::vector<std::unique_ptr<Agent>> agents; //owns all agents

    SectionPool pool; //pool id owned by System

public:

    Agent* createAgent(const char& name) //creates an agent , uses agent pointer
    {
        auto a = std::make_unique<Agent>(name, this); //after creating, it transfers ownership to system

        Agent* ptr = a.get(); //we get an agent pointer

        agents.push_back(std::move(a)); //agent pointer is moved to be ownwed

        return ptr;
    }

    Section* createSection(const char& id,
                           double c = 0.0,
                           double u = 1.0,
                           int g = 0)
    {
        return pool.createSection(id, c, u, g); // this enables creation of section
    }

    Section* getSection(const char& id)
    {
        return pool.getSection(id); //gets section
    }

    SectionPool& getPool()
    {
        return pool; //this gets pool
    }
};

void Agent::updateSection(const char& id,
                            double certainty,
                            double uncertainty,
                            int gaps)
{
    Section* sec = system->getSection(id);//the pointer sec points to system instance of a certain section id
    if (!sec) //if none, returns nothing
        return;
    if (certainty > sec->certainty) //if certainty available currently by the agent is greate than the certainty in thhe global, then it updates the global one
    {
        sec->certainty = certainty;
        sec->uncertainty = uncertainty;
        sec->gaps = gaps;
    }

    addSection(sec); //adds a section  with the new one
}


//computes the certainty aware shortest path and returns a vector representing the path - agent is to choose a path that is both short and more certain
std::vector<Section*> Agent::shortestCertainPath(Section* start,
                                                 Section* goal) 
{
 //min-heap of nodes - always expand the most promising section next
    std::priority_queue<NodeCost, //stores nodes + accumulated cost
                        std::vector<NodeCost>,
                        std::greater<NodeCost>> pq; //ensures the node with the lowest cost comes first

    std::unordered_map<Section*, double> dist; //keps track of the shortest cost found so far to each section
    std::unordered_map<Section*, Section*> prev; //keeps track of previous section on the best path
    pq.push({start, 0.0}); //start from cost = 0, adds it to queue for exploration
    dist[start] = 0.0;

    while (!pq.empty()) //while pq is not empty
    {
        auto current = pq.top();
        pq.pop(); //pop the next node with the smallest cost
        if (current.node == goal) //if its the goal we are done, the loop breaks as the path has been found
            break;

        for (auto* neighbor : current.node->neighbors) //higher certainty->lower cost
        //certaint close to 1 -> weight closer to 1.0 -> more desirable
        //certainty close to 0-> weight ~100 less desirable
        //this encourages agents to prefer paths through more certain sections
        
        {
            double weight = 1.0 / (neighbor->certainty + 0.01);
            double newCost = dist[current.node] + weight; // remember the cost is a combo of distance and certainty weight, so we calculate the new cost to find the cost to reach current node + cost to move to neighbour

            if (!dist.count(neighbor) || newCost < dist[neighbor]) //if new path to neighbour is cheaper than am previous one, update dist annd prev
            {

                dist[neighbor] = newCost; //unordered map lookups etc
                prev[neighbor] = current.node;
                pq.push({neighbor, newCost}); // add neighbor to priority queue to explore its neighbors
            }
        }
    }

    std::vector<Section*> path;
    Section* cur = goal;
    while (cur)
    { // once we reach the goal, we reconstruct the path
        path.push_back(cur); //start at goal
        if (!prev.count(cur)) //follow prev map back to start
            break;
        cur = prev[cur];
    }
    std::reverse(path.begin(), path.end()); //reverse to get path from start to goal

    return path;
}
