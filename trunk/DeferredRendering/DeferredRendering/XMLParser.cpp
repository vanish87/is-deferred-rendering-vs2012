#include "XMLParser.h"

#include <fstream>
#include <iostream>
#include <string>

namespace MocapGE
{
	XMLParser::XMLParser(void)
	{
	}


	XMLParser::~XMLParser(void)
	{
	}
	bool XMLParser::LoadFile(std::string const & file_name)
	{
		std::string line;
		std::ifstream file;
		file.open(file_name);
		if (file.is_open())
		{
			while ( !file.eof() )
			{
				file>>line;
				file_str_.append(" ");
				file_str_.append(line);
			}
			file.close();
		}
		else
			return false;
		char* xml_file=const_cast<char*>(file_str_.c_str());
		xml_file[file_str_.length()] = '\0';
		doc_.parse<0>(xml_file);
		return true;
	}
	XMLNodePtr XMLParser::GetRoot(std::string const & node_name)
	{
		return boost::shared_ptr<XMLNode>(new XMLNode(doc_.first_node(node_name.c_str())));
	}


	XMLNodePtr XMLNode::GetChild( std::string const & node_name )
	{
		return XMLNodePtr(new XMLNode(node_->first_node(node_name.c_str())));
	}

	XMLNodePtr XMLNode::NextNode()
	{
		return XMLNodePtr(new XMLNode(node_->next_sibling()));
	}

	XMLAttrPtr XMLNode::GetAttri( std::string const & attri_name )
	{
		return XMLAttrPtr(new XMLAttribute(node_->first_attribute(attri_name.c_str())));
	}

	XMLNode::XMLNode( void )
	{

	}

	XMLNode::~XMLNode( void )
	{

	}

	std::string XMLNode::GetValue()
	{
		return node_->value();
	}


	std::string XMLAttribute::GetValue()
	{
		return attri_->value();
	}

	XMLAttrPtr XMLAttribute::NextAttribute()
	{
		return XMLAttrPtr(new XMLAttribute(attri_->next_attribute()));
	}

}
