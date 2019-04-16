#ifndef TRIATS_H
#define TRIATS_H

#include <type_traits>
#include <vector>
#include <list>
#include <set>

#define TIMER_START auto time1 = std::chrono::high_resolution_clock::now();
#define TIMER_END auto time2 = std::chrono::high_resolution_clock::now();\
    auto time_duration = std::chrono::duration_cast\
            <std::chrono::microseconds>\
                ( time2 - time1 ).count();\
    std::cout << "Elapsed time: " << time_duration\
    << std::endl;

#define PRETTY_TYPE(Type) std::cout << \
    boost::typeindex::type_id_with_cvr<Type>()\
    .pretty_name()<< std::endl;

namespace utility
{


//template<typename, typename = std::void_t<>>
//struct has_key_method : std::false_type 
//{};
    
//template<typename T>
//struct has_key_method<T, 
       //std::void_t<decltype(std::declval<T>().key())>> 
       //: std::true_type
//{};
#define DEFINE_HAS_MEMBER(Member) \
template<typename, typename = std::void_t<>> \
struct HasMemberT_##Member : std::false_type { }; \
template<typename T> struct HasMemberT_##Member<T,\
std::void_t<decltype(&T::Member)>> : std::true_type { };\
template<typename T> \
using has_member_##Member_v \
= typename HasMemberT_##Member<T>::value;



template <typename T>
struct is_sequence_container : std::false_type
{};

template<typename... Args>
struct is_sequence_container<std::vector<Args...>> : std::true_type
{};

template<typename... Args>
struct is_sequence_container<std::list<Args...>> : std::true_type
{};

template<typename... Args>
struct is_sequence_container<std::set<Args...>> : std::true_type
{};


}



#endif


