#ifndef _SPATIAL_INDEX_H
#define _SPATIAL_INDEX_H

#include <stdint.h>
#include "Space.h"
#include "SpatialIndex.h"
#include "SpatialObject.h"
#include "OrderedIndex.h"
#include "SessionMemory.h"
#include "SpatialIndexScan.h"
#include "ZArray.h"
#include "util.h"

namespace geophile
{
    template <class SOR> class OrderedIndex;
    template <class SOR> class SpatialIndexScan;
    template <class SOR> class SpatialObjectReferenceManager;
    class Space;
    class SpatialIndexFilter;
    class SpatialObject;

    /*
     * A SpatialIndex organizes a set of SpatialObjects for the
     * efficient execution of spatial searches.
     */
    template <class SOR>
    class SpatialIndex
    {
    public:
        /*
         * Space containing the indexed SpatialObjects.
         */
        const Space* space() const
        {
            return _space;
        }

        /*
         * Adds spatial_object to this SpatialIndex. memory contains
         * resources used internally.
         */
        void add(const SpatialObject* spatial_object, SessionMemory<SOR>* memory)
        {
            GEOPHILE_ASSERT(spatial_object->id() != SpatialObject::UNINITIALIZED_ID);
            ZArray* zs = memory->zArray();
            zs->clear();
            _space->decompose(spatial_object, spatial_object->maxZ(), memory);
            for (uint32_t i = 0; i < zs->length(); i++) {
                _index->add(zs->at(i), 
                            _spatial_object_reference_manager->newSpatialObjectReference(spatial_object));
            }
        }

        /*
         * Removes spatial_object to this SpatialIndex. memory contains
         * resources used internally.
         */
        int32_t remove(const SpatialObject& spatial_object, SessionMemory<SOR>* memory)
        {
            // TBD
            GEOPHILE_ASSERT(false); 
            return false;
        }

        /*
         * Prepares this SpatialIndex for retrieval.
         */
        void freeze()
        {
            _index->freeze();
        }

        /*
         * Find all the SpatialObjects in this SpatialIndex that
         * overlap spatial_object.  Spatial index retrieval may return
         * false positives, which are removed by the filter.
         * The results are returned in memory->output(). 
         */
        void findOverlapping(const SpatialObject* query_object, 
                             const SpatialIndexFilter* filter,
                             SessionMemory<SOR>* memory) const
        {
            _space->decompose(query_object, query_object->maxZ(), memory);
            SpatialIndexScan<SOR>* scan = newScan(query_object, filter, memory);
            ZArray* zs = memory->zArray();
            for (uint32_t i = 0; i < zs->length(); i++) {
                scan->find(zs->at(i));
            }
            delete scan;
        }

        /*
         * Constructor.
         *     space: The Space containing the SpatialObjects to be indexed.
         *     index: The OrderedIndex that will contain records of the spatial index.
         */
        SpatialIndex(const Space* space, 
                     OrderedIndex<SOR>* index,
                     SpatialObjectReferenceManager<SOR>* spatial_object_reference_manager)
            : _space(space),
              _index(index),
              _spatial_object_reference_manager(spatial_object_reference_manager)
            {}

    public: // Not part of the API. Public for testing.
        SpatialIndexScan<SOR>* newScan(const SpatialObject* query_object,
                                       const SpatialIndexFilter* filter, 
                                       SessionMemory<SOR>* memory) const
        {
            return new SpatialIndexScan<SOR>(_index, 
                                             query_object, 
                                             filter,
                                             _spatial_object_reference_manager,
                                             (OutputArray<SOR>*) memory->output());
        }

    private:
        const Space* _space;
        OrderedIndex<SOR>* _index;
        SpatialObjectReferenceManager<SOR>* _spatial_object_reference_manager;
    };
}

#endif
