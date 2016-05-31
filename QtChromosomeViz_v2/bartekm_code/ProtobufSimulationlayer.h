#ifndef PROTOBUFSIMULATIONLAYER_H
#define PROTOBUFSIMULATIONLAYER_H


#include "SimulationLayer.h"
#include "include/stream.h"
#include "include/posix_file_backend.h"
#include "include/mmap_backend.h"
#include "include/cache.h"
#include "message_format/message.pb.h"
#include <vector>
#include <map>

using namespace protostream;

struct string_factory {
    using type = std::string;

    static type build(const char* start, std::size_t len) {
        return std::string(start, start + len);
    }

    template <class Ptr>
    static type build(Ptr&& ptr, std::size_t len) {
        return build(reinterpret_cast<const char*>(detail::as_ptr(ptr)), len);
    }
};

using mmap_reader = stream<with_backend<mmap_backend<file_mode_t::READ_ONLY>>,
                           with_cache<offsets_only_cache>,
                           with_keyframe_factory<string_factory>,
                           with_delta_factory<string_factory>,
                           with_proto_header_factory<string_factory>>;

using stream_reader = stream<with_backend<posix_file_backend<file_mode_t::READ_ONLY>>,
                             with_cache<full_cache>,
                             with_keyframe_factory<string_factory>,
                             with_delta_factory<string_factory>,
                             with_proto_header_factory<string_factory>>;

class ProtobufSimulationLayer : public SimulationLayer {
private:
    std::string fileName_;
    int connectionCount_;
    bool reachedEndOfFile_;
    std::vector<bio::motions::format::proto::Keyframe> keyframes_;
    mmap_reader rd_;
    frameNumber_t deltasPerKeyframe_;
    std::vector<mmap_reader::keyframe_data> keyframesData_;
    std::vector<std::string> binderTypes;
    std::vector<std::vector<std::string>> chainAtomTypes;
    std::vector<std::string> binderColorMap;
    std::map<std::vector<int>, std::string> evColorMap;

    static Atom getAtomFromString(const std::string & str);
    std::shared_ptr<Frame> readCurrentFrame();
    void setColors(const std::string & fileName);
public:
    ProtobufSimulationLayer(const std::string & name, const std::string & fileName);
    ProtobufSimulationLayer(const std::string & fileName);
    ~ProtobufSimulationLayer() noexcept {};
    virtual std::shared_ptr<Frame> getFrame(frameNumber_t position) override;
    virtual bool reachedEndOfFile() const override;
};

#endif // PROTOBUFSIMULATIONLAYER_H
