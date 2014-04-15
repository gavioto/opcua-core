/******************************************************************************
 *   Copyright (C) 2014-2014 Olivier Roulet-Dubonnet          *
 *   olivier.roulet@gmail.com          *
 *                      *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as      *
 *   published by the Free Software Foundation; version 3 of the License.   *
 *                      *
 *   This library is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 *   GNU Lesser General Public License for more details.        *
 *                      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the          *
 *   Free Software Foundation, Inc.,              *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.        *
 ******************************************************************************/

#ifndef NODE_H
#define NODE_H

#include <sstream>

#include <opc/ua/server.h>



namespace OpcUa
{
  //A Node object represent an OPC-UA node. It is high level object intended for developper who want to expose
  // data through OPC-UA or read data from an OPCUA server.
  //Node are usually not create directly but obtained through call 
  // to GetRootNode og GetObjectsNode on server or client side
  class Node
  {
    public:
      Node( OpcUa::Remote::Server* server, const NodeID& nodeid ) {
        this->server = server;
        this->NodeId = nodeid;
        mIsNull = false;
      }
      Node( OpcUa::Remote::Server* server, const NodeID& nodeid, bool isNull ) {
        this->server = server;
        this->NodeId = nodeid;
        mIsNull = isNull;
      }
      Node( OpcUa::Remote::Server* server){ this->server = server; mIsNull = true; };
      //~Node(){};

      bool IsNull() const { return mIsNull; }
      NodeID GetNodeId() const {return NodeId;}
      OpcUa::Remote::Server* GetServer() const {return server;}


      //The Browse methods return "childs" of a node. Optionnnaly the reference type can be spcified
      std::vector<Node> Browse(OpcUa::ReferenceID refid);
      std::vector<Node> Browse() {return this->Browse(OpcUa::ReferenceID::HierarchicalReferences);}
      std::vector<Node> GetProperties() {return Browse(OpcUa::ReferenceID::HasProperty);}
      std::vector<Node> GetVariables() {return Browse(OpcUa::ReferenceID::HasComponent);} //Not correct should filter by variable type

          
      //The Read and Write methods read or write attributes of the node
      //FIXME: add possibility to read and write several nodes at once
      Variant Read(OpcUa::AttributeID attr);
      StatusCode Write(OpcUa::AttributeID attr, const Variant &val);
      //std::vector<StatusCode> WriteAttrs(OpcUa::AttributeID attr, const Variant &val);
      StatusCode WriteValue(const Variant &variant);
      Variant ReadValue() { return Read(OpcUa::AttributeID::VALUE);}
      Variant ReadDataType() {return Read(OpcUa::AttributeID::DATA_TYPE);}
      QualifiedName ReadBrowseName() 
      {
        Variant var = Read(OpcUa::AttributeID::BROWSE_NAME); 
        if (var.Type == OpcUa::VariantType::QUALIFIED_NAME)
        {
          return var.Value.Name.front();
        }
        return QualifiedName();
      }
      void SetBrowseNameCache(const QualifiedName &browsename){this->browseName= browsename;}  
      void WriteBrowseName (const QualifiedName &browsename); 
      QualifiedName GetBrowseNameCache() const { return browseName; }
                
      //The GetChildNode methods return a node defined by its path from the node. A path is defined by
      // a sequence of browse name(QualifiedName). A browse name is either defined through a qualifiedname object
      // or a string of format namespace:browsename. If a namespace is not specified it is assumed to be
      //the same as the parent
      Node GetChildNode (std::vector<QualifiedName> const &path);
      Node GetChildNode (const std::vector<std::string> &path); 
      Node GetChildNode (const std::string &browsename) {return GetChildNode(std::vector<std::string>({browsename}));}

      std::string ToString() const; 
      explicit operator bool() const {return !mIsNull;}
      bool operator==(Node const& x) const { return NodeId == x.NodeId; }
      bool operator!=(Node const& x) const { return NodeId != x.NodeId; }
      
      //OpcUa low level methods to to modify address space model
      void AddAttribute(OpcUa::AttributeID attr, const OpcUa::Variant val){return server->AddressSpace()->AddAttribute(this->NodeId, attr, val);}
      void AddReference(const OpcUa::ReferenceDescription desc) {return server->AddressSpace()->AddReference(this->NodeId, desc);}
      //Helper classes to modify address space model
      Node AddFolder(const NodeID& nodeid, const QualifiedName& browsename);
      Node AddFolder(const std::string& name); 
      Node AddVariable(const NodeID& nodeid, const QualifiedName& browsename, const Variant& val);
      Node AddVariable(const std::string& name, const Variant& val); 
      Node AddProperty(const NodeID& nodeid, const QualifiedName& browsename, const Variant& val);
      Node AddProperty(const std::string& name, const Variant& val); 

    private:
      //We cnnot use shared pointer because we donnot own them 
      //they can be invalidated when  share libs are unloaded
      //OpcUa::Remote::Server::SharedPtr server;
      OpcUa::Remote::Server* server;
      bool mIsNull = true;
      NodeID NodeId;
      QualifiedName browseName ;
      QualifiedName ParseQualifiedNameFromString(const std::string& str, ushort default_ns);
      friend std::ostream& operator<<(std::ostream& os, const Node& node){
        os << node.ToString();
        return os;
      //OpcUa::Server::AddressSpaceRegistry:: registry
      //void createNode(NodeID nodeid, QualifiedName browsename);

      }

  };

  //FIXME: This should be somewhere else, maybe wariant.h. And maybe there is another way or this is wrong
  ObjectID VariantTypeToDataType(VariantType vt)
  {
    switch (vt)
    {
      case VariantType::BOOLEAN:          return ObjectID::Boolean;   
      case VariantType::SBYTE:            return ObjectID::SByte;  
      case VariantType::BYTE:             return ObjectID::Byte;
      case VariantType::INT16:            return ObjectID::Int16;
      case VariantType::UINT16:           return ObjectID::UInt16;
      case VariantType::INT32:            return ObjectID::Int32;
      case VariantType::UINT32:           return ObjectID::UInt32;
      case VariantType::INT64:            return ObjectID::Int64;
      case VariantType::UINT64:           return ObjectID::UInt64;
      case VariantType::FLOAT:            return ObjectID::Float;
      case VariantType::DOUBLE:           return ObjectID::Double;
      case VariantType::STRING:           return ObjectID::String;
      case VariantType::DATE_TIME:        return ObjectID::DateTime;
      case VariantType::GUID:             return ObjectID::Guid;
      case VariantType::BYTE_STRING:      return ObjectID::ByteString;
      case VariantType::XML_ELEMENT:      return ObjectID::XmlElement;
      case VariantType::NODE_ID:          return ObjectID::NodeID;
      case VariantType::EXPANDED_NODE_ID: return ObjectID::ExpandedNodeID;
      case VariantType::STATUS_CODE:      return ObjectID::StatusCode;
      case VariantType::QUALIFIED_NAME:   return ObjectID::QualifiedName;
      case VariantType::LOCALIZED_TEXT:   return ObjectID::LocalizedText;
      case VariantType::DIAGNOSTIC_INFO:  return ObjectID::DiagnosticInfo;
      case VariantType::DATA_VALUE:       return ObjectID::DataValue;
      case VariantType::NUL:              return ObjectID::Null;
      case VariantType::EXTENSION_OBJECT:
      case VariantType::VARIANT:          
      default:
        throw std::logic_error("Unknown variant type.");
    }
  }



} // namespace OpcUa

#endif
