#include<unordered_map>
#include<map>
#include <utility>
#include "xml/Writer.h"
#include "cnt/PushBackVector.h"
#include "td/String.h"
#include <vector>

#include <thread>
#include <mutex>

class baseNode {

	static const td::String attributeKeywords[];
	class Messages;

	
	std::mutex mtx;

protected:
	std::map<td::String, td::String> attribs;
	std::vector<baseNode*> nodes;
	virtual bool nodeAction(const td::String &command, baseNode*& newChild) = 0;
	td::String comment;
	size_t addLine(std::vector<std::pair<td::String, td::String>> lines, size_t startLine = 0);

public:
	virtual void printNode(xml::Writer& w);
	virtual void setAttrib(const td::String& name, const td::String& val) {
		attribs[name] = val;
	}
	inline td::String& operator[](const td::String& name) {
		return attribs[name];
	}
	void addComment(const td::String& comment);
	void addComment(td::String&& comment);

	void processCommands(const td::String& text);
	virtual inline const char* getName() = 0;

	~baseNode() {
		for each (baseNode * var in nodes) {
			delete var;
		}
	}

};

const td::String baseNode::attributeKeywords[] = { "type", "domain", "name", "eps", "dT", "signal", "out", "desc" };

class modelNode : public baseNode {
	std::map<td::String, td::String> attribs;
	bool done = false;

public:
	static const td::String attributeKeywords[];
	modelNode() {};
	modelNode(td::String command);
	bool nodeAction(const td::String& command, baseNode*& newChild) override;
	inline const char* getName() override {
		return "Model";
	}

};


