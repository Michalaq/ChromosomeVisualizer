#include "ProtobufSimulationlayer.h"
#include <set>
#include <fstream>
#include <exception>
#include <string>

using namespace protostream;


ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string &name, const std::string &fileName)
    : fileName_(fileName)
    , reachedEndOfFile_(false)
    , rd_(fileName.c_str())
    , deltasPerKeyframe_(0)
    , SimulationLayer(name)
{
    setColors(fileName);
    bio::motions::format::proto::Header header;
    header.ParseFromString(rd_.get_proto_header());
    std::cout << header.simulation_name() << std::endl;
    std::cout << header.simulation_description() << std::endl;
    for (const auto& keyframe : rd_) {
        keyframes_.resize(keyframes_.size() + 1);
        keyframesData_.push_back(keyframe);

        if (!keyframes_.back().ParseFromString(keyframe.get())) {
              std::cout << "Failed to parse keyframe." << std::endl;
        }
    }
    std::cout << keyframes_.size() << std::endl;\
    /*std::vector<std::string> binderColorMap {
        "LAM",
        "BIN"
    };
    std::map<std::vector<int>, std::string> evColorMap {
        {{0, 0}, "UNB"},
        {{0, 1}, "BOU"},
        {{1, 0}, "LAM"},
        {{2, 0}, "LAM"}
    };*/

    for (const auto& binder : keyframes_[0].binders()) {
        binderTypes.push_back(binderColorMap[binder.binder_type()]);
    }
    for (const auto& chain : header.chains()) {
        std::vector<std::string> types;
        for (const auto& beadDesc : chain.beads()) {
            std::vector<int> w(header.binders_types_count(), 0);
            for (const auto& binding : beadDesc.energy_vector()) {
                w[binding.binder_type()] = binding.force();
            }
            types.push_back(evColorMap[w]);
        }
        chainAtomTypes.push_back(types);
    }
    for (const auto& d : chainAtomTypes) {
        for (const auto& e : d) {
            std::cout << "Typ: " << e << std::endl;
        }
    }
//    for (const auto& realtyp : chain_binder_types_renumbered_) {
//        std::cout << "lol";
//        for (const auto& dupa : realtyp)
//            std::cout << dupa << std::endl;
//    }
//    for (const auto& typek : binder_types_) {
//        if (typek == 1) {
//         std::cout << typek << ", ";
//        }
//    }

    // TODO: Change to rd_.frames_per_keyframe() when libprotostream version is updated
    deltasPerKeyframe_ = rd_.frames_per_keyframe();
    std::cout << deltasPerKeyframe_ << std::endl;
}

ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string & fileName)
    : ProtobufSimulationLayer(fileName, fileName)
{}

class colors_file_not_found_exception : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Colors file not found.";
  }
} ex;

void ProtobufSimulationLayer::setColors(const std::string & simFileName)
{
    std::string fileName = simFileName;
    std::cout << "NO ELO" << fileName << std::endl;
    std::size_t found = fileName.find("bin");
    if (found != std::string::npos) {
        fileName.replace(found, 3, "pdb.meta");
    } else {
        throw ex;
    }
    std::ifstream file(fileName);
    std::string s;
    while (getline(file, s)) {
        std::cout << "linia: " << s << std::endl;
        if (s.find("EV") != std::string::npos) {
            char s2[10], s3[10];
            sscanf(s.c_str(), "EV [%[^]]] %s", &s2, &s3);
            printf("jeden: %s, dwa: %s.\n", s2, s3);
            char * p = strtok(s2, ",");
            std::vector<int> bindingSitesTypes;
            while (p) {
                int x = atoi(p);
                bindingSitesTypes.push_back(x);
                printf("Token: %d\n", x);
                p = strtok(NULL, " ");
            }
            evColorMap[bindingSitesTypes] = std::string(s3);
        } else if (s.find("BT") != std::string::npos) {
            char s2[10];
            int type;
            sscanf(s.c_str(), "BT %d %s", &type, &s2);
            printf("Numer typu: %d, nazwa: %s.\n", type, s2);
            binderColorMap.push_back(std::string(s2));
        }
    }
}

struct lex_comp
{

    bool operator()(const Atom& a, const Atom& b) const
    {
        return a.x < b.x
                || (a.x == b.x && a.y < b.y)
                || (a.x == b.x && a.y == b.y && a.z < b.z);
    }

};

std::pair<std::string, float> parseCallback(bio::motions::format::proto::Callback cb)
{
    float value;
    std::string name = cb.has_name() ? cb.name() : "unknown_name";
    switch (cb.callback_type()) {
    case bio::motions::format::proto::Callback_Type_DOUBLE : value = cb.double_value(); break;
    case bio::motions::format::proto::Callback_Type_INT : value = cb.int_value(); break;
    case bio::motions::format::proto::Callback_Type_DOUBLE_LIST :
        value = 0;
        for (const auto& v : cb.double_list_value()) {
            value += v;
        }
        value /= cb.double_list_value_size();
        break;
    case bio::motions::format::proto::Callback_Type_INT_LIST :
        value = 0;
        for (const auto& v : cb.int_list_value()) {
            value += v;
        }
        value /= cb.int_list_value_size();
        break;
    }
    return {name, value};
}

std::shared_ptr<Frame> ProtobufSimulationLayer::getFrame(frameNumber_t position)
{
    if (position >= rd_.frame_count() - 1) {
        position = rd_.frame_count() - 1;
        reachedEndOfFile_ = true;
    }

    auto kf_no = position / deltasPerKeyframe_;
    auto delta_no = position % deltasPerKeyframe_;
    auto kf = keyframes_[kf_no];
    std::set<Atom, lex_comp> atoms;
    std::vector<std::pair<int, int>> connectedRanges;
    std::map<std::string, float> functionValues;
    frameNumber_t stepNo = kf.has_step_counter() ? kf.step_counter() : position;
    /*for (const auto& cb : kf.callbacks()) {
        std::cout << "callback!" << std::endl;
        auto p = parseCallback(cb);
        functionValues[p.first] = p.second;
    }*/

    int aid = 1;
    for (int i = 0; i < kf.binders_size(); i++) {
        auto& binder = kf.binders(i);
        if (!binder.has_position()) {
            std::cout << "Binder without position!" << std::endl;
            break;
        }
        auto& point = binder.position();
        Atom a;
        a.id = aid++;
        strcpy(a.type, binderTypes[a.id - 1].c_str());
        a.x = point.x();
        a.y = point.y();
        a.z = point.z();
        a.layerNo = layerId_;
        a.inLayerId = a.id;
        atoms.insert(a);
    }

    for (int i = 0; i < kf.chains_size(); i++) {
        auto& chain = kf.chains(i);
        int old = aid;
        connectedRanges.push_back({ aid, aid + chain.bead_positions_size() - 1 });
        for (int j = 0; j < chain.bead_positions_size(); j++) {
            auto& point = chain.bead_positions(j);
            Atom a;
            a.id = aid++;
            strcpy(a.type, chainAtomTypes[i][j].c_str());
            a.x = point.x();
            a.y = point.y();
            a.z = point.z();
            a.layerNo = layerId_;
            a.inLayerId = a.id;
            atoms.insert(a);
        }
    }

    int i = 0, j = 0;
    auto kf_it = keyframesData_[kf_no];
    for (const auto& delta_it : kf_it) {
        if (i == delta_no)
            break;
        bio::motions::format::proto::Delta delta;
        delta.ParseFromString(delta_it.get());
        if (i == delta_no - 1) { // the proper, last delta: parse callbacks, get stepno.
            for (const auto& cb : delta.callbacks()) {
                auto p = parseCallback(cb);
                functionValues[p.first] = p.second;
            }
            if (delta.has_step_counter())
                stepNo = delta.step_counter();
        }
        auto& p = delta.from();
        auto& q = delta.disp();
        Atom a;
        a.x = p.x();
        a.y = p.y();
        a.z = p.z();
        a.layerNo = layerId_;
        a.inLayerId = a.id;
        auto it = atoms.find(a);
        if (it != atoms.end()) {
            atoms.erase(it);
            Atom b = *it;
            b.x += q.x();
            b.y += q.y();
            b.z += q.z();
            atoms.insert(b);
            j++;
        }
        i++;
    }

    std::vector<Atom> bidy(atoms.begin(), atoms.end());
    std::sort(bidy.begin(), bidy.end(), [](const auto& a, const auto& b) { return a.id < b.id; });

    Frame f = {
        position,
        stepNo,
        std::move(bidy),
        std::move(functionValues),
        std::move(connectedRanges)
    };

//    for (const auto& p : f.connectedRanges) {
//        std::cout << p.first << ", " << p.second << std::endl;
//    }

    if (position >= frameCount_) { // hihihi
        frameCount_ = position + 1;
        emit frameCountChanged(frameCount_);
    }

    return std::make_shared<Frame>(f);
}

bool ProtobufSimulationLayer::reachedEndOfFile() const
{
    return reachedEndOfFile_;
}
