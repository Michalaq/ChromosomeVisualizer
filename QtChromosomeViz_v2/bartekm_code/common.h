//
// Created by bartek on 09.12.15.
//

#ifndef PDB_PARSER_COMMON_H
#define PDB_PARSER_COMMON_H

#include <map>
#include <vector>
#include <iostream>
#include <cstdint>

#ifdef DEBUG
    const bool dbg = true;
    inline void print_dbg(const std::string & str) { std::cout << str << std::endl; };
#else
    const bool dbg = false;
    inline void print_dbg(const std::string & str) {};
#endif

using frameNumber_t = int64_t;

struct Atom {
    int id;
    char type[4];
    float x, y, z;
    int layerNo, inLayerId;
    const char* t = 0;
};

inline bool operator ==(const Atom& lhs, const Atom &rhs) {
    return lhs.id == rhs.id && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

inline bool operator !=(const Atom& lhs, const Atom &rhs) {
    return !(lhs == rhs);
}

inline std::ostream& operator <<(std::ostream& o, const Atom& a) {
    o << "{ id: " << a.id << ", type: " << a.type << ", x: " << a.x << ", " << a.y << ", " << a.z << "}";
    return o;
}

struct Frame {
    frameNumber_t no, step; // no - which frame is it in the visualisation, step - which frame is it in the simulation
    std::vector<Atom> atoms;
    std::map<std::string, float> functionValues;
    std::vector<std::pair<int, int>> connectedRanges;
//    Frame(std::vector<Atom> tmp_atoms) : atoms(tmp_atoms) {};
};


inline std::ostream& operator <<(std::ostream& o, const Frame& d) {
    o << "Frame NO: " << d.no << ", STEP: " << d.step << ",\nfunction values:";
    for (const auto& f : d.functionValues) {
        o << "{ " << f.first.c_str() << " = " << f.second << " }, ";
    }
    o << "\natoms: [";
    for (const Atom& a : d.atoms) {
        o << a << "\n";
    }
    o << "]" << std::endl;
    return o;
}

#endif //PDB_PARSER_COMMON_H
