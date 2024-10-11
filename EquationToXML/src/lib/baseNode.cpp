#include "./../../inc/nodes.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include "td/String.h"
#include <stack>

#define BACK_COMMENT_CHAR "ˇ"
#define INDENT_CHAR "\t"

const td::String baseNode::attributeKeywords[] = { "type", "domain", "name", "eps", "dT", "signal", "out", "desc", "method"};
const std::regex baseNode::varPatten = std::regex(R"((^|[^A-Za-z_\.])([a-zA-Z_](?:\w+?)?)(?:$|[^A-Za-z_\.]))");


void baseNode::printNodeToString(td::String& string) const{
	xml::Writer w;
	w.startDocument();
	printNode(w);
	w.getString(string);
}

bool baseNode::printNode(const td::String& path) const{
	xml::Writer w;
	w.open(path);
	if(!w.isOk())
		return false;
	w.startDocument();
	printNode(w);
	w.close();
	if(!w.isOk())
		return false;
	return true;
}

void baseNode::printNode(xml::Writer& w) const{
	w.startNode(this->getName());
	for (auto & at : this->attribs)
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
		for (baseNode * var : nodes)
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



void baseNode::prettyPrint(td::String& text) const
{
	cnt::StringBuilder str;
	td::String indent;
	
	std::stack<const baseNode*> stack;
	stack.push(this);
	const baseNode* current;

	while (!stack.empty()) {
		current = stack.top();
		
		if (current == nullptr) {
			stack.pop();
			stack.top()->prettyPrintClosing(str, indent);
			stack.pop();
			continue;
		}


		current->prettyPrint(str, indent);

		if(!current->comment.isNull()){
			str << current->comment << "\n";
		}
		



		if (!current->nodes.empty()) {
			stack.push(nullptr);

			for (auto it = current->nodes.rbegin(); it != current->nodes.rend(); ++it)
			stack.push(*it);

		}else{
			stack.pop();
			current->prettyPrintClosing(str, indent);
		}

		
	}

	str.getString(text);
}



size_t baseNode::addLine(std::vector<std::pair<td::String, td::String>> &lines, size_t startLine) {
	while (startLine < lines.size()) {
		if (lines[startLine].first.isNull() && !lines[startLine].second.isNull()) {
			if (lastChlid->nodes.size() != 0) {
				lastChlid->nodes.back()->addComment(BACK_COMMENT_CHAR);
				lastChlid->nodes.back()->addComment(lines[startLine].second);
			}
			else {
				lastChlid->addComment("\n");
				lastChlid->addComment(lines[startLine].second);
			}
			++startLine;
			continue;
		}

		int pozEq = lines[startLine].first.find("=");

		bool found = false;
		if (pozEq != -1) {
			td::String keyword = lines[startLine].first.subStr(0, pozEq - 1).trimRight();
			for (auto & var : attributeKeywords) {
				if (var == keyword) {
					lastChlid->setAttrib(lines[startLine].first.subStr(0, pozEq - 1).trimRight(), lines[startLine].first.subStr(pozEq + 1, -1).trimLeft());
					lastChlid->addComment(lines[startLine].second);
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
		if (lastChlid->nodeAction(lines[startLine].first, child)) {
			if (child != nullptr) {
				child->parent = lastChlid;
				child->addComment(lines[startLine].second);
				lastChlid = child;
				startLine = child->addLine(lines, startLine + 1);
				continue;
			}
			lastChlid->addComment(lines[startLine].second);
 			++startLine;
			continue;
		}

		if (child != nullptr) {
			child->parent = lastChlid;
			lastChlid = child;
			startLine = child->addLine(lines, startLine);
			continue;
		}

		break;
	}

	if (parent != nullptr)
		parent->lastChlid = parent;

	return startLine;
}

void baseNode::prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const
{
	if(parent != nullptr && !parent->attribs.empty())
		str << indent << "\n";
	str << indent << getName() << ":\n";
	indent += INDENT_CHAR;
	for (const auto& attrib : attribs)
		str << indent << attrib.first << " = " << attrib.second << "\n";

}

void baseNode::prettyPrintClosing(cnt::StringBuilder<> &str, td::String &indent) const
{
	indent.reduceSize(1);
}
void baseNode::addComment(const td::String &comment)
{
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

void baseNode::addChild(baseNode *childNode)
{
	nodes.emplace_back(childNode);
	nodes.back()->parent = this;
}
void baseNode::processCommands(const td::String &text)
{
    std::vector<std::pair<td::String, td::String>> lines;
	cnt::PushBackVector<td::String> l;
	parent = nullptr;
	text.split("\n;", l, true, true);
	int poz;
	for (int i = 0; i < l.size(); ++i) {
		l[i] = l[i].trim();
		if(l[i].isNull())
			continue;
		poz = l[i].find("//");
		if (poz == -1)
			lines.emplace_back<std::pair<td::String, td::String>>({ l[i], "" });
		else if (poz != 0)
			lines.emplace_back<std::pair<td::String, td::String>>({ l[i].subStr(0,poz - 1).trimRight(), l[i].subStr(poz + 2, -1).trimLeft()});
		else
			lines.emplace_back<std::pair<td::String, td::String>>({ "", l[i].subStr(poz + 2, -1).trimLeft()});
	}
	addLine(lines);
}

void baseNode::clear()
{
	for (const auto& node : nodes)
		delete node;
	nodes.clear();
	attribs.clear();
	comment.clean();
	lastChlid = this;
	parent = nullptr;
}

baseNode::baseNode(const baseNode &node, const td::String &alias)
{
	comment = node.comment;
	lastChlid = node.lastChlid;
	parent = node.parent;
	attribs = node.attribs;
	for (const auto& n : node.nodes)
		nodes.emplace_back(n->createCopy(alias));
}
const std::vector<baseNode *> &baseNode::getNodes()
{
    return nodes;
}

baseNode *baseNode::getParent() const
{
    return parent;
}
baseNode::baseNode()
{
}

baseNode::baseNode(const baseNode& node)
{
	comment = node.comment;
	lastChlid = node.lastChlid;
	parent = node.parent;
	attribs = node.attribs;
	for (const auto& n : node.nodes)
		nodes.emplace_back(n->createCopy(""));
}