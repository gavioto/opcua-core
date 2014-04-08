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
  class Node
  {
    public:
      Node( const OpcUa::Remote::Computer::SharedPtr& server, const NodeID& nodeid ) {
        this->server = server;
        this->NodeId = nodeid;
        mIsNull = false;
      }
      Node( const OpcUa::Remote::Computer::SharedPtr& server){ this->server = server; mIsNull = true; };
      //~Node(){};

      bool isNull() const { return mIsNull; }
      NodeID GetNodeId() const {return NodeId;}

      //The tree base Opc-Ua methods
      //std::vector<Node> PyBrowse() {return this->Browse(OpcUa::ReferenceID::HierarchicalReferences);}
      std::vector<Node> Browse() {return this->Browse(OpcUa::ReferenceID::HierarchicalReferences);}
      std::vector<Node> Browse(OpcUa::ReferenceID refid);
      Variant Read(OpcUa::AttributeID attr);
      StatusCode Write(OpcUa::AttributeID attr, const Variant &val);
      //std::vector<StatusCode> WriteAttrs(OpcUa::AttributeID attr, const Variant &val);

      Node GetChildNode (std::vector<QualifiedName> const &path);

      //Helper methods
      Node GetChildNode (ushort ns, const std::string &browsename);
      Node GetChildNode (const QualifiedName &browsename);
      // a path element is either a string of format ns:browsename or simple browsename and namespace is dedices from preceding element or node namespace
      Node GetChildNode (const std::vector<std::string> &path); //assume namespace is same as parent
      Node GetChildNode (const std::string &browsename) {return GetChildNode(this->browseName.NamespaceIndex, browsename);}

      StatusCode WriteValue(const Variant &variant);
      Variant ReadValue();
      Variant ReadDataType();
      void SetBrowseNameCache(const QualifiedName &browsename){this->browseName= browsename;}  
      void WriteBrowseName (const QualifiedName &browsename); 
      QualifiedName GetBrowseName() const { return browseName; }
      std::vector<Node> GetProperties() {return Browse(OpcUa::ReferenceID::HasProperty);}
      std::vector<Node> GetChildren() {return Browse();}
      std::vector<Node> GetVariables() {return Browse(OpcUa::ReferenceID::HasComponent);} //Not correct should filter by variable type


      std::string ToString() const; 
      explicit operator bool() const {return !mIsNull;}

      bool operator==(Node const& x) const { return NodeId == x.NodeId; }
      bool operator!=(Node const& x) const { return NodeId != x.NodeId; }


    private:
      OpcUa::Remote::Computer::SharedPtr server;
      bool mIsNull = true;
      NodeID NodeId;
      QualifiedName browseName ;
      QualifiedName ParseQualifiedNameFromString(const std::string& str, ushort default_ns);
      friend std::ostream& operator<<(std::ostream& os, const Node& node){
        os << node.ToString();
        return os;
      }

  };
} // namespace OpcUa

#endif
