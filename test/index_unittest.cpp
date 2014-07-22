#include <assert.h>
#include <stdio.h>

#include "geophile/testbase.h"
#include "geophile/SpatialObjectPointer.h"
#include "geophile/InMemorySpatialObjectMemoryManager.h"
#include "RecordArray.h"

using namespace geophile;

static SessionMemory<SpatialObjectPointer> memory;

class RecordArrayFactory : public OrderedIndexFactory<SpatialObjectPointer>
{
public:
    virtual OrderedIndex<SpatialObjectPointer>* newIndex
    (const SpatialObjectTypes* spatial_object_types) const
    {
        return new RecordArray<SpatialObjectPointer>(spatial_object_types, 
                                                     &_spatial_object_memory_manager,
                                                     &memory);
    }

private:
    InMemorySpatialObjectMemoryManager _spatial_object_memory_manager;
};

RecordArrayFactory RECORD_ARRAY_FACTORY;

int main(int32_t argc, const char** argv)
{
    runTests(&RECORD_ARRAY_FACTORY);
}
