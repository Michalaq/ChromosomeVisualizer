#include "ProtobufSimulationlayer.h"

using namespace protostream;


ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string &name, const std::string &fileName)
    : fileName_(fileName)
    , rd_(fileName.c_str())
    , deltasPerKeyframe_(0)
    , SimulationLayer(name)
{
    //std::cout << "header: " << rd_.get_proto_header() << std::endl;
    for (const auto& keyframe : rd_) {
        keyframes_.resize(keyframes_.size() + 1);
        keyframesData_.push_back(keyframe);

        if (!keyframes_.back().ParseFromString(keyframe.get())) {
              std::cout << "Failed to parse address book." << std::endl;
        }
    }
    for (const auto& delta : keyframesData_[0])
        deltasPerKeyframe_++;
    deltasPerKeyframe_ = deltasPerKeyframe_ == 0 ? 1 : deltasPerKeyframe_;
}

ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string & fileName)
    : ProtobufSimulationLayer(fileName, fileName)
{}

struct lex_comp
{

    bool operator()(const Atom& a, const Atom& b) const
    {
        return a.x < b.x
                || (a.x == b.x && a.y < b.y)
                || (a.x == b.x && a.y == b.y && a.z < b.z);
    }

};

std::shared_ptr<Frame> ProtobufSimulationLayer::getFrame(frameNumber_t position)
{
    auto kf_no = position / deltasPerKeyframe_;
    auto delta_no = position % deltasPerKeyframe_;
    auto kf = keyframes_[kf_no];
    std::set<Atom, lex_comp> atoms;
    static const char * types[] = { "LAM", "BIN" }; // hehehehe

    int aid = 1;
    for (int i = 0; i < kf.binders_size(); i++) {
        auto binder = kf.binders(i);
        auto point = binder.position();
        Atom a;
        a.id = aid++;
        strcpy(a.type, types[1]);
        a.x = point.x();
        a.y = point.y();
        a.z = point.z();
        atoms.insert(a);
    }
    for (int i = 0; i < kf.chains_size(); i++) {
        auto chain = kf.chains(i);
        for (int j = 0; j < chain.bead_positions_size(); j++) {
            auto point = chain.bead_positions(j);
            Atom a;
            a.id = aid++;
            strcpy(a.type, types[0]);
            a.x = point.x();
            a.y = point.y();
            a.z = point.z();
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
        auto p = delta.from();
        auto q = delta.disp();
        Atom a;
        a.x = p.x();
        a.y = p.y();
        a.z = p.z();
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
//    if (j != delta_no) {
//        std::cout << "COS ZLEGO SIE STALO." << std::endl;
//    }

    std::vector<Atom> bidy(atoms.begin(), atoms.end());
    std::sort(bidy.begin(), bidy.end(), [](const auto& a, const auto& b) { return a.id < b.id; });

    Frame f = {
        position,
        position,
        std::move(bidy),
        std::map<std::string, float>()
    };
    if (position >= frameCount_) { // hihihi
        frameCount_ = position + 1;
        emit frameCountChanged(frameCount_);
    }

    return std::make_shared<Frame>(f);
}
