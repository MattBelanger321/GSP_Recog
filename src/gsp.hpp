#ifndef GSP_H
#define GSP_H

#include "SPandOP.hpp"
#include <limits>
using namespace std;

Certificate GSP_verify(graph_t &g2, const size_t size);

template <typename T, size_t size>
bool contains_pair(const std::array<std::vector<std::pair<T,T>>, size> &vec, const T s, const T t)
{
    const auto e1= std::make_pair(s, t);
    const auto e2 = std::make_pair(t,s);
    for(auto v:vec)
    {
        for(auto e:v)
        {
            if(e ==e1 || e==e2)
                return true;
        }
    }
    return false;
}
template <typename T, size_t size>
void remove_pair(std::array<std::vector<std::pair<T,T>>, size> &vec, const T s, const T t)
{
    size_t edge_index;
    auto found_edge = false;
    for(auto& v:vec)
    {
        found_edge = false;
        for(size_t i=0;i <v.size();i++)
        {
            auto e1= std::make_pair(s, t);
            auto e2 = std::make_pair(t,s);
            if(v[i] ==e1 || v[i]==e2){
                edge_index=i;
                found_edge=true;
            }
        }
        if (found_edge)
        {
            std::vector<std::pair<T,T>> temp;
             for(size_t i=0;i <v.size();i++)
             {
                 if (i==edge_index)
                 {
                     continue;
                 }
                 else
                 {
                     temp.push_back(v[i]);
                 }
             }
             v=temp;
        }
    }
}


#endif
