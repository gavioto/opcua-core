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

#pragma once

#include <opc/ua/server.h>

#include <sstream>


namespace OpcUa
{
  /// @brief A Node object represent an OPC-UA node.
  /// It is high level object intended for developper who want to expose
  /// data through OPC-UA or read data from an OPCUA server.
  /// Node are usually not create directly but obtained through call
  /// to GetRootNode of GetObjectsNode on server or client side

  class Node
  {
  public:
    // Creating Root Node.
    explicit Node(Remote::Server& srv);
    Node(Remote::Server& srv, const NodeID& id);
    Node(Remote::Server& srv, const NodeID& id, const QualifiedName& name);

    NodeID GetId() const;

    QualifiedName GetName() const;
    void SetName(const QualifiedName& name);

    /// @brief List childrenn nodes by specified reference
    /// @return One or zero chilren nodes.
    std::vector<Node> GetChildren(OpcUa::ReferenceID refid) const;

    /// @brief Get ghildren by hierarchal referencies.
    /// @return One or zero chilren nodes.
    std::vector<Node> GetChildren() const;

    //The GetChildNode methods return a node defined by its path from the node. A path is defined by
    // a sequence of browse name(QualifiedName). A browse name is either defined through a qualifiedname object
    // or a string of format namespace:browsename. If a namespace is not specified it is assumed to be
    //the same as the parent
    Node GetChild(const std::vector<OpcUa::QualifiedName>& path) const;
    Node GetChild(const std::vector<std::string>& path) const;
    Node GetChild(const std::string& browsename) const;

    std::vector<Node> GetProperties() const;
    std::vector<Node> GetVariables() const;

    //TODO: How to get Referencies?

    //The Read and Write methods read or write attributes of the node
    //FIXME: add possibility to read and write several nodes at once
    Variant GetAttribute(AttributeID attr) const;
    StatusCode SetAttribute(AttributeID attr, const Variant &val);
    //std::vector<StatusCode> WriteAttrs(OpcUa::AttributeID attr, const Variant &val);

    Variant GetValue() const;
    StatusCode SetValue(const Variant& value);

    Variant DataType() const;
    StatusCode WriteValue(const Variant &variant);

    //OpcUa low level methods to to modify address space model
    void AddAttribute(OpcUa::AttributeID attr, const OpcUa::Variant& val);

    void AddReference(const OpcUa::ReferenceDescription desc);

    //Helper classes to modify address space model
    Node AddFolder(const NodeID& folderId, const QualifiedName& browsename);
    Node AddFolder(const std::string& name);

    Node AddVariable(const NodeID& variableId, const QualifiedName& browsename, const Variant& val);
    Node AddVariable(const std::string& variableName, const Variant& val);

    Node AddProperty(const NodeID& propertyId, const QualifiedName& browsename, const Variant& val);
    Node AddProperty(const std::string& propertyName, const Variant& val);

    std::string ToString() const;

    bool operator==(Node const& x) const { return Id == x.Id; }
    bool operator!=(Node const& x) const { return Id != x.Id; }

  private:
    OpcUa::Remote::Server& Server;
    NodeID Id;
    QualifiedName BrowseName;
  };


  std::ostream& operator<<(std::ostream& os, const Node& node);

  QualifiedName ParseQualifiedNameFromString(uint16_t default_ns, const std::string& str);
  NodeID ParseNodeIdFromString(uint16_t default_ns, const std::string& str);
  ObjectID VariantTypeToDataType(VariantType vt);

} // namespace OpcUa
