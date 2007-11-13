#ifndef _ManagementObject_
#define _ManagementObject_

/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "Manageable.h"
#include "qpid/sys/Time.h"
#include <qpid/framing/Buffer.h>
#include <boost/shared_ptr.hpp>
#include <map>

namespace qpid { 
namespace management {

const uint16_t OBJECT_SYSTEM      = 1;
const uint16_t OBJECT_BROKER      = 2;
const uint16_t OBJECT_VHOST       = 3;
const uint16_t OBJECT_QUEUE       = 4;
const uint16_t OBJECT_EXCHANGE    = 5;
const uint16_t OBJECT_BINDING     = 6;
const uint16_t OBJECT_CLIENT      = 7;
const uint16_t OBJECT_SESSION     = 8;
const uint16_t OBJECT_DESTINATION = 9;
const uint16_t OBJECT_PRODUCER    = 10;
const uint16_t OBJECT_CONSUMER    = 11;

class Manageable;

class ManagementObject
{
  protected:
    
    uint64_t    createTime;
    uint64_t    destroyTime;
    uint64_t    objectId;
    bool        configChanged;
    bool        instChanged;
    bool        deleted;
    Manageable* coreObject;
    
    static const uint8_t TYPE_UINT8  = 1;
    static const uint8_t TYPE_UINT16 = 2;
    static const uint8_t TYPE_UINT32 = 3;
    static const uint8_t TYPE_UINT64 = 4;
    static const uint8_t TYPE_BOOL   = 5;
    static const uint8_t TYPE_STRING = 6;

    static const uint8_t FLAG_CONFIG = 0x01;
    static const uint8_t FLAG_INDEX  = 0x02;
    static const uint8_t FLAG_END    = 0x80;
    
    void schemaItem      (qpid::framing::Buffer& buf,
                          uint8_t     typeCode,
                          std::string name,
                          std::string description,
                          bool        isConfig = false,
                          bool        isIndex  = false);
    void schemaListBegin (qpid::framing::Buffer& buf);
    void schemaListEnd   (qpid::framing::Buffer& buf);
    void writeTimestamps (qpid::framing::Buffer& buf);

  public:
    typedef boost::shared_ptr<ManagementObject> shared_ptr;

    ManagementObject (Manageable* _core) :
        destroyTime(0), objectId (0), configChanged(true),
        instChanged(true), deleted(false), coreObject(_core)
    { createTime = uint64_t (qpid::sys::Duration (qpid::sys::now ())); }
    virtual ~ManagementObject () {}

    virtual uint16_t    getObjectType        (void) = 0;
    virtual std::string getObjectName        (void) = 0;
    virtual void        writeSchema          (qpid::framing::Buffer& buf) = 0;
    virtual void        writeConfig          (qpid::framing::Buffer& buf) = 0;
    virtual void        writeInstrumentation (qpid::framing::Buffer& buf) = 0;
    virtual bool        getSchemaNeeded      (void) = 0;
    virtual void        setSchemaNeeded      (void) = 0;
    virtual void        doMethod             (std::string            methodName,
                                              qpid::framing::Buffer& inBuf,
                                              qpid::framing::Buffer& outBuf) = 0;

    void         setObjectId      (uint64_t oid) { objectId = oid; }
    uint64_t     getObjectId      (void) { return objectId; }
    inline  bool getConfigChanged (void) { return configChanged; }
    virtual bool getInstChanged   (void) { return instChanged; }
    inline  void setAllChanged    (void)
    {
        configChanged = true;
        instChanged   = true;
    }

    inline void resourceDestroy  (void) {
        destroyTime = uint64_t (qpid::sys::Duration (qpid::sys::now ()));
        deleted     = true;
    }
    bool isDeleted (void) { return deleted; }

};

 typedef std::map<uint64_t,ManagementObject::shared_ptr> ManagementObjectMap;

}}
            


#endif  /*!_ManagementObject_*/
