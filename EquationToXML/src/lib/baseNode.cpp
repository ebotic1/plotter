#include "./../../inc/nodes.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include "td/String.h"

#define BACK_COMMENT_CHAR "¸"


void baseNode::printNode(xml::Writer& w) {
	w.startNode(this->getName());
	for each (auto & at in this->attribs)
		w.attribute(at.first.c_str(), at.second);



	if (nodes.size() == 0)
		w.endNode();

	cnt::PushBackVector<td::String> comSides;

	if (!comment.isNull()) {
		cnt::PushBackVector<td::String> coms;
		comment.split(BACK_COMMENT_CHAR, comSides, true, true);

		comSides[0].split("\n", coms, true, true);
		if (!coms[0].isNull())
			w.comment(coms[0].c_str());
		for (int i = 1; i < coms.size(); ++i)
			if (!coms[i].isNull())
				w.comment(coms[i].c_str()); //treba commentInNewLine ali kad se popravi funkcija

	}

	if (nodes.size() != 0) {

		for each (baseNode * var in nodes)
			var->printNode(w);

		w.endNode();
	}

	//'BACK_COMMENT_CHAR' oznacava da se komentar printa na kraju elementa
	cnt::PushBackVector<td::String> coms;
	for (int i = 1; i < comSides.size(); ++i)
		comSides[i].split("\n", coms, true, false);
	for (int i = 0; i < coms.size(); ++i)
		if (!coms[i].isNull())
			w.commentInNewLine(coms[i].c_str());

}

class baseNode::Messages {
	std::vector<std::pair<td::String, td::String>> commands;
	int current;
public:

	std::pair<td::String, td::String>& getCommand() {
		return commands[current];
	}

	std::pair<td::String, td::String>& getNextCommand() {
		return commands[++current];
	}

	bool addCommands(bool stopExecution = false) {
		if (current > 10) { //nema potrebe za oslobadanjem memorije ako imamo manje od 10 komandi
			commands.erase(commands.begin(), commands.begin() + current);
			current = 0;
		}
		mtx
	}

};

size_t baseNode::addLine(std::vector<std::pair<td::String, td::String>> lines, size_t startLine) {
	while (startLine < lines.size()) {
		if (lines[startLine].first.isNull() && !lines[startLine].second.isNull()) {
			if (nodes.size() != 0) {
				nodes.back()->addComment(BACK_COMMENT_CHAR);
				nodes.back()->addComment(lines[startLine].second);
			}
			else {
				addComment("\n");
				addComment(lines[startLine].second);
			}
			++startLine;
			continue;
		}

		int pozEq = lines[startLine].first.find("=");

		bool found = false;
		if (pozEq != -1) {
			td::String keyword = lines[startLine].first.subStr(0, pozEq - 1).trimRight();
			for each (auto & var in attributeKeywords) {
				if (var == keyword) {
					setAttrib(lines[startLine].first.subStr(0, pozEq - 1).trimRight(), lines[startLine].first.subStr(pozEq + 1, -1).trimLeft());
					addComment(lines[startLine].second);
					found = true;
					break;
				}
			}
		}
		if (found) {
			++startLine;
			continue;
		}
		baseNode* child = nullptr;
		if (nodeAction(lines[startLine].first, child)) {
			if (child != nullptr) {
				child->addComment(lines[startLine].second);
				startLine = child->addLine(lines, startLine + 1);
				continue;
			}
			addComment(lines[startLine].second);
			++startLine;
			continue;
		}

		if (child != nullptr) {
			startLine = child->addLine(lines, startLine);
			continue;
		}

		break;
	}
	return startLine;
}

void baseNode::addComment(const td::String& comment) {
	if (comment.isNull())
		return;
	this->comment += comment;
}

void baseNode::addComment(td::String&& comment) {
	if (comment.isNull())
		return;
	if (this->comment.isNull())
		this->comment = comment;
	else {
		this->comment += comment;
	}
}

void baseNode::processCommands(const td::String& text) {
	std::vector<std::pair<td::String, td::String>> lines;
	cnt::PushBackVector<td::String> l;
	text.split("\n;", l, true, true);
	int poz;
	for (int i = 0; i < l.size(); ++i) {
		poz = l[i].find("//");
		if (poz == -1)
			lines.emplace_back<std::pair<td::String, td::String>>({ l[i].trim(), "" });
		else if (poz != 0)
			lines.emplace_back<std::pair<td::String, td::String>>({ l[i].subStr(0,poz - 1).trim(), l[i].subStr(poz + 2, -1) });
		else
			lines.emplace_back<std::pair<td::String, td::String>>({ "", l[i].subStr(poz + 2, -1) });
	}
	addLine(lines);
}
