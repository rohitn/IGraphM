#ifndef IGCOMMON_H
#define IGCOMMON_H

extern "C" { // workaround for igraph_version() C++ compatibility bug in igraph <= 0.7.1
#include <igraph/igraph.h>
}

#include "mlstream.h"
#include "LTemplate.h"

#include <algorithm>
#include <string>
#include <sstream>


inline igraph_vector_t igVectorView(mma::RealTensorRef t) {
    static double dummy = 0.0; // work around igraph not liking zero-length vectors will NULL pointers
    igraph_vector_t vec;
    mint len = t.length();
    igraph_vector_view(&vec, len == 0 ? &dummy : t.data(), len);
    return vec;
}


struct igVector {
    igraph_vector_t vec;

    igVector() { igraph_vector_init(&vec, 0); }
    ~igVector() { igraph_vector_destroy(&vec); }

    long length() const { return vec.stor_begin - vec.end; }

    igraph_real_t *begin() { return vec.stor_begin; }
    igraph_real_t *end() { return vec.end; }

    const igraph_real_t *begin() const { return vec.stor_begin; }
    const igraph_real_t *end() const { return vec.end; }

    void clear() { igraph_vector_clear(&vec); }

    void copyFromMTensor(mma::RealTensorRef t) {
        igraph_vector_t from = igVectorView(t);
        igraph_vector_update(&vec, &from);
    }

    mma::RealTensorRef makeMTensor() const {
        mma::RealTensorRef res = mma::makeVector<double>(length());
        std::copy(begin(), end(), res.begin());
        return res;
    }
};


struct igMatrix {
    igraph_matrix_t mat;

    igMatrix() { igraph_matrix_init(&mat, 0, 0); }
    ~igMatrix() { igraph_matrix_destroy(&mat); }

    long length() const { return mat.data.end - mat.data.stor_begin; }
    long nrow() const { return mat.nrow; }
    long ncol() const { return mat.ncol; }

    igraph_real_t *begin() { return mat.data.stor_begin; }
    igraph_real_t *end() { return mat.data.end; }

    const igraph_real_t *begin() const { return mat.data.stor_begin; }
    const igraph_real_t *end() const { return mat.data.end; }

    mma::RealMatrixRef makeMTensor() const {
        mma::RealMatrixRef res = mma::makeMatrix<double>(mat.nrow, mat.ncol);
        std::copy(begin(), end(), res.begin());
        return res;
    }
};


struct igList {
    igraph_vector_ptr_t list;

    igList() { igraph_vector_ptr_init(&list, 0); }
    ~igList() { igraph_vector_ptr_destroy_all(&list); }

    long length() const { return igraph_vector_ptr_size(&list); }
};


inline mlStream & operator << (mlStream &ml, const igraph_vector_t &vec) {
    if (! MLPutReal64List(ml.link(), vec.stor_begin, vec.end - vec.stor_begin))
        ml.error("cannot return vector");
    return ml;
}


inline mlStream & operator << (mlStream &ml, const igList &list) {
    long len = list.length();
    if (! MLPutFunction(ml.link(), "List", len))
        ml.error("cannot return vector list");
    for (int i=0; i < len; ++i)
        ml << *static_cast<igraph_vector_t *>(VECTOR(list.list)[i]);
    return ml;
}


inline void igCheck(int err) {
    if (! err) return;
    std::ostringstream msg;
    msg << "igraph returned with error: " << igraph_strerror(err);
    throw mma::LibraryError(msg.str());
}


#endif // IGCOMMON_H
