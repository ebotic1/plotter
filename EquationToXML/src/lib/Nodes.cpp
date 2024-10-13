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
	nameNode(const nameNode& node, const td::String &alias ) :
		baseNode(node, alias) {
		name = node.name;
	}
	baseNode* createCopy(const td::String& alias) override {
		return new nameNode(*this, alias);
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
	conditionNode(const conditionNode& node, const td::String &alias):
		baseNode(node, alias){
		tip = node.tip;
	}
	baseNode* createCopy(const td::String& alias) override {
		return new conditionNode(*this, alias);
	}
	void prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override{
		if(getName()[0] != 'T'){ //not Then
			indent.reduceSize(1);
			str << indent << conditionNode::getName() << "\n";
			indent += INDENT_CHAR;
		}

		if (auto itFx = attribs.find("fx"); itFx != attribs.end())
			str  << indent << itFx->second << "\n";
		for (const auto& attrib : attribs)
			if(attrib.first.cCompare("fx") != 0)
				str  << indent << attrib.first << " = " << attrib.second << "\n";
	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override{
		const auto &nodes = getParent()->getNodes();
		for(int i = 0; i < nodes.size(); ++i)
			if(nodes[i] == this){
				if(i != nodes.size()-1)
					if(auto ptr = dynamic_cast<conditionNode *>(nodes[i+1]);  ptr != nullptr && ptr->tip == type::elsee)
						return;
				indent.reduceSize(1);
				str << indent << "end if\n";
				indent += INDENT_CHAR;
			}
	}

	virtual bool nodeAction(const td::String& command, baseNode*& newChild) override;
	inline const char* getName() const override {
		return (tip == type::thenn) ? "Then" : "Else";
	}
};

conditionNode::conditionNode(type t) : tip(t) {}


static td::String subStr(const td::String &str, int pozStart, int pozEnd){
	if(pozStart >= pozEnd)
		return td::String();
	return td::String(str.begin() + pozStart, pozEnd - pozStart);
}

static inline void addAlias(td::String &line, const td::String& alias) {
	if (alias.isNull())
		return;
	std::cmatch match;
	cnt::StringBuilderSmall str;

	const td::UTF8* start = line.begin();
	const td::UTF8* previousEnd = start;
	while (std::regex_search(start, line.end(), match, baseNode::varPatten)) {
		//str << td::String(previousEnd, match[1].first - line.begin() - previousEnd) << alias << ".";
		str << subStr(line, previousEnd - line.begin(), match[2].first - line.begin()) << alias << ".";
		previousEnd = match[2].first;
		start = match.suffix().first;
	}
	
	str << subStr(line, previousEnd - line.begin(), line.end() - line.begin());
	str.getString(line);
}

class singleEquation : public baseNode {
	bool added = false;
	bool consumeEnd = false;
public:
	singleEquation() = default;
	singleEquation(const singleEquation &n, const td::String &alias ):baseNode(n, alias)
	{
		added = n.added;
		consumeEnd = n.consumeEnd;
	}
	baseNode* createCopy(const td::String& alias) override {
		singleEquation &equation = *new singleEquation(*this, alias);
		
		if (auto itFx = equation.attribs.find("fx"); itFx != equation.attribs.end())
			addAlias(itFx->second, alias);
		if (auto itW = equation.attribs.find("w"); itW != equation.attribs.end())
			addAlias(itW->second, alias);
		if (auto it = equation.attribs.find("cond"); it != equation.attribs.end())
			addAlias(it->second, alias);
		
		return &equation;
	}

	void prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override{
		if (auto it = attribs.find("cond"); it != attribs.end()){
			str << indent << "if " << it->second;
			indent += INDENT_CHAR;
		}

		if (auto itFx = attribs.find("fx"); itFx != attribs.end())
			str << indent << itFx->second;
		if (auto itW = attribs.find("w"); itW != attribs.end())
			str << indent << " [ " << itW->second << " ]";
		
		str << "\n";

		for (const auto& attrib : attribs) {
			if (attrib.first.cCompare("fx") == 0)
				continue;
			else if (attrib.first.cCompare("w") == 0)
				continue;
			else if (attrib.first.cCompare("cond") == 0)
				continue;
			str << indent << attrib.first << " = " << attrib.second << "\n";
		}
	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override{
		if (auto it = attribs.find("cond"); it != attribs.end())
			indent.reduceSize(1);
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
	containerNode() = default;
	containerNode(const containerNode<containerName, nodeType>& node) = default;
	containerNode(const containerNode<containerName, nodeType>& node, const td::String &alias):
	baseNode(node,alias){}
	baseNode* createCopy(const td::String &alias) override {
		return new containerNode<containerName, nodeType>(*this, alias);
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
			done = true;
			return true;
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
	variableNode() = default;
	variableNode(const variableNode &n) = default;
	variableNode(const variableNode &n, const td::String &alias):
		baseNode(n,alias){}
	virtual baseNode* createCopy(const td::String &alias) override{
		auto &var = *new variableNode(*this, alias);
		if (auto it = var.attribs.find("name"); it != var.attribs.end())
			addAlias(it->second, alias);
		return &var;
	}
	void prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override {
		if (auto it = attribs.find("name"); it != attribs.end())
			str << indent << it->second;
		if (auto it = attribs.find("val"); it != attribs.end())
			str << " = " << it->second;
		str << "\n";
		for (const auto& attrib : attribs) {
			if (attrib.first.cCompare("name") == 0)
				continue;
			else if (attrib.first.cCompare("val") == 0)
				continue;
			str << indent << INDENT_CHAR << attrib.first << " = " << attrib.second << "\n";
		}
	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override{
		
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
	
	nodes.push_back(new singleEquation);
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
	initNode() = default;
	initNode(const initNode& node, const td::String &alias ):
		baseNode(node, alias){

		}
	baseNode* createCopy(const td::String &alias) override {
		return new initNode(*this, alias);
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


modelNode::modelNode(const modelNode& model, const td::String &alias):
	baseNode(model, alias)
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


	if (command.getLastChar() == ':' || true) { //radi se o novom elementu // uvijek true jer treba da baci exception jer ako je doslo do ove funkcije onda nije rijec o atributu tako da nek baci exception jer neka je greska
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
			throw exceptionInvalidBlockName(command);

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
				for (const auto& child : n->getNodes())
					thisNode->addChild(child->createCopy(alias));
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
			node.addChild(new modelNode);
		else if(name.cCompare("Vars") == 0)
			node.addChild(new varsNode);
		else if(name.cCompare("Var") == 0)
			node.addChild(new variableNode<varName>);
		else if(name.cCompare("Params") == 0)
			node.addChild(new paramsNode);
		else if(name.cCompare("Param") == 0)
			node.addChild(new variableNode<paramName>);
		else if(name.cCompare("NLEqs") == 0)
			node.addChild(new NLEquationsNode);
		else if(name.cCompare("Eq") == 0)
			node.addChild(new singleEquation);
		else if(name.cCompare("Then") == 0)
			node.addChild(new conditionNode(conditionNode::type::thenn));
		else if(name.cCompare("Else") == 0)
			node.addChild(new conditionNode(conditionNode::type::elsee));
		else if(name.cCompare("Group") == 0)
			node.addChild(new containerNode<GroupName, singleEquation>);
		else if(name.cCompare("ODEqs") == 0)
			node.addChild(new ODEquationsNode);
		else if(name.cCompare("Init") == 0)
			node.addChild(new initNode);
		else if(name.cCompare("PostProc") == 0)
			node.addChild(new postProcNode);
		else if(name.cCompare("MeasEqs") == 0)
			node.addChild(new MeasEqNode);
		else if(name.cCompare("Limits") == 0)
			node.addChild(new LimitNode);
		else if(name.cCompare("ECs") == 0)
			node.addChild(new ECsNode);
		else if(name.cCompare("TFs") == 0)
			node.addChild(new TFsNode);
		else{
			node.addChild(new nameNode(name));
			throw (modelNode::exceptionInvalidBlockName) name;
		}

		for(const auto &at : it->attribs)
			node.getNodes().back()->attribs[at.getName()] = at.getValue();


		auto childIter = it.getChildNode();
		addNodeFromXML(childIter, *node.getNodes().back());

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

