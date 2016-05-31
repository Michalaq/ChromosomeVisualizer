#include <sstream>
#include <cassert>
#include "PDBSimulationLayer.h"

PDBSimulationLayer::PDBSimulationLayer(const std::string &name, const std::string &fileName)
    : fileName_(fileName)
    , file_(fileName)
    , reachedEndOfFile_(false)
    , SimulationLayer(name)
{
    cachedFramePositions_[0] = 0;
}

PDBSimulationLayer::PDBSimulationLayer(const std::string & fileName)
    : PDBSimulationLayer(fileName, fileName)
{}

std::shared_ptr<Frame> PDBSimulationLayer::getFrame(frameNumber_t position)
{
    // assert(frameBound_ == 0 || position < frameBound_);
    if (reachedEndOfFile() && position > frameCount_ - 1)
        position = frameCount_ - 1;

	auto it = cachedFramePositions_.upper_bound(position);

    // Find cached frame before position
    if (it != cachedFramePositions_.begin())
        --it;

	frameNumber_t cachedPosition = it->first;
	assert(cachedPosition <= position);

	file_.seekg(it->second, file_.beg);

    std::shared_ptr<Frame> ret, prev;
    prev = cachedFrame_ ? cachedFrame_ : std::make_shared<Frame>();
	while (cachedPosition <= position) {
		ret = readCurrentFrame();
        if (!ret) {
            // Most probably there was a read error
            // because the end of file has been checked before
            return prev;
        }

        cachedPosition++;
		cachedFramePositions_[cachedPosition] = file_.tellg();
        prev = ret;
	}

    if (!reachedEndOfFile_ && position >= frameCount_) {
        // Check if the next frame is reachable
        if (readCurrentFrame() == nullptr)
            reachedEndOfFile_ = true;

        frameCount_ = position + 1;
        emit frameCountChanged(frameCount_);
    }

	return ret;
}

bool PDBSimulationLayer::reachedEndOfFile() const
{
    return reachedEndOfFile_;
}

static std::pair<int, int> getStepInfo(const std::string & line)
{
	int no, step;
	sscanf(line.c_str(), "HEADER %d %*d step %d", &no, &step);
	return std::pair<int, int>(no, step);
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

std::shared_ptr<Frame> PDBSimulationLayer::readCurrentFrame()
{
	std::string line;
	getline(file_, line); // HEADER
	int no, step;
	auto vals = getStepInfo(line);
    no = vals.first; step = vals.second;
	getline(file_, line); // TITLE
	std::map<std::string, float> functionValues = getFunctionValues(line);
	std::vector<Atom> atoms;
    getline(file_, line);
    int currentConnectionCount = 0;
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

Atom PDBSimulationLayer::getAtomFromString(const std::string & str)
{
	Atom a;
    int x, y, z;

    sscanf(str.c_str(), "ATOM %d %*c %s %*d %d.0 %d.0 %d.0",
        &a.id, &a.type, &x, &y, &z);

    a.x = x;
    a.y = y;
    a.z = z;

    return a;
}
