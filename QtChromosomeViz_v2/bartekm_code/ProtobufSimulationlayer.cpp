#include "ProtobufSimulationlayer.h"
#include <set>
#include <QDebug>
#include <fstream>
#include <exception>
#include <string>
#include "preferences.h"

using namespace protostream;


ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string &name, const std::string &fileName)
    : UntransformedSimulationLayer(name)
    , fileName_(fileName)
    , reachedEndOfFile_(false)
    , rd_(fileName.c_str())
    , deltasPerKeyframe_(0)
    , positionCachedFor_(-1)
{
    bio::motions::format::proto::Header header;
    header.ParseFromString(rd_.get_proto_header());
//    std::cout << header.simulation_name() << std::endl;
//    std::cout << header.simulation_description() << std::endl;
    for (const auto& keyframe : rd_) {
        keyframes_.emplace_back();
        keyframesData_.push_back(keyframe);

        if (!keyframes_.back().ParseFromString(keyframe.get())) {
              std::cout << "Failed to parse keyframe." << std::endl;
        }
    }
//    std::cout << keyframes_.size() << std::endl;

    for (const auto& binder : keyframes_[0].binders()) {
        binderTypes.push_back(Preferences::getInstance()->bt2typename(binder.binder_type()));
    }
    for (const auto& chain : header.chains()) {
        std::vector<int> types;
        for (const auto& beadDesc : chain.beads()) {
            std::vector<int> w(header.binders_types_count(), 0);
            for (const auto& binding : beadDesc.energy_vector()) {
                w[binding.binder_type()] = binding.force();
            }
            types.push_back(Preferences::getInstance()->ev2typename(w));
        }
        chainAtomTypes.push_back(types);
    }
//    for (const auto& d : chainAtomTypes) {
//        for (const auto& e : d) {
//            std::cout << "Typ: " << e << std::endl;
//        }
//    }
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
//    std::cout << deltasPerKeyframe_ << std::endl;

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
        a.type = binderTypes[a.id - 1];
        a.x = point.x();
        a.y = point.y();
        a.z = point.z();
        atoms.insert(a);
    }

    for (int i = 0; i < kf.chains_size(); i++) {
        auto& chain = kf.chains(i);
        connectedRanges.push_back({ aid, aid + chain.bead_positions_size() - 1 });
        for (int j = 0; j < chain.bead_positions_size(); j++) {
            auto& point = chain.bead_positions(j);
            Atom a;
            a.id = aid++;
            a.type = chainAtomTypes[i][j];
            a.x = point.x();
            a.y = point.y();
            a.z = point.z();
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
    {
        if (outPosition)
            *outPosition = 0;
        return keyframes_.front().step_counter();
    }

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
