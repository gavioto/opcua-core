/******************************************************************************
 *   Copyright (C) 2014-2014 by Sintef Raufoss Manufacturing                  *
 *   olivier.roulet@gmail.com                  *
 *                                          *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as          *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                          *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.              *
 *                                          *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                  *
 *   Free Software Foundation, Inc.,                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ******************************************************************************/

#ifndef NODE_H
#define NODE_H
#include <sstream>

#include <opc/ua/channel.h>
#include <opc/ua/computer.h>
#include <opc/common/addons_core/addon_manager.h>

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/session.h>


namespace OpcUa
{
    class Node
    {
        public:
            Node( const std::shared_ptr<OpcUa::Remote::Computer> server, const NodeID nodeid ) {
                this->server = server;
                this->NodeId = nodeid;
                mIsNull = false;
            }
            Node( const std::shared_ptr<OpcUa::Remote::Computer> server){ this->server = server; mIsNull = true; };
            //~Node(){};
            bool isNull(){ return mIsNull; }
            NodeID GetNodeId() {return NodeId;}

            //The tree common Opc-Ua methods
            std::vector<Node> Browse( OpcUa::ReferenceID refid=OpcUa::ReferenceID::HierarchicalReferences);
            DataValue Read(OpcUa::AttributeID const attr);
            std::vector<DataValue> ReadVector(OpcUa::AttributeID const attr);;
            std::vector<StatusCode> Write(OpcUa::AttributeID const attr, Variant val);
            //void Write(OpcUa::AttributeID const attr, const std::vector<Variant>);

            //Helper methods
            void WriteValue(Variant variant);
            Variant ReadValue();
            DataValue ReadDataType();
            //std::vector<DataValue> ReadValueVector();
            void SetBrowseNameCache(QualifiedName browsename){this->browseName= browsename;}  
            void WriteBrowseName(QualifiedName browsename); 
            QualifiedName GetBrowseName() { return browseName; }
            Node FindChildNode(std::string browsename, ushort ns=0);
            Node FindChildNode(QualifiedName browsename);

            void setProperty(std::string browsename, Variant val);
            void setPropertyValue(ushort ns, std::string browsename, Variant val);
            Variant getPropertyValue (std::string browsename);
            Variant getPropertyValue (std::string browsename, ushort ns);
            std::string ToString() const; 

            explicit operator bool() const {return !mIsNull;}


        private:
            std::shared_ptr<OpcUa::Remote::Computer> server;
            bool mIsNull = true;
            NodeID NodeId;
            QualifiedName browseName ;
            friend std::ostream& operator<<(std::ostream& os, const Node& node){
                os << node.ToString();
                return os;
            }

    };
} // namespace OpcUa

#endif
