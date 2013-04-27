#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#pragma once

#include "PreDec.h"
#include <boost\shared_ptr.hpp>
#include <rapidxml.hpp>

namespace MocapGE
{
	class XMLNode;
	typedef boost::shared_ptr<XMLNode> XMLNodePtr;
	class XMLAttribute;
	typedef boost::shared_ptr<XMLAttribute> XMLAttrPtr;

	class XMLParser
	{

	public:
		XMLParser(void);
		~XMLParser(void);

		bool LoadFile(std::string const & file_name);
		XMLNodePtr GetRoot(std::string const & node_name);

	private:
		rapidxml::xml_document<> doc_;
		std::string file_str_;

	};

	class XMLNode
	{
	public:
		XMLNode(void);
		~XMLNode(void);

		XMLNode(rapidxml::xml_node<> const * node_ptr)
			:node_(node_ptr)
		{
		}
		XMLNodePtr GetChild(std::string const & node_name);
		XMLNodePtr NextNode();
		XMLAttrPtr GetAttri(std::string const & attri_name);
		std::string GetValue();
	private:
		rapidxml::xml_node<> const * node_;
	};
	class XMLAttribute
	{
	public:
		XMLAttribute(rapidxml::xml_attribute<> const * attri_ptr)
			:attri_(attri_ptr)
		{
		}
		
		std::string GetValue();
		XMLAttrPtr NextAttribute();
	private:
		rapidxml::xml_attribute<> const * attri_;
	};
}

#endif

