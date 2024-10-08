#include "./../../inc/generateXML.h"
#include "./../../inc/nodes.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include <td/String.h>
#include "xml/Writer.h"
#include <map>
#include <utility>
#include <xml/DOMParser.h> 
#include <xml/SAXParser.h> 

#define INDENT_CHAR "\t"

class nameNode : public baseNode {
	td::String name;
public:
	nameNode(const td::String &name) : name(name) {}
	nameNode(td::String &&name) : name(name) {}
	nameNode(const nameNode& node) :
		baseNode(node) {
		name = node.name;
	}
	baseNode* createCopy(const td::String& alias) override {
		return new nameNode(*this);
	}
	bool nodeAction(const td::String& command, baseNode*& newChild) override {
		return false;
	}

	inline const char* getName() const override{
		return name.c_str();
	}
};


class conditionNode : public baseNode {
public:
	enum class type { thenn, elsee };
private:
	type tip;
public:
	conditionNode(type);
	conditionNode(const conditionNode& node):
		baseNode(node){
		tip = node.tip;
	}
	baseNode* createCopy(const td::String& alias) override {
		return new conditionNode(*this);
	}
	void prettyPrint(cnt::StringBuilder<>& str, const td::String& indent) const override{
		str << indent << conditionNode::getName() << "\n";
		for (const auto& attrib : attribs)
			str << indent << INDENT_CHAR << attrib.first << " = " << attrib.second << "\n";
	}
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override;
	inline const char* getName() const override {
		return (tip == type::thenn) ? "Then" : "Else";
	}
};

conditionNode::conditionNode(type t) : tip(t) {}

static inline void addAlias(td::String &line, const td::String& alias) {
	if (alias.isNull())
		return;
	std::cmatch match;
	cnt::StringBuilderSmall str;

	const td::UTF8* start = line.begin();
	const td::UTF8* previousStart = start;
	while (std::regex_search(start, line.end(), match, baseNode::varPatten)) {
		str << line.subStr(previousStart - line.begin(), match[1].first - line.begin()) << alias << ".";
		previousStart = start;
		start = match.suffix().first;
	}
	str.getString(line);
}

class singleEquation : public baseNode {
	bool added = false;
	bool consumeEnd = false;
public:
	baseNode* createCopy(const td::String& alias) override {
		singleEquation &equation = *new singleEquation(*this);
		
		if (auto itFx = equation.attribs.find("fx"); itFx != equation.attribs.end())
			addAlias(itFx->second, alias);
		if (auto itW = equation.attribs.find("w"); itW != equation.attribs.end())
			addAlias(itW->second, alias);
		
		return &equation;
	}

	void prettyPrint(cnt::StringBuilder<>& str, const td::String& indent) const override{
		if (auto itFx = attribs.find("fx"); itFx != attribs.end())
			str << indent << itFx->second;
		if (auto itW = attribs.find("w"); itW != attribs.end())
			str << indent << " [ " << itW->second << " ]";
		
		str << indent << "\n";

		for (const auto& attrib : attribs) {
			if (attrib.first.cCompare("fx") == 0)
				continue;
			else if (attrib.first.cCompare("w") == 0)
				continue;
			str << indent << INDENT_CHAR << attrib.first << " = " << attrib.second << "\n";
		}
	}

	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override {
		if (command.beginsWithCI("then"))
			return true;


		if (command.beginsWithCI("end")) {
			if (consumeEnd) {
				added = true;
				consumeEnd = false;
				return true;
			}
			return false;
		}

		if (added)
			return false;

		if (command.beginsWithCI("else")) {
			nodes.push_back(new conditionNode(conditionNode::type::elsee));
			newChild = nodes.back();
			added = true;
			consumeEnd = true;
			return true;
		}

		if (command.beginsWithCI("if")) {
			setAttrib("cond", command.subStr(2, -1).trimLeft());
			nodes.push_back(new conditionNode(conditionNode::type::thenn));
			newChild = nodes.back();
			consumeEnd = true;
			return true;
		}

		added = true;
		int wPoz = command.find("[");

		if (wPoz != -1 && command.getLastChar() == ']') {
			setAttrib("fx", command.subStr(0, wPoz - 1).trimRight());
			setAttrib("w", command.subStr(wPoz + 1, command.length() - 2));
		}
		else
			setAttrib("fx", command);



		return true;
	}
	inline const char* getName() const override {
		return "Eq";
	}
};


struct GroupName { static const char* val; };
const char* GroupName::val = "Group";

template<typename containerName, typename nodeType>
class containerNode : public baseNode {
	bool done = false;
public:
	containerNode(){}
	containerNode(const containerNode<containerName, nodeType>& node) = default;
	baseNode* createCopy(const td::String &alias) override {
		return new containerNode<containerName, nodeType>(*this);
	}
	bool nodeAction(const td::String& command, baseNode*& newChild) override{
		if (done) return false;

		if (command.cCompareNoCase("Group:") == 0) {
			typedef containerNode<GroupName, singleEquation> GroupNode;
			nodes.push_back(new GroupNode);
			newChild = nodes.back();
			return true;
		}
		if (command.getLastChar() == ':') return false;
		if (command.beginsWithCI("end")) {
			//done = true;
			return false;
		}



		newChild = new nodeType;
		nodes.push_back(newChild);

		return false;

	}
	
	inline const char* getName() const override {
		return containerName::val;
	}

};

template<typename variableName>
class variableNode : public baseNode {
	bool added = false;
public:
	virtual baseNode* createCopy(const td::String &alias) override{
		auto &var = *new variableNode<variableName>(*this);
		if (auto it = var.attribs.find("name"); it != var.attribs.end())
			addAlias(it->second, alias);
		return &var;
	}
	void prettyPrint(cnt::StringBuilder<>& str, const td::String& indent) const override {
		if (auto it = attribs.find("name"); it != attribs.end())
			str << indent << it->second;
		if (auto it = attribs.find("val"); it != attribs.end())
			str << " = " << it->second;
		str << indent << "\n";
		for (const auto& attrib : attribs) {
			if (attrib.first.cCompare("name") == 0)
				continue;
			else if (attrib.first.cCompare("val") == 0)
				continue;
			str << indent << INDENT_CHAR << attrib.first << " = " << attrib.second << "\n";
		}
	}
	bool nodeAction(const td::String& command, baseNode*& newChild) override {
		if (added) return false;
		added = true;

		int pozEq = command.find("=");
		if (pozEq == -1) {
			setAttrib("name", command);
			return true;
		}

		setAttrib("name", command.subStr(0, pozEq - 1).trimRight());
		setAttrib("val", command.subStr(pozEq + 1, -1).trimLeft());
		
		return true;
	}

	inline const char* getName() const override {
		return variableName::val;
	}
};







bool conditionNode::nodeAction(const td::String& command, baseNode*& newChild){

	if (command.beginsWithCI("end"))
		return false;
	
	
	if (command.findCI("else") != -1)
		return false;
	
	nodes.push_back(new singleEquation());
	newChild = nodes.back();

	return false;
}



struct varsName { static const char* val; };
const char* varsName::val = "Vars";

struct varName { static const char* val; };
const char* varName::val = "Var";

struct paramsName { static const char* val; };
const char* paramsName::val = "Params";

struct paramName { static const char* val; };
const char* paramName::val = "Param";

struct NLEName { static const char* val; };
const char* NLEName::val = "NLEqs";

struct ODEName { static const char* val; };
const char* ODEName::val = "ODEqs";

struct postProcName { static const char* val; };
const char* postProcName::val = "PostProc";

struct MeasEqName { static const char* val; };
const char* MeasEqName::val = "MeasEqs";

struct LimitName { static const char* val; };
const char* LimitName::val = "Limits";

struct ECsName { static const char* val; };
const char* ECsName::val = "ECs";

struct TfName { static const char* val; };
const char* TfName::val = "TFs";

typedef containerNode<varsName, variableNode<varName>> varsNode;
typedef containerNode<paramsName, variableNode<paramName>> paramsNode;
typedef containerNode<NLEName, singleEquation> NLEquationsNode;
typedef containerNode<ODEName, singleEquation> ODEquationsNode;
typedef containerNode<postProcName, singleEquation> postProcNode;
typedef containerNode<MeasEqName, singleEquation> MeasEqNode;

typedef containerNode<LimitName, singleEquation> LimitNode;
typedef containerNode<ECsName, singleEquation> ECsNode;
typedef containerNode<TfName, singleEquation> TFsNode;


class initNode : public baseNode {
public:
	initNode(){}
	initNode(const initNode& node) = default;
	baseNode* createCopy(const td::String &alias) override {
		return new initNode(*this);
	}
	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override {

		if (command.cCompareNoCase("model:") == 0) {
			nodes.push_back(new modelNode());
			newChild = nodes.back();
			return true;
		}
		
		return false;
	}
	inline const char* getName() const override {
		return "Init";
	}
};


modelNode::modelNode(const modelNode& model):
	baseNode(model)
{
	done = model.done;
}

modelNode &modelNode::operator=(const modelNode &model)
{
    this->~modelNode();
	new(this) modelNode(model);
	return *this;
}

modelNode::modelNode(td::String text)
{
    processCommands(text);
}

bool modelNode::nodeAction(const td::String& command, baseNode*& newChild){
	if (done) return false;

	if (command.beginsWithCI("end")) {
		done = true;
		return true;
	}


	if (command.getLastChar() == ':') { //radi se o novom elementu
		if (command.cCompareNoCase("Vars:") == 0)
			nodes.push_back(new varsNode());
		else if (command.cCompareNoCase("Params:") == 0)
			nodes.push_back(new paramsNode());
		else if (command.cCompareNoCase("model:") == 0)
			return true;
		else if (command.cCompareNoCase("NLEqs:") == 0 || command.cCompareNoCase("NL:") == 0)
			nodes.push_back(new NLEquationsNode());
		else if (command.cCompareNoCase("ODEqs:") == 0 || command.cCompareNoCase("ODE:") == 0)
			nodes.push_back(new ODEquationsNode());
		else if (command.cCompareNoCase("Init:") == 0)
			nodes.push_back(new initNode);
		else if (command.cCompareNoCase("PostProc:") == 0)
			nodes.push_back(new postProcNode);
		else if (command.cCompareNoCase("MeasEqs:") == 0 || command.cCompareNoCase("Meas:") == 0)
			nodes.push_back(new MeasEqNode);
		else if (command.cCompareNoCase("Limits:") == 0)
			nodes.push_back(new LimitNode);
		else if (command.cCompareNoCase("ECs:") == 0 || command.cCompareNoCase("EC:") == 0)
			nodes.push_back(new ECsNode);
		else if (command.cCompareNoCase("TFs:") == 0 || command.cCompareNoCase("TF:") == 0)
			nodes.push_back(new TFsNode);
		else
			nodes.push_back(new nameNode(command.subStr(0, command.length() - 2)));

		newChild = nodes.back();
		return true;
	}

	return false;
}

modelNode &modelNode::addWtih(const modelNode &model, const td::String &alias)
{

    bool found;
	for (const auto& n : model.nodes) {
		found = false;
		for (const auto& thisNode : nodes)
			if (std::strcmp(thisNode->getName(), n->getName()) == 0) {
				found = true;
				for (const auto& child : n->nodes)
					thisNode->nodes.push_back(child->createCopy(alias));
			}
			
		if (!found)
			nodes.push_back(n->createCopy(alias));
	}
	return *this;
}

void modelNode::clear(){
	baseNode::clear();
	done = false;
}



static void addNodeFromXML(xml::FileParser::node_iterator &it, baseNode &node){
	td::String name;

	while(!it.end()){
		name = it->getName();
		if(name.cCompare("Model") == 0)
			node.nodes.emplace_back(new modelNode);
		else if(name.cCompare("Vars") == 0)
			node.nodes.emplace_back(new varsNode);
		else if(name.cCompare("Var") == 0)
			node.nodes.emplace_back(new variableNode<varName>);
		else if(name.cCompare("Params") == 0)
			node.nodes.emplace_back(new paramsNode);
		else if(name.cCompare("Param") == 0)
			node.nodes.emplace_back(new variableNode<paramName>);
		else if(name.cCompare("NLEqs") == 0)
			node.nodes.emplace_back(new NLEquationsNode);
		else if(name.cCompare("Eq") == 0)
			node.nodes.emplace_back(new singleEquation);
		else if(name.cCompare("Then") == 0)
			node.nodes.emplace_back(new conditionNode(conditionNode::type::thenn));
		else if(name.cCompare("Else") == 0)
			node.nodes.emplace_back(new conditionNode(conditionNode::type::elsee));
		else if(name.cCompare("Group") == 0)
			node.nodes.emplace_back(new containerNode<GroupName, singleEquation>);
		else if(name.cCompare("ODEqs") == 0)
			node.nodes.emplace_back(new ODEquationsNode);
		else if(name.cCompare("Init") == 0)
			node.nodes.emplace_back(new initNode);
		else if(name.cCompare("PostProc") == 0)
			node.nodes.emplace_back(new postProcNode);
		else if(name.cCompare("MeasEqs") == 0)
			node.nodes.emplace_back(new MeasEqNode);
		else if(name.cCompare("Limits") == 0)
			node.nodes.emplace_back(new LimitNode);
		else if(name.cCompare("ECs") == 0)
			node.nodes.emplace_back(new ECsNode);
		else if(name.cCompare("TFs") == 0)
			node.nodes.emplace_back(new TFsNode);
		else{
			node.nodes.emplace_back(new nameNode(name));
			throw (modelNode::exceptionInvalidBlockName) name;
		}

		for(const auto &at : it->attribs)
			node.nodes.back()->attribs[at.getName()] = at.getValue();

		auto childIter = it.getChildNode();
		addNodeFromXML(childIter, *node.nodes.back());

		++it;
	}

}

bool modelNode::readFromFile(const td::String &path)
{
    clear();
	xml::FileParser par;
	par.parseFile(path);
	if(!par.isOk())
		return false;
	auto root = par.getRootNode();
	if(root->getName().cCompareNoCase("Model") != 0)
		return false;

	for(const auto &at : root->attribs)
		attribs[at.getName()] = at.getValue();

	auto ch = root.getChildNode();
	addNodeFromXML(ch, *this);
	return true;

}

baseNode *modelNode::createCopy(const td::String& alias)
{
	return new modelNode(*this);
}


void generateXML(const td::String& equations, const td::String& output_path) {
	modelNode m(equations);

	xml::Writer w(output_path);
	w.startDocument();

	m.printNode(w);

	w.endDocument();
	w.close();
}

