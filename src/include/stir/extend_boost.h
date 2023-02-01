#include <boost/unordered_map.hpp>
namespace boost{
    template <typename T> using coord = std::vector<T>;

    template <typename T> struct ihash
        : std::unary_function<coord<T>, std::size_t>
    {
        std::size_t operator()(coord<T> const& e) const
        {
            return boost::hash_range(e.begin(), e.end());
        }
    };

    template <typename T> struct iequal_to
        : std::binary_function<coord<T>, coord<T>, bool>
    {
        bool operator()(coord<T> const& x, coord<T> const& y) const
        {
            return (x==y);
        }
    };
    template <typename K, typename V> using coord_map = boost::unordered_map< coord<K>, coord<V>, ihash<K>, iequal_to<K> >;
}
