#ifndef EPIWORLD_ENTITIES_BONES_HPP
#define EPIWORLD_ENTITIES_BONES_HPP

template<typename TSeq>
class Virus;

template<typename TSeq>
class Agent;


/**
 * @brief Set of Entities (useful for building iterators)
 * 
 * @tparam TSeq 
 */
template<typename TSeq>
class Entities {
    friend class Entity<TSeq>;
    friend class Agent<TSeq>;
private:
    std::vector< Entity<TSeq>* > * dat;
    const epiworld_fast_uint * n_entities;

public:

    Entities() = delete;
    Entities(Agent<TSeq> & p) : dat(&p.entities), n_entities(&p.n_entities) {};

    typename std::vector< Entity<TSeq>* >::iterator begin();
    typename std::vector< Entity<TSeq>* >::iterator end();

    Entity<TSeq>* & operator()(size_t i);
    Entity<TSeq>* & operator[](size_t i);

    size_t size() const noexcept;

};

template<typename TSeq>
inline typename std::vector< Entity<TSeq>* >::iterator Entities<TSeq>::begin()
{

    if (*n_entities == 0u)
        return dat->end();
    
    return dat->begin();
}

template<typename TSeq>
inline typename std::vector< Entity<TSeq>* >::iterator Entities<TSeq>::end()
{
     
    return begin() + *n_entities;
}

template<typename TSeq>
inline Entity<TSeq>* & Entities<TSeq>::operator()(size_t i)
{

    if (i >= *n_entities)
        throw std::range_error("Entity index out of range.");

    return dat->operator[](i);

}

template<typename TSeq>
inline Entity<TSeq>* & Entities<TSeq>::operator[](size_t i)
{

    return dat->operator[](i);

}

template<typename TSeq>
inline size_t Entities<TSeq>::size() const noexcept 
{
    return *n_entities;
}

/**
 * @brief Set of Entities (const) (useful for iterators)
 * 
 * @tparam TSeq 
 */
template<typename TSeq>
class Entities_const {
    friend class Virus<TSeq>;
    friend class Agent<TSeq>;
private:
    const std::vector< Entity<TSeq>* > * dat;
    const epiworld_fast_uint * n_entities;

public:

    Entities_const() = delete;
    Entities_const(const Agent<TSeq> & p) : dat(&p.entities), n_entities(&p.n_entities) {};

    typename std::vector< Entity<TSeq>* >::const_iterator begin();
    typename std::vector< Entity<TSeq>* >::const_iterator end();

    const Entity<TSeq>* & operator()(size_t i);
    const Entity<TSeq>* & operator[](size_t i);

    size_t size() const noexcept;

};

template<typename TSeq>
inline typename std::vector< Entity<TSeq>* >::const_iterator Entities_const<TSeq>::begin() {

    if (*n_entities == 0u)
        return dat->end();
    
    return dat->begin();
}

template<typename TSeq>
inline typename std::vector< Entity<TSeq>* >::const_iterator Entities_const<TSeq>::end() {
     
    return begin() + *n_entities;
}

template<typename TSeq>
inline const Entity<TSeq>* & Entities_const<TSeq>::operator()(size_t i)
{

    if (i >= *n_entities)
        throw std::range_error("Entity index out of range.");

    return dat->operator[](i);

}

template<typename TSeq>
inline const Entity<TSeq>* & Entities_const<TSeq>::operator[](size_t i)
{

    return dat->operator[](i);

}

template<typename TSeq>
inline size_t Entities_const<TSeq>::size() const noexcept 
{
    return *n_entities;
}


#endif