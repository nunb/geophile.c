#ifndef _RECORDARRAY_H
#define _RECORDARRAY_H

#include "OrderedIndex.h"
#include "Record.h"
#include "ByteBuffer.h"
#include "Cursor.h"

namespace geophile
{
    class SpatialObjectKey;
    class SpatialObjectTypes;

    class RecordArray : public OrderedIndex
    {
    public:
        // Index
        virtual void add(Z z, const SpatialObject* spatial_object);
        virtual int32_t remove(Z z, int64_t soid);
        virtual void freeze();
        virtual Cursor* cursor();
        virtual ~RecordArray();
        // RecordArray
        // If return value is negative, _start_key is missing, 
        // and insert position is -returnvalue - 1.
        int32_t position(const SpatialObjectKey& key, 
                         int32_t forward_move, 
                         int32_t include_key) const;
        uint32_t nRecords() const;
        Record at(int32_t position) const;
        SpatialObject* copySpatialObject(const SpatialObject* spatial_object);
        RecordArray(const SpatialObjectTypes* spatial_object_types, uint32_t capacity);
        
    private:
        // serialize and deserialize use _buffer (which is why this class
        // is not threadsafe).
        void serialize(const SpatialObject* spatial_object);
        SpatialObject* deserialize();
        void growBuffer();
        
    private:
        static int32_t recordCompare(const void* x, const void* y);
        
    private:
        static const uint32_t INITIAL_BUFFER_SIZE = 1000;
        
    private:
        uint32_t _capacity;
        int32_t _n;
        Record* _records;
        uint32_t _buffer_size;
        byte* _buffer;
    };

    class RecordArrayCursor : public Cursor
    {
    public:
        virtual Record next();
        virtual Record previous();
        virtual void goTo(const SpatialObjectKey& key);
        RecordArrayCursor(RecordArray& record_array);
        
    private:
        Record neighbor(int32_t forward_move);
        void startIteration(int32_t forward_move, int32_t include_start_key);
        
    private:
        RecordArray& _record_array;
        int32_t _position;
        SpatialObjectKey _start_at;
        int32_t _forward;
    };
}

#endif
