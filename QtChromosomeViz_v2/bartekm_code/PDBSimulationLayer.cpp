#include <algorithm>
#include <sstream>
#include <cassert>
#include <QDebug>
#include "PDBSimulationLayer.h"

PDBSimulationLayer::PDBSimulationLayer(const std::string &name, const std::string &fileName)
    : fileName_(fileName)
    , file_(fileName)
    , positionCachedFor_(-1)
    , reachedEndOfFile_(false)
    , UntransformedSimulationLayer(name)
{
    cachedFramePositions_[0] = 0;
    numbersAndPositions_.push_back(frameCount_ = readFrameHeader());
}

PDBSimulationLayer::PDBSimulationLayer(const std::string & fileName)
    : PDBSimulationLayer(fileName, fileName)
{}

frameNumber_t PDBSimulationLayer::readFrameHeader()
{
    std::string line;
    frameNumber_t step = -1;

    getline(file_, line);
    sscanf(line.c_str(), "HEADER %*d %*d step %lld", &step);

    if (file_.fail()) {
        file_.clear(); // Clear the eofbit so seekg may work
        reachedEndOfFile_ = true;
    }

    return step;
}

static std::map<std::string, float> getFunctionValues(const std::string & line)
{
    std::stringstream ss(line);
    std::string item;
    std::map<std::string, float> functionValues;
    char tmpKey[100];
    float tmpVal;
    std::getline(ss, item, ';'); // skip "TITLE chromosome"
    while (std::getline(ss, item, ';')) {
        print_dbg(item);
        sscanf(item.c_str(), "%[^=]=%f", tmpKey, &tmpVal);
        functionValues[std::string(tmpKey)] = tmpVal;
    }
    return functionValues;
}

std::shared_ptr<Frame> PDBSimulationLayer::readFrameContents(framePositionInfo_t position)
{
    std::string line;
    int no, step;
    // Unfortunately, in some files the first value after HEADER
    // is always the same, so it is actually useless as a frame number (in simulation)
    no = position.first;
    step = (position.second != -1) ? position.second : position.first;
    getline(file_, line); // TITLE
    std::map<std::string, float> functionValues = getFunctionValues(line);
    std::vector<Atom> atoms;
    getline(file_, line);
    int leftbound = -1, rightbound = -1, tmpl, tmpr;
    std::vector<std::pair<int,int>> connectionRanges;
    while (line.find("END") == std::string::npos) {
        if (line.substr(0, 4) == "ATOM") {
            auto a = getAtomFromString(line);
            a.layerNo = layerId_;
            a.inLayerId = a.id;
            atoms.push_back(a);
        } else
            if (line.substr(0, 6) == "CONECT") {
                sscanf(line.c_str(), "CONECT %d %d", &tmpl, &tmpr);
                if (leftbound == -1) {
                    leftbound = tmpl;
                    rightbound = tmpr;
                } else if (rightbound == tmpl) {
                    rightbound++;
                } else if (rightbound < tmpl) {
                    connectionRanges.push_back({leftbound, rightbound});
                    leftbound = tmpl;
                    rightbound = tmpr;
                }
            }
        if(!getline(file_, line))
            break;
    }
    if (rightbound > -1 && (connectionRanges.size() == 0 || rightbound != connectionRanges.back().second)) { // somethings missing
        connectionRanges.push_back({leftbound, rightbound});
    }
    if (connectionCount_ < 0)
        connectionCount_ = connectionRanges.size();

    if (file_.fail()) {
        file_.clear(); // Clear the eofbit so seekg may work
        reachedEndOfFile_ = true;
        return nullptr;
    }

    Frame d = {
        no,
        step,
        std::move(atoms),
        std::move(functionValues),
        std::move(connectionRanges)
    };

    if (cachedFrame_.unique())
        *cachedFrame_ = std::move(d);
    else
        cachedFrame_ = std::make_shared<Frame>(d);

    return cachedFrame_;
}

void PDBSimulationLayer::skipFrameContents()
{
    std::string line;
    while (line.find("END") == std::string::npos) {
        if (!getline(file_, line))
            break;
    }

    if (file_.fail()) {
        file_.clear(); // Clear the eofbit so seekg may work
        reachedEndOfFile_ = true;
    }
}

std::shared_ptr<Frame> PDBSimulationLayer::getFrameById(frameNumber_t position)
{
    if (reachedEndOfFile() && position > frameCount_ - 1)
        position = frameCount_ - 1;

    auto it = cachedFramePositions_.upper_bound(position);

    // Find cached frame before position
    if (it != cachedFramePositions_.begin())
        --it;

    frameNumber_t cachedPosition = it->first;
    assert(cachedPosition <= position);

    file_.seekg(it->second, file_.beg);
    frameNumber_t cachedTime = readFrameHeader();

    while (cachedPosition < position && !reachedEndOfFile()) {
        skipFrameContents();
        cachedPosition++;
        cachedFramePositions_[cachedPosition] = file_.tellg();
        cachedTime = readFrameHeader();

        if (cachedTime == -1)
            cachedTime = cachedPosition;
        numbersAndPositions_.push_back(cachedTime);
    }

    frameCount_ = std::max(frameCount_, cachedPosition);

    if (reachedEndOfFile()) {
        // Return the last frame, or - if the file is empty - return an empty frame
        if (frameCount_ == 0)
            return std::make_shared<Frame>();
        else
            return getFrameById(frameCount_ - 1); // This has bigger chance to succeed
    }

    return readFrameContents({ cachedPosition, cachedTime });
}

std::shared_ptr<Frame> PDBSimulationLayer::getFrame(frameNumber_t time)
{
    QMutexLocker lock(&accessMutex_);
    bool needToEmit = false;

    bool c1 = numbersAndPositions_.empty() || (numbersAndPositions_.back() <= time);
    bool c3 = !reachedEndOfFile();

    frameNumber_t cachedPosition;
    frameNumber_t cachedTime;
    std::shared_ptr<Frame> ret;

    if (c1 && c3) {
        // Load frames until we encounter a header which is past the 'time" moment
        cachedPosition = cachedFramePositions_.size() - 1;
        file_.seekg(cachedFramePositions_[cachedPosition]);

        cachedTime = readFrameHeader();
        ret = readFrameContents({ cachedPosition, cachedTime });

        if (!ret) {
            // Oops - the last frame is corrupted
            // Remove header of the faulty frame
            numbersAndPositions_.pop_back();
            cachedFramePositions_.erase(cachedFramePositions_.find(cachedPosition));

            if (cachedFramePositions_.empty()) {
                // We failed to read the first frame!
                return std::make_shared<Frame>();
            } else {
                // Return the previous one instead
                cachedPosition = cachedFramePositions_.size() - 1;
                file_.seekg(cachedFramePositions_[cachedPosition]);

                cachedTime = readFrameHeader();
                return readFrameContents({ cachedPosition, cachedTime });
            }
        }

        while (cachedTime <= time && !reachedEndOfFile()) {
            auto filePosition = file_.tellg();
            cachedPosition++;
            cachedTime = readFrameHeader();
            if (cachedTime == -1)
                cachedTime = cachedPosition;
            if (reachedEndOfFile())
                break;
            if (cachedTime <= time) {
                std::shared_ptr<Frame> next = readFrameContents({ cachedPosition, cachedTime });
                if (!next)
                    break;
                ret = next;
            }

            numbersAndPositions_.push_back(cachedTime);
            cachedFramePositions_[cachedPosition] = filePosition;
        }

        // For better debug output
        cachedPosition--;
        positionCachedFor_ = cachedPosition;
        cachedFrame_ = ret;

        if (frameCount_ < cachedTime) {
            frameCount_ = cachedTime;
            needToEmit = true;
        }
    } else {
        auto it = std::upper_bound(numbersAndPositions_.begin(),
                                   numbersAndPositions_.end(),
                                   time);
        if (it != numbersAndPositions_.begin())
            --it;
        cachedPosition = std::distance(numbersAndPositions_.begin(), it);
        cachedTime = *it;

        if (positionCachedFor_ == cachedPosition) {
            ret = cachedFrame_;
        } else {
            file_.seekg(cachedFramePositions_[cachedPosition]);
            ret = readFrameContents({ cachedPosition, cachedTime });

            positionCachedFor_ = cachedPosition;
            cachedFrame_ = ret;
        }
    }

//    qDebug() << cachedPosition << " --- " << time;

    lock.unlock();
    if (needToEmit)
        emit frameCountChanged(frameCount_);

    return ret;
}

frameNumber_t PDBSimulationLayer::getNextTime(frameNumber_t time)
{
    QMutexLocker lock(&accessMutex_);

    const auto end = numbersAndPositions_.end();
    auto it = std::lower_bound(numbersAndPositions_.begin(), end, time);
    auto oldit = it;
    while (it != end && *it == time)
        ++it;
    return (it == end) ? *oldit : *it;
}

frameNumber_t PDBSimulationLayer::getPreviousTime(frameNumber_t time)
{
    QMutexLocker lock(&accessMutex_);

    if (time <= 0)
        return time;

    const auto begin = numbersAndPositions_.begin();
    const auto end = numbersAndPositions_.end();
    auto it = std::lower_bound(begin, end, time);
    while (it != begin && *it >= time)
        --it;
    return *it;
}

bool PDBSimulationLayer::reachedEndOfFile() const
{
    return reachedEndOfFile_;
}

#include "defaults.h"

Atom PDBSimulationLayer::getAtomFromString(const std::string & str)
{
    Atom a;
    int x, y, z;
    char type[4];

    if (str[21] != ' ') {
        sscanf(str.c_str(), "ATOM %d %*c %s %*c %*d %d.%*s %d.%*s %d.%*s",
            &a.id, type, &x, &y, &z);
    } else {
        sscanf(str.c_str(), "ATOM %d %*c %s %*d %d.%*s %d.%*s %d.%*s",
            &a.id, type, &x, &y, &z);
    }
    a.type = Defaults::rs2typename(type);
    a.x = x;
    a.y = y;
    a.z = z;

    return a;
}
