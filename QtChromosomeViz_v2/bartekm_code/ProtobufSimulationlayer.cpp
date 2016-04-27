#include "ProtobufSimulationlayer.h"

using namespace protostream;


ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string &name, const std::string &fileName)
    : fileName_(fileName)
    , rd_(fileName.c_str())
    , SimulationLayer(name)
{
    //std::cout << "header: " << rd_.get_proto_header() << std::endl;
    for (const auto& keyframe : rd_) {
        keyframes_.resize(keyframes_.size() + 1);

        if (!keyframes_.back().ParseFromString(keyframe.get())) {
              std::cout << "Failed to parse address book." << std::endl;
        }
        //std::cout << "keyframe: " << keyframe.get() << std::endl;

        /*for (const auto& delta : keyframe) {
            bio::motions::format::proto::Delta d;

            if (!d.ParseFromString(delta.get())) {
                  std::cout << "Failed to parse address book." << std::endl;
                  return -1;
            } else {
                  std::cout << (d.has_from() ? "ma" : "nie ma")  << " " << d.callbacks_size() << std::endl;
                  if (d.has_from()) {
                    bio::motions::format::proto::Point * p = d.mutable_from();
                    if  (p->has_x() && p->has_y() && p->has_z()) {
                        std::cout << "dupa: (" <<  p->x() << ", " << p->y() << ", " << p->z() << ")" <<  std::endl;
                    }
                  }
            }
            //std::cout << "delta: " << delta.get() << std::endl;
        }*/
    }
}

ProtobufSimulationLayer::ProtobufSimulationLayer(const std::string & fileName)
    : ProtobufSimulationLayer(fileName, fileName)
{}



std::shared_ptr<Frame> ProtobufSimulationLayer::getFrame(frameNumber_t position)
{
    auto kf = keyframes_[position];
    std::vector<Atom> atoms;
    static const char * types[] = { "LAM", "BIN" }; // hehehehe
    for (int i = 0; i < kf.binders_size(); i++) {
        auto binder = kf.binders(i);
        auto point = binder.position();
        Atom a;
        a.id = i+1;
        strcpy(a.type, types[0]);
        a.x = point.x();
        a.y = point.y();
        a.z = point.z();
        atoms.push_back(a);
    }

    Frame f = {
        position,
        position,
        std::move(atoms),
        std::map<std::string, float>()
    };
    if (position >= frameCount_) { // hihihi
        frameCount_ = position + 1;
        emit frameCountChanged(frameCount_);
    }

    return std::make_shared<Frame>(f);
}
