//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef COMMON_INTERNAL_RESOURCEATTRIBUTESUTILS_H
#define COMMON_INTERNAL_RESOURCEATTRIBUTESUTILS_H

namespace OIC
{
    namespace Service
    {
        bool acceptableAttributes(const ResourceAttributes& dest, const ResourceAttributes& attr);

        bool acceptableAttributeValue(const ResourceAttributes::Value& dest,
                const ResourceAttributes::Value& value);

        void replaceAttributesRecursively(ResourceAttributes& dest, const ResourceAttributes& attr);

        void replaceAttributeValueRecursively(ResourceAttributes::Value& dest,
                const ResourceAttributes::Value& value);
    }
}

#endif // COMMON_INTERNAL_RESOURCEATTRIBUTESUTILS_H
