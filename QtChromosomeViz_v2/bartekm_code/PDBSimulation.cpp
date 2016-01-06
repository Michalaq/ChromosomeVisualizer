#include <sstream>
#include <cassert>
#include "PDBSimulation.h"

PDBSimulation::PDBSimulation(const std::string &name, const std::string &fileName)
    : fileName_(fileName)
    , file_(fileName)
    , Simulation(name)
{
    cachedFramePositions_[0] = 0;
}

PDBSimulation::PDBSimulation(const std::string & fileName)
    : PDBSimulation(fileName, fileName)
{}

std::shared_ptr<Frame> PDBSimulation::getFrame(frameNumber_t position)
{
	auto it = cachedFramePositions_.upper_bound(position);

    // Find cached frame before position
    if (it != cachedFramePositions_.begin())
        --it;

	frameNumber_t cachedPosition = it->first;
	assert(cachedPosition <= position);

	file_.seekg(it->second, file_.beg);

	std::shared_ptr<Frame> ret;
	while (cachedPosition <= position) {
		ret = readCurrentFrame();
		cachedPosition++;

		cachedFramePositions_[cachedPosition] = file_.tellg();
	}

    if (position >= frameCount_) {
        frameCount_ = position + 1;
        emit frameCountChanged(frameCount_);
    }

	return ret;
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

std::shared_ptr<Frame> PDBSimulation::readCurrentFrame()
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
    bool connectionStarted = false;
    int currentConnectionCount = 0;
	while (line.find("END") == std::string::npos) {
		if (line.substr(0, 4) == "ATOM")
			atoms.push_back(getAtomFromString(line));
        else if (connectionCount_< 0) {
            if (!connectionStarted && line.substr(0, 6) == "CONNECT")
                connectionStarted = true;
            if (connectionStarted)
                currentConnectionCount++;
        }
		getline(file_, line);
	}
    if (connectionCount_ < 0)
        connectionCount_ = currentConnectionCount;
	Frame d = {
		no,
		step,
		std::move(atoms),
		std::move(functionValues)
	};

	if (cachedFrame_.unique())
		*cachedFrame_ = std::move(d);
	else
		cachedFrame_ = std::make_shared<Frame>(d);

	return cachedFrame_;
}

Atom PDBSimulation::getAtomFromString(const std::string & str)
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
