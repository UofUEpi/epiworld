#ifndef EPIWORLD_PERSONVIRUSES_BONES_HPP
#define EPIWORLD_PERSONVIRUSES_BONES_HPP

template<typename TSeq>
class Virus;

/**
 * @brief Set of viruses in host
 * 
 * @tparam TSeq Type of sequence
 */
template<typename TSeq = bool>
class PersonViruses {
    friend class Person<TSeq>;
    friend class Model<TSeq>;
    friend class Virus<TSeq>;
private:
    Person<TSeq> * host;
    std::vector< std::shared_ptr< Virus<TSeq> > > viruses;
    int nactive = 0;

public:

    PersonViruses();

    void add_virus(epiworld_fast_uint new_status, Virus<TSeq> v);
    size_t size() const;
    int size_active() const;
    Virus<TSeq> & operator()(int i);
    void mutate();
    void reset();
    Person<TSeq> * get_host();
    bool has_virus(unsigned int v) const;
    bool has_virus(std::string vname) const;

};


#endif