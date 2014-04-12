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

#include <opc/ua/computer.h>



namespace OpcUa
{
  //A Node object represent an OPC-UA node. It is high level object intended for developper who want to expose
  // data through OPC-UA or read data from an OPCUA server.
  //Node are usually not create directly but obtained through call 
  // to GetRootNode og GetObjectsNode on server or client side
  class Node
  {
    public:
      Node( const OpcUa::Remote::Computer::SharedPtr& server, const NodeID& nodeid ) {
        this->server = server;
        this->NodeId = nodeid;
        mIsNull = false;
      }
      Node( const OpcUa::Remote::Computer::SharedPtr& server, const NodeID& nodeid, bool isNull ) {
        this->server = server;
        this->NodeId = nodeid;
        mIsNull = isNull;
      }
      Node( const OpcUa::Remote::Computer::SharedPtr& server){ this->server = server; mIsNull = true; };
      //~Node(){};

      bool IsNull() const { return mIsNull; }
      NodeID GetNodeId() const {return NodeId;}
      OpcUa::Remote::Computer::SharedPtr GetServer() const {return server;}


      //The Browse methods return "childs" of a node. Optionnnaly the reference type can be spcified
      std::vector<Node> Browse() {return this->Browse(OpcUa::ReferenceID::HierarchicalReferences);}
      std::vector<Node> Browse(OpcUa::ReferenceID refid);
      std::vector<Node> GetProperties() {return Browse(OpcUa::ReferenceID::HasProperty);}
      std::vector<Node> GetChildren() {return Browse();}
      std::vector<Node> GetVariables() {return Browse(OpcUa::ReferenceID::HasComponent);} //Not correct should filter by variable type

          
      //The Read and Write methods read or write attributes of the node
      Variant Read(OpcUa::AttributeID attr);
      StatusCode Write(OpcUa::AttributeID attr, const Variant &val);
      //std::vector<StatusCode> WriteAttrs(OpcUa::AttributeID attr, const Variant &val);
      StatusCode WriteValue(const Variant &variant);
      Variant ReadValue() { return Read(OpcUa::AttributeID::VALUE);}
      Variant ReadDataType() {return Read(OpcUa::AttributeID::DATA_TYPE);}
      QualifiedName ReadBrowseName() {
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
      Node GetChildNode (ushort ns, const std::string &browsename);
      Node GetChildNode (const QualifiedName &browsename);
      Node GetChildNode (const std::vector<std::string> &path); 
      Node GetChildNode (const std::string &browsename) {return GetChildNode(this->browseName.NamespaceIndex, browsename);}





      std::string ToString() const; 
      explicit operator bool() const {return !mIsNull;}
      bool operator==(Node const& x) const { return NodeId == x.NodeId; }
      bool operator!=(Node const& x) const { return NodeId != x.NodeId; }

      Node AddFolderNode(uint16_t ns, uint32_t id, std::string name); //This is possible, would be an nice API but require adding to move OpcUa::Server::AddressSpaceRegistry to common.  and later implement in client ...
      Node AddVariable(uint16_t ns, uint32_t id, std::string name);

    private:
      OpcUa::Remote::Computer::SharedPtr server;
      bool mIsNull = true;
      NodeID NodeId;
      QualifiedName browseName ;
      QualifiedName ParseQualifiedNameFromString(const std::string& str, ushort default_ns);
      friend std::ostream& operator<<(std::ostream& os, const Node& node){
        os << node.ToString();
        return os;
      //OpcUa::Server::AddressSpaceRegistry:: registry
        
      }

  };
} // namespace OpcUa

#endif
