#include "ProtobufSimulationlayer.h"
#include <set>
#include <QDebug>

using namespace protostream;


ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string &name, const std::string &fileName)
    : fileName_(fileName)
    , reachedEndOfFile_(false)
    , rd_(fileName.c_str())
    , deltasPerKeyframe_(0)
    , positionCachedFor_(-1)
    , SimulationLayer(name)
{
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
    std::cout << keyframes_.size() << std::endl;
    str_types_ = { "BIN", "LAM", "BOU", "UNB" };
    std::set<std::vector<int>> binder_types;
    std::vector<std::vector<int>> binder_types_vec;
    for (const auto& binder : keyframes_[0].binders()) {
        binder_types_.push_back(binder.binder_type());
        binder_types.insert({binder.binder_type()});
    }
    for (const auto& chain : header.chains()) { // each chain
        for (const auto& bead : chain.beads()) { // has atoms
            std::vector<int> atom_types;
            if (bead.energy_vector_size() == 0) {
                std::cout << "Bead without bindings." << std::endl;
            } else if (bead.energy_vector_size() > 1) {
                std::cout << "Bead with several bindings." << std::endl;
            }
            for (const auto& binding : bead.energy_vector()) { // of several types
                    auto type = binding.binder_type();
                    atom_types.push_back(type);
            }
            std::sort(atom_types.begin(), atom_types.end());
            binder_types.insert(atom_types);
            binder_types_vec.push_back(atom_types);
        }
    }
    binder_types_real_ = std::vector<std::vector<int>>(binder_types.begin(), binder_types.end()); // mapping index(our type) -> real type
    int chain_no = 0, atom_no = 0, curr_chain_size = header.chains(chain_no).beads_size();
    chain_binder_types_renumbered_.resize(chain_binder_types_renumbered_.size() + 1);
    for (int i = 0; i < binder_types_vec.size(); i++) {
        if (atom_no >= curr_chain_size) {
            atom_no -= curr_chain_size;
            chain_no++;
            curr_chain_size = header.chains(chain_no).beads_size();
            chain_binder_types_renumbered_.resize(chain_binder_types_renumbered_.size() + 1);
        }
        for (int j = 0; j < binder_types_real_.size(); j++) {
            if (binder_types_vec[i] == binder_types_real_[j]) {
                chain_binder_types_renumbered_[chain_no].push_back(j);
                atom_no++;
                break;
            }
        }
    }
    for (int i = 0; i < binder_types_.size(); i++) {
        for (int j = 0; j < binder_types_real_.size(); j++) {
            if (binder_types_real_[j] == std::vector<int>{ binder_types_[i] }) {
                binder_types_[i] = j;
                break;
            }
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

    deltasPerKeyframe_ = rd_.frames_per_keyframe();
    std::cout << deltasPerKeyframe_ << std::endl;

    // Get last frame time
    lastFrameNumber_ = rd_.frame_count() - 1;
    auto it = keyframesData_.back().begin();
    if (it == keyframesData_.back().end()) {
        if (keyframes_.back().has_step_counter())
            lastFrameNumber_ = keyframes_.back().step_counter();
    } else {
        auto it2 = ++keyframesData_.back().begin();
        while (it2 != keyframesData_.back().end()) {
            ++it;
            ++it2;
        }

        bio::motions::format::proto::Delta delta;
        delta.ParseFromString(it->get());

        if (delta.has_step_counter())
            lastFrameNumber_ = delta.step_counter();
    }
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

std::shared_ptr<Frame> ProtobufSimulationLayer::getFrame(frameNumber_t time)
{
    frameNumber_t position;
    frameNumber_t nextTime = getPositionInfo(time, 1, &position);

    if (time >= frameCount_) { // hihihi
        if (time >= lastFrameNumber_) {
            time = lastFrameNumber_;
            reachedEndOfFile_ = true;
        }

        if (time > lastFrameNumber_)
            time = lastFrameNumber_;

        frameCount_ = nextTime;
        emit frameCountChanged(frameCount_);
    }

    return getFrameById(position);
}

std::shared_ptr<Frame> ProtobufSimulationLayer::getFrameById(frameNumber_t position)
{
    if (position == positionCachedFor_)
        return cachedFrame_;

    auto kf_no = position / (deltasPerKeyframe_ + 1);
    auto delta_no = position % (deltasPerKeyframe_ + 1);
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
        strcpy(a.type, str_types_[binder_types_[i]].c_str());
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
            strcpy(a.type, str_types_[chain_binder_types_renumbered_[i][j]].c_str());
            a.x = point.x();
            a.y = point.y();
            a.z = point.z();
            a.layerNo = layerId_;
            a.inLayerId = a.id;
            atoms.insert(a);
        }
    }

    if (delta_no == 0) {
        for (const auto& cb : kf.callbacks()) {
            auto p = parseCallback(cb);
            functionValues[p.first] = p.second;
        }
        if (kf.has_step_counter())
            stepNo = kf.step_counter();
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

    positionCachedFor_ = position;
    cachedFrame_ = std::make_shared<Frame>(std::move(f));

    // TODO: Emit frameCountChanged here

    return cachedFrame_;
}

frameNumber_t ProtobufSimulationLayer::getPositionInfo(frameNumber_t time, int offset, frameNumber_t *outPosition) const
{
    // Find appropriate keyframe
    auto compKeyframe = [](const bio::motions::format::proto::Keyframe & key, frameNumber_t time) {
        return key.step_counter() > time;
    };
    auto it = std::lower_bound(keyframes_.rbegin(), keyframes_.rend(), time, compKeyframe);
    if (it == keyframes_.rend())
        it = keyframes_.rbegin();

    // Look for the right delta
    // qDebug() << keyframes_.size();
    int keyframeID = std::distance(it, keyframes_.rend()) - 1;
    int deltaID = 0;
    auto data = keyframesData_.begin() + keyframeID;
    frameNumber_t position = it->step_counter();
    auto it2 = data->begin();
    for (; it2 != data->end(); it2++) {
        bio::motions::format::proto::Delta delta;
        delta.ParseFromString(it2->get());
        frameNumber_t newPosition = delta.step_counter();
        if (newPosition > time)
            break;
        ++deltaID;
        position = newPosition;
    }

    assert(position <= time);

    if (outPosition)
        *outPosition = keyframeID * rd_.frames_per_keyframe() + deltaID;

    // Move forwards, or backwards
    if (offset > 0) {
        if (it2 == data->end()) {
            ++data;
            if (data == keyframesData_.end())
                return time;
            --it;
            position = it->step_counter();
        } else {
            bio::motions::format::proto::Delta delta;
            delta.ParseFromString(it2->get());
            position = delta.step_counter();
        }
    }

    if (offset < 0 && position == time) {
        if (it2 == data->begin()) {
            if (data == keyframesData_.begin())
                return time;
            --data;
            auto it3 = data->begin();
            auto it4 = ++data->begin();
            while (it4 != data->end()) {
                ++it3;
                ++it4;
            }
            bio::motions::format::proto::Delta delta;
            delta.ParseFromString(it3->get());
            position = delta.step_counter();
        } else {
            if (deltaID == 1) {
                position = it->step_counter();
            } else {
                auto it3 = data->begin();
                deltaID--;
                while (--deltaID > 0)
                    it3++;
                bio::motions::format::proto::Delta delta;
                delta.ParseFromString(it3->get());
                position = delta.step_counter();
            }
        }
    }

    return position;
}

frameNumber_t ProtobufSimulationLayer::getNextTime(frameNumber_t time)
{
    return getPositionInfo(time, 1);
}

frameNumber_t ProtobufSimulationLayer::getPreviousTime(frameNumber_t time)
{
    return getPositionInfo(time, -1);
}

bool ProtobufSimulationLayer::reachedEndOfFile() const
{
    return reachedEndOfFile_;
}
