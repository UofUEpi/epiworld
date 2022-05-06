#ifndef EPIWORLD_VIRUS_MEAT_HPP
#define EPIWORLD_VIRUS_MEAT_HPP

template<typename TSeq>
inline Virus<TSeq>::Virus(std::string name) {
    set_name(name);
}

template<typename TSeq>
inline Virus<TSeq>::Virus(const Virus<TSeq> & v)
{

    // Basic data
    this->host              = v.host;
    this->baseline_sequence = v.baseline_sequence;
    this->virus_name        = v.virus_name;
    this->date              = v.date;
    this->id                = v.id;

    // Functions
    this->mutation_fun                 = v.mutation_fun;
    this->post_recovery_fun            = v.post_recovery_fun;
    this->probability_of_infecting_fun = v.probability_of_infecting_fun;
    this->probability_of_recovery_fun  = v.probability_of_recovery_fun;
    this->probability_of_death_fun     = v.probability_of_death_fun;

    // Setup parameters
    this->params = v.params;
    this->data   = v.data;

}

template<typename TSeq>
inline Virus<TSeq>::Virus(Virus<TSeq> && v)
{

    // Basic data
    this->host = v.host;
    v.host = nullptr;
    
    this->baseline_sequence = std::move(v.baseline_sequence);
    v.baseline_sequence = nullptr;

    this->virus_name = std::move(v.virus_name);
    this->date       = std::move(v.date);
    this->id         = std::move(v.id);

    // Functions
    this->mutation_fun                 = std::move(v.mutation_fun);
    this->post_recovery_fun            = std::move(v.post_recovery_fun);
    this->probability_of_infecting_fun = std::move(v.probability_of_infecting_fun);
    this->probability_of_recovery_fun  = std::move(v.probability_of_recovery_fun);
    this->probability_of_death_fun     = std::move(v.probability_of_death_fun);

    // Setup parameters
    this->params = v.params;
    this->data   = std::move(v.data);

}

template<typename TSeq>
inline Virus<TSeq> &  Virus<TSeq>::operator=(const Virus<TSeq> & v)
{

    // Basic data
    this->host              = v.host;
    this->baseline_sequence = v.baseline_sequence;
    this->virus_name        = v.virus_name;
    this->date              = v.date;
    this->id                = v.id;

    // Functions
    this->mutation_fun                 = v.mutation_fun;
    this->post_recovery_fun            = v.post_recovery_fun;
    this->probability_of_infecting_fun = v.probability_of_infecting_fun;
    this->probability_of_recovery_fun  = v.probability_of_recovery_fun;
    this->probability_of_death_fun     = v.probability_of_death_fun;

    // Setup parameters
    this->params = v.params;
    this->data   = v.data;

    return *this;

}

// template<typename TSeq>
// inline Virus<TSeq>::Virus(TSeq sequence, std::string name) {
//     baseline_sequence = std::make_shared<TSeq>(sequence);
//     set_name(name);
// }

template<typename TSeq>
inline void Virus<TSeq>::mutate() {

    if (mutation_fun)
        if (mutation_fun(host, this, this->get_model()))
            host->get_model()->record_variant(this);

    return;
}

template<typename TSeq>
inline void Virus<TSeq>::set_mutation(
    MutFun<TSeq> fun
) {
    mutation_fun = MutFun<TSeq>(fun);
}

template<typename TSeq>
inline const TSeq * Virus<TSeq>::get_sequence() {
    return &(*baseline_sequence);
}

template<typename TSeq>
inline void Virus<TSeq>::set_sequence(TSeq sequence) {
    baseline_sequence = std::make_shared<TSeq>(sequence);
    return;
}

template<typename TSeq>
inline Person<TSeq> * Virus<TSeq>::get_host() {
    return host;
}

template<typename TSeq>
inline Model<TSeq> * Virus<TSeq>::get_model() {
    return host->get_model();
}

template<typename TSeq>
inline void Virus<TSeq>::set_id(int idx) {
    id = idx;
    return;
}

template<typename TSeq>
inline int Virus<TSeq>::get_id() const {
    
    return id;
}

template<typename TSeq>
inline void Virus<TSeq>::set_date(int d) {
    date = d;
    return;
}

template<typename TSeq>
inline int Virus<TSeq>::get_date() const {
    
    return date;
}


template<typename TSeq>
inline void Virus<TSeq>::rm(epiworld_fast_uint next_status)
{

    Model<TSeq> * model = get_model();

    // Downcount the next exposed for this virus
    model->get_db().down_exposed(this, host->status);

    // Down count the status and upcount the next status
    model->get_db().state_change(host->get_status(), next_status);

    // Will update the status of the person
    host->update_status(next_status);

    // Finally, downcount the number of active viruses and
    // add the virus to the list.
    host->get_viruses().nactive--;
    model->virus_to_remove.push_back(this);

    if (model->is_queuing_on())
        model->get_queue() -= host;

}



template<typename TSeq>
inline epiworld_double Virus<TSeq>::get_prob_infecting()
{

    if (probability_of_infecting_fun)
        return probability_of_infecting_fun(host, this, host->get_model());
        
    return EPI_DEFAULT_VIRUS_PROB_INFECTION;

}



template<typename TSeq>
inline epiworld_double Virus<TSeq>::get_prob_recovery()
{

    if (probability_of_recovery_fun)
        return probability_of_recovery_fun(host, this, host->get_model());
        
    return EPI_DEFAULT_VIRUS_PROB_RECOVERY;

}



template<typename TSeq>
inline epiworld_double Virus<TSeq>::get_prob_death()
{

    if (probability_of_death_fun)
        return probability_of_death_fun(host, this, host->get_model());
        
    return EPI_DEFAULT_VIRUS_PROB_DEATH;

}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_infecting_fun(VirusFun<TSeq> fun)
{
    probability_of_infecting_fun = fun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_recovery_fun(VirusFun<TSeq> fun)
{
    probability_of_recovery_fun = fun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_death_fun(VirusFun<TSeq> fun)
{
    probability_of_death_fun = fun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_infecting(epiworld_double * prob)
{
    VirusFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            return *prob;
        };
    
    probability_of_infecting_fun = tmpfun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_recovery(epiworld_double * prob)
{
    VirusFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            return *prob;
        };
    
    probability_of_recovery_fun = tmpfun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_death(epiworld_double * prob)
{
    VirusFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            return *prob;
        };
    
    probability_of_death_fun = tmpfun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_infecting(epiworld_double prob)
{
    VirusFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            return prob;
        };
    
    probability_of_infecting_fun = tmpfun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_recovery(epiworld_double prob)
{
    VirusFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            return prob;
        };
    
    probability_of_recovery_fun = tmpfun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_prob_death(epiworld_double prob)
{
    VirusFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            return prob;
        };
    
    probability_of_death_fun = tmpfun;
}

template<typename TSeq>
inline void Virus<TSeq>::set_post_recovery(PostRecoveryFun<TSeq> fun)
{
    if (post_recovery_fun)
    {
        printf_epiworld(
            "Warning: a PostRecoveryFun is alreay in place (overwriting)."
            );
    }

    post_recovery_fun = fun;
}

template<typename TSeq>
inline void Virus<TSeq>::post_recovery()
{

    if (post_recovery_fun)
        return post_recovery_fun(host, this, host->get_model());    

    return;
        
}

template<typename TSeq>
inline void Virus<TSeq>::set_post_immunity(
    epiworld_double prob
)
{

    if (post_recovery_fun)
    {

        std::string msg =
            std::string(
                "You cannot set post immunity when a post_recovery "
                ) +
            std::string(
                "function is already in place. Redesign the post_recovery function."
                );

        throw std::logic_error(msg);
        
    }

    PostRecoveryFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            Tool<TSeq> no_reinfect(
                "No reinfect virus " +
                std::to_string(v->get_id())
                );
            
            no_reinfect.set_susceptibility_reduction(prob);
            no_reinfect.set_death_reduction(0.0);
            no_reinfect.set_transmission_reduction(0.0);
            no_reinfect.set_recovery_enhancer(0.0);

            p->add_tool(m->today(), no_reinfect);

            return;

        };

    post_recovery_fun = tmpfun;

}

template<typename TSeq>
inline void Virus<TSeq>::set_post_immunity(
    epiworld_double * prob
)
{

    if (post_recovery_fun)
    {

        std::string msg =
            std::string(
                "You cannot set post immunity when a post_recovery "
                ) +
            std::string(
                "function is already in place. Redesign the post_recovery function."
                );

        throw std::logic_error(msg);

    }

    PostRecoveryFun<TSeq> tmpfun = 
        [prob](Person<TSeq> * p, Virus<TSeq> * v, Model<TSeq> * m)
        {
            Tool<TSeq> no_reinfect(
                "No reinfect virus " +
                std::to_string(v->get_id())
                );
            
            no_reinfect.set_susceptibility_reduction(*prob);
            no_reinfect.set_death_reduction(0.0);
            no_reinfect.set_transmission_reduction(0.0);
            no_reinfect.set_recovery_enhancer(0.0);

            p->add_tool(m->today(), no_reinfect);

            return;

        };

    post_recovery_fun = tmpfun;

}

template<typename TSeq>
inline void Virus<TSeq>::set_name(std::string name)
{

    if (name == "")
        virus_name = nullptr;
    else
        virus_name = std::make_shared<std::string>(name);

}

template<typename TSeq>
inline std::string Virus<TSeq>::get_name() const
{

    if (virus_name)
        return *virus_name;
    
    return "unknown virus";

}

template<typename TSeq>
inline std::vector< epiworld_double > & Virus<TSeq>::get_data() {
    return data;
}

#endif
