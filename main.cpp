
#include <iostream> //debug prints
#include <memory> //unique ptr and make unique
#include <vector> //vector containers
#include <string> //used for identifiers

//defines the components of the sections
struct Section{ 
    std::string id;
    double certainty;
    double uncertainty;
    int gaps;

    Section(const std::string& id_, double c = 0.0, double u = 1.0, int g = 0) :id(id_), certainty(c), uncertainty(u), gaps(g) {}; //mapping
};

//for more flexibility, we define a memory pool, a contiguous memory pool  where we will be storing sections
class SectionPool {
    std::vector<Section> pool; // contiguous memory pool
public:
    // Preallocate memory for n sections - we now avoid dynamic heap allocations
    void reserve(int n) { pool.reserve(n); }

    // Add a section to the pool, then we return the pointer to the section in the pool
    Section* addSection(const std::string& id, double c=0.0, double u=1.0, int g=0) {
        pool.emplace_back(id, c, u, g);
        return &pool.back(); 
    }

    // Find section by id - the look up is  linear, which is still a problem -> perhaps id can be used to create a sort of id range defined search that enables our search to jump effectively->think about hash maps this time
    Section* getSection(const std::string& id) {
        for (auto& s : pool)
            if (s.id == id) return &s;
        return nullptr;
    }

    // Optional: iterate all sections
    std::vector<Section>& getAll() { return pool; }
};

class Agent {
    std::string name; //name identifier
    std::vector<Section*> mySections; // borrowed pointers from pool
    class System* system;             // raw pointer to system (borrowed)

public:
    Agent(const std::string& n, System* sys) : name(n), system(sys) {} //initializes name & system pointer

    void addSection(Section* s) { 
        mySections.push_back(s); // just store pointer, no ownership of Section, pool guarantees section's lifetime.
    }

    inline Section* getSection(const std::string& id) {
        //to get section from the agents sections
        for (auto* s : mySections)
            if (s->id == id) return s;
        return nullptr;
    }

    //update belief heuristic
    void shareBelief(Agent* other, const std::string& sectionId) {
        Section* mySec = getSection(sectionId);
        Section* otherSec = other->getSection(sectionId);
        if (!mySec || !otherSec) return;

        if (mySec->certainty > otherSec->certainty) {
            otherSec->certainty = mySec->certainty;
            otherSec->uncertainty = mySec->uncertainty;
            otherSec->gaps = mySec->gaps;
            std::cout << name << " shared " << sectionId << " with " << other->name << "\n";
        }
    }

    void printSections() {
        std::cout << "Agent " << name << " sections:\n";
        for (auto* s : mySections)
            std::cout << "  " << s->id << " | C:" << s->certainty
                      << " U:" << s->uncertainty << " G:" << s->gaps << "\n";
    }

    std::string getName() const { return name; }
};


class System {
    std::vector<std::unique_ptr<Agent>> agents; // owns agents using unique_ptr
    SectionPool pool;                            // owns all sections so agents would never own dangling pointers

public:
    // Agent management - creates agents and then transfers ownwership to the system, returns a raw pointer for temporary access
    Agent* createAgent(const std::string& name) {
        auto a = std::make_unique<Agent>(name, this);
        Agent* ptr = a.get();
        agents.push_back(std::move(a));
        return ptr;
    }

    //linear search for small numbers
    Agent* getAgent(const std::string& name) {
        for (auto& a : agents)
            if (a->getName() == name) return a.get();
        return nullptr;
    }

    // The agents can borrow sections from pool
    Section* createSection(const std::string& id, double c=0.0, double u=1.0, int g=0) {
        return pool.addSection(id, c, u, g);
    }

    Section* getSection(const std::string& id) {
        return pool.getSection(id);
    }

    void printAll() {
        for (auto& a : agents) a->printSections();
    }

    SectionPool& getPool() { return pool; }
};







int main() {
    System sys;

    // Create sections in the pool
    Section* secA1 = sys.createSection("A1", 0.9, 0.1, 2);
    Section* secA2 = sys.createSection("A2", 0.7, 0.3, 5);

    // Create agents
    Agent* alice = sys.createAgent("Alice");
    Agent* bob   = sys.createAgent("Bob");

    // Assign sections from pool to agents
    alice->addSection(secA1);
    alice->addSection(secA2);

    bob->addSection(secA1); // both agents share same Section pointers
    bob->addSection(secA2);

    std::cout << "Before sharing:\n";
    sys.printAll();

    // Share beliefs (updates same pool objects)
    alice->shareBelief(bob, "A1");
    bob->shareBelief(alice, "A2");

    std::cout << "\nAfter sharing:\n";
    sys.printAll();
}