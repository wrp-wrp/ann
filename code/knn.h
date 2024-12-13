// #include <vector>
// #include <cstddef>
// #include "hnsw.h"

// namespace KNNs {

// using namespace std;

// template<typename ValueType>
// class KNN
// {
// private:
//     hnsw<ValueType> hn;

// public:
//     using T = ValueType;
//     using CVPointer = const vector<T>*;
//     using BinData = vector<char>;
//     KNN(std::size_t len);
//     ~KNN();
//     CVPointer insert(const std::vector<T>& vec);
//     bool erase(CVPointer vec);
//     std::vector<CVPointer> search(const std::vector<T>& vec, std::size_t k) const;
//     BinData save() const;
//     void load(BinData data);
//     std::size_t length() const;
//     std::size_t size() const;
// };

// template<typename ValueType> KNN<ValueType>::KNN(std::size_t len) {
//     hn = hnsw<ValueType>(len);

// }

// template<typename ValueType> KNN<ValueType>::~KNN() {
    
// }

// template<typename ValueType> typename KNN<ValueType>::CVPointer KNN<ValueType>::insert(const std::vector<T>& vec) {
//     return hn.insert(vec);
// }

// template<typename ValueType> bool KNN<ValueType>::erase(CVPointer vec) {
//     return hn.erase(vec);
// }

// template<typename ValueType> std::vector<typename KNN<ValueType>::CVPointer> KNN<ValueType>::search(const std::vector<T>& vec, std::size_t k) const {
//     vector<CVPointer> res = hn.k_nn_search_pointer(vec, k);  //
//     return res;
// }

// template<typename ValueType> typename KNN<ValueType>::BinData KNN<ValueType>::save() const {
//     return hn.save_data();
// }

// template<typename ValueType> void KNN<ValueType>::load(BinData data) {
//     hn.read_data(data);
// }

// template<typename ValueType> std::size_t KNN<ValueType>::length() const {
//     return hn.A[0].size();
// }

// template<typename ValueType> std::size_t KNN<ValueType>::size() const {
//     return hn.A.size();
// }

// } // namespace KNNs

#include <vector>
#include <cstddef>
#include "hnsw.h"

namespace KNNs {

using namespace std;
using ull = unsigned long long;

template<typename ValueType>
class KNN
{
private :
    hnsw<ValueType> hn;
public:
    using T = ValueType;
    KNN(std::size_t len);
    ~KNN();
    void insert(const std::vector<T>& vec, ull& key);
    bool erase(const ull& key);
    std::vector<ull> search(const std::vector<T>& vec, std::size_t k) const;
    std::size_t length() const;
    std::size_t size() const;
};

template<typename ValueType> KNN<ValueType>::KNN(std::size_t len) {
    // hn = hnsw<ValueType>(len);
    hn = hnsw<ValueType>(len);
} 

template<typename ValueType> KNN<ValueType>::~KNN() {
    
}

template<typename ValueType> void KNN<ValueType>::insert(const std::vector<T>& vec, ull& key) {
    key = hn.insert(vec);
}

template<typename ValueType> bool KNN<ValueType>::erase(const ull& key) {
    return hn.erase(key);
}

template<typename ValueType> std::vector<ull> KNN<ValueType>::search(const std::vector<T>& vec, std::size_t k) const {
    vector<ull> res = hn.k_nn_search_pointer(vec, k);  //
    return res;
}

template<typename ValueType> std::size_t KNN<ValueType>::length() const {
    if (hn.A.size() == 0) {
        std :: cerr << "No data in the KNN!" << std :: endl;
        assert (hn.A.size() > 0);
    }
    return hn.A[0].size();
}

template<typename ValueType> std::size_t KNN<ValueType>::size() const {
    return hn.A.size();
}

} // namespace KNNs

using KNNs :: KNN;